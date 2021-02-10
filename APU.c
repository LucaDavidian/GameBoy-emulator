#include "APU.h"
#include "SDL2/SDL.h"
#include <stdint.h>

#define CLOCK_FREQUENCY                                      4194304
#define FRAME_SEQUENCER_FREQUENCY                                512
#define SAMPLING_FREQUENCY                                     44100
#define CYCLES_PER_SAMPLE      (CLOCK_FREQUENCY / SAMPLING_FREQUENCY)    

#define BUFFER_SIZE 4096 * 2

/******** channel 1 - square wave ********/
struct Channel1
{
	union
	{
		struct
		{
			uint8_t sweep_shift     : 3;         // sweep shift value (0 - 7) - sweep changes frequency as f(t +  1) = f(t) +/- f(t) >> sweep_shift
			uint8_t sweep_direction : 1;         // sweep direction (0: increase, 1: decrease)
			uint8_t sweep_period    : 3;         // sweep period - sweep changes frequency every sweep_period / 128 seconds
			uint8_t : 1;
		} bits;

		uint8_t reg;
	} sweep_register;                  //////////////// NR10 sweep register - 0xFF10

	union
	{
		struct
		{
			uint8_t sound_length : 6;            // sound length - binary value loaded in length counter is ~value + 1  = 63 + 1 - value
			uint8_t duty_cycle   : 2;            // duty cycle of square wave (0: 12.5% - 1: 25% - 2: 50% - 3: 75%) 
		} bits;

		uint8_t reg;
	} sound_length_and_duty_cycle;     //////////////// NR11 sound length and wave pattern/duty cycle register  - 0xFF11

	union
	{
		struct
		{
			uint8_t envelope_period        : 3;  // number of envelope sweeps (0 -7) - 0 == stop envelope
			uint8_t envelope_direction     : 1;  // envelope direction (0: decrease, 1: increase)
			uint8_t initial_volume         : 4;  // initial volume of envelope (0 - 15) - 0 == no sound and channel disabled
		} bits;

		uint8_t reg;
	} volume_envelope;                 ////////////////// NR12 volume envelope register - 0xFF12

	union
	{
		struct
		{
			uint8_t frequency_low : 8;           // lower 8 bits of timer frequency
		} bits;

		uint8_t reg;
	} frequency_low;                   //////////////// NR13 frequency low register - 0xFF13

	union
	{
		struct
		{
			uint8_t frequency_high         : 3;  // higher 8 bits of timer frequency
			uint8_t                        : 3;
			uint8_t counter_enable         : 1;  // enable length counter (0: length counter disabled - consecutive, 1: length counter enabled - sound is cut off when counter reaches 0)
			uint8_t initial                : 1;  // initial (1: restart sound)
		} bits;

		uint8_t reg;
	} frequency_high;                  //////////////// NR14 frequency high register - 0xFF14                         

	uint16_t frequency_timer;                    // 11-bit programmable timer, clocked by system clock @4.194304 MHz divided by 32 (5-bit divider) 
	uint8_t waveform_generator;

	uint8_t length_counter;                      // 6-bit length counter, clocked by frame sequencer @ 256 Hz

	uint8_t volume;                              // channel volume (0 - 15)
	uint8_t volume_sweep_counter;                // 3-bit volume sweep counter

	uint8_t sweep_timer;
	uint16_t sweep_shadow_frequency_register;

	uint8_t digital_output;                      // channel's 4-bit DAC - 16 steps of output voltage (0 -- 15)
	float DAC;                                   // DAC analog output (-1.0V -- 1.0V)
	int DAC_enabled;
};

/******** channel 2 - square wave ********/
struct Channel2
{
	union
	{
		struct
		{
			uint8_t sound_length : 6;            // sound length - binary value loaded in length counter is inverted + 1 (63 + 1 - value)
			uint8_t duty_cycle   : 2;            // duty cycle of square wave (0: 12.5% - 1: 25% - 2: 50% - 3: 75%) 
		} bits;

		uint8_t reg;
	} sound_length_and_duty_cycle;     //////////////// NR21 sound length and wave pattern/duty cycle register  - 0xFF16

	union
	{
		struct
		{
			uint8_t envelope_period    : 3;  // number of envelope sweeps (0 -7) - if 0 stop envelope
			uint8_t envelope_direction : 1;  // envelope direction (0: decrease, 1: increase)
			uint8_t initial_volume     : 4;  // initial volume of envelope (0 - 15) - if 0 no sound
		} bits;

		uint8_t reg;
	} volume_envelope;                 ////////////////// NR22 volume envelope register - 0xFF17

	union
	{
		struct
		{
			uint8_t frequency_low : 8;           // lower 8 bits of timer frequency
		} bits;

		uint8_t reg;
	} frequency_low;                   //////////////// NR23 frequency low register - 0xFF18

	union
	{
		struct
		{
			uint8_t frequency_high         : 3;  // higher 8 bits of timer frequency
			uint8_t                        : 3;
			uint8_t counter_enable         : 1;  // enable length counter (0: length counter disabled - consecutive, 1: length counter enabled - sound is cut off when counter reaches 0)
			uint8_t initial                : 1;  // initial (1: restart sound)
		} bits;

		uint8_t reg;
	} frequency_high;                  //////////////// NR24 frequency high register - 0xFF19   

	uint16_t frequency_timer;                    // 11-bit timer counter, clocked by system clock (@4.194304 MHz) divided by 32 (5-bit divider) 
	uint8_t waveform_generator;

	uint8_t length_counter;                      // 6-bit length counter, clocked by frame sequencer every 1/256th of a second

	uint8_t volume;                              // channel volume (0 - 15)
	uint8_t volume_sweep_counter;                // 3-bit volume sweep counter

	uint8_t digital_output;                      // channel's 4-bit DAC - 16 steps of output voltage (0 -- 15)
	float DAC;                                   // DAC analog output (-1.0V -- 1.0V)
	int DAC_enabled;
};

/******** channel 3 - custom wave ********/
struct Channel3
{
	union
	{
		struct
		{
			uint8_t : 7;
			uint8_t sound_on_off : 1;            // enable or disable channel (0: playback off, 1: playback on) 
		} bits;

		uint8_t reg;
	} sound_on_off;                    ////////////////// NR30 sound on or off register - 0xFF1A             

	union
	{
		struct
		{
			uint8_t sound_length : 8;            // sound length - binary value loaded in length counter is inverted + 1 (255 + 1 - value)
		} bits;

		uint8_t reg;
	} sound_length;                    ////////////////// NR31 sound length register - 0xFF1B

	union
	{
		struct
		{
			uint8_t                     : 5;
			uint8_t output_level_select : 2;     // select output level (0: mute, 1: 100%, 2: 50%, 3: 25%)
			uint8_t                     : 1;
		} bits;

		uint8_t reg;
	} output_level_select;             ////////////////// NR32 output level select register - 0xFF1C

	union
	{
		struct
		{
			uint8_t frequency_low : 8;           // lower 8 bits of timer frequency
		} bits;

		uint8_t reg;
	} frequency_low;                   ////////////////// NR33 frequency low register - 0xFF1D       

	union
	{
		struct
		{
			uint8_t frequency_high         : 3;  // higher 8 bits of timer frequency
			uint8_t                        : 3;
			uint8_t counter_enable         : 1;  // enable length counter (0: length counter disabled - consecutive, 1: length counter enabled - sound is cut off when counter reaches 0)
			uint8_t initial                : 1;  // initial (1: restart sound)
		} bits;

		uint8_t reg;
	} frequency_high;                  ////////////////// NR34 frequency high register  - 0xFF1E

	uint16_t frequency_timer;                    // 11-bit timer counter, clocked by system clock (@4.194304 MHz) divided by 32 (5-bit divider) 
	uint8_t length_counter;                      // 8-bit length counter, clocked by frame sequencer every 1/256th of a second

	uint8_t wave_RAM[16];                        // 32 4-bits entries 0xFF30 - 0xFF3F 

	uint8_t digital_output;                      // channel's 4-bit DAC - 16 steps of output voltage (0 -- 15)
	float DAC;                                   // DAC analog output (-1.0V -- 1.0V)
	int DAC_enabled;
};

/******** channel 4 - noise ********/
struct Channel4
{
	union
	{
		struct
		{
			uint8_t sound_length : 6;
			uint8_t              : 2;
		} bits;

		uint8_t reg;
	} sound_length;                    ////////////////// NR41 sound length register - 0xFF20

	union
	{
		struct
		{
			uint8_t envelope_period    : 3;  // number of envelope sweeps (0 -7) - if 0 stop envelope
			uint8_t envelope_direction : 1;  // envelope direction (0: decrease, 1: increase)
			uint8_t initial_volume     : 4;  // initial volume of envelope (0 - 15) - if 0 no sound
		} bits;

		uint8_t reg;
	} volume_envelope;                 ////////////////// NR42 volume envelope register - 0xFF21

	union
	{
		struct
		{
			uint8_t frequency_divide_ratio : 3;
			uint8_t counter_step_width     : 1;
			uint8_t shift_clock_frequency  : 4;
		} bits;

		uint8_t reg;
	} polynomial_counter;              ////////////////// NR43 polynomial counter register - 0xFF22

	union
	{
		struct
		{
			uint8_t                : 6;
			uint8_t counter_enable : 1;
			uint8_t initial        : 1;
		} bits;

		uint8_t reg;
	} counter_consecutive_initial;     ////////////////// NR44 counter,consecutive,initial register - 0xFF23

	uint16_t frequency_timer;                    // 16-bit timer counter
	uint8_t length_counter;                      // 6-bit length counter, clocked by frame sequencer every 1/256th of a second

	uint8_t digital_output;                      // channel's 4-bit DAC - 16 steps of output voltage (0 -- 15)
	float DAC;                                   // DAC analog output (-1.0V -- 1.0V)
	int DAC_enabled;
};

/******** sound controller ********/
typedef struct APU APU;

struct APU
{
	struct Channel1 channel1;
	struct Channel2 channel2;
	struct Channel3 channel3;
	struct Channel4 channel4;

	// sound control registers
	union
	{
		struct
		{
			uint8_t S01_output_level : 3;        // SO1 (right output terminal) volume (0 - 7) 
			uint8_t Vin_to_SO1       : 1;        // Vin to SO1 output (0: disable, 1: enable)
			uint8_t S02_output_level : 3;        // SO2 (left output terminal) volume (0 - 7)  
			uint8_t Vin_to_SO2       : 1;        // Vin to SO2 output (0: disable, 1: enable)
		} bits;

		uint8_t reg;
	} channel_control_on_off_volume;   ////////////////// NR50 channel control register - 0xFF24

	union
	{
		struct
		{
			uint8_t channel1_to_SO1 : 1;         // channel 1 on SO1 - right output (0: disable, 1: enable) 
			uint8_t channel2_to_SO1 : 1;         // channel 2 on SO1 - right output (0: disable, 1: enable)
			uint8_t channel3_to_SO1 : 1;         // channel 3 on SO1 - right output (0: disable, 1: enable)
			uint8_t channel4_to_SO1 : 1;         // channel 4 on SO1 - right output (0: disable, 1: enable)
			uint8_t channel1_to_SO2 : 1;         // channel 1 on SO2 - left output (0: disable, 1: enable)
			uint8_t channel2_to_SO2 : 1;         // channel 2 on SO2 - left output (0: disable, 1: enable)
			uint8_t channel3_to_SO2 : 1;         // channel 3 on SO2 - left output (0: disable, 1: enable)
			uint8_t channel4_to_SO2 : 1;         // channel 4 on SO2 - left output (0: disable, 1: enable)
		} bits;

		uint8_t reg;
	} sound_output_terminal_selection; ////////////////// NR51 channel output selection register - 0xFF25

	union
	{
		struct
		{
			uint8_t channel1_on : 1;     // channel 1 on read-only flag - set by writing 1 to initial bit in NR14, cleared when channel 1 length counter (if enabled) expires
			uint8_t channel2_on : 1;     // channel 2 on read-only flag - set by writing 1 to initial bit in NR24, cleared when channel 2 length counter (if enabled) expires
			uint8_t channel3_on : 1;     // channel 3 on read-only flag - set by writing 1 to initial bit in NR34, cleared when channel 3 length counter (if enabled) expires
			uint8_t channel4_on : 1;     // channel 4 on read-only flag - set by writing 1 to initial bit in NR44, cleared when channel 4 length counter (if enabled) expires
			uint8_t : 3;
			uint8_t sound_controller_on : 1;     // enable/disable sound controller (0: disable, 1: enable) - when sound controller is disabled all registers except NR52 are inaccessible
		} bits;

		uint8_t reg;
	} sound_controller_on_off;         ////////////////// NR52 sound controller on/off register - 0xFF26       

	uint64_t clock_cycles;

	float SO1_output;                            // right output 
	float SO2_output;                            // left output
};

static APU apu;
static SDL_AudioDeviceID audio_device;
static uint8_t audio_buffer[BUFFER_SIZE];
static int audio_buffer_index = 0;

void APU_init(void)
{
	if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0)
		printf("error initializing audio system: %s", SDL_GetError());

	SDL_AudioSpec audio_settings = { 0 };

	audio_settings.format = AUDIO_U8;
	audio_settings.freq = SAMPLING_FREQUENCY;
	audio_settings.channels = 2;  // stereo
 	audio_settings.samples = BUFFER_SIZE / 2;
	audio_settings.callback = NULL;
	audio_settings.userdata = NULL;
	//audio_settings.size;      // calculated
	//audio_settings.silence;   // calculated

	if ((audio_device = SDL_OpenAudioDevice(NULL, 0, &audio_settings, NULL, 0)) == 0)
		printf("error opening audio device: %s", SDL_GetError());

	SDL_PauseAudioDevice(audio_device, 0);
}

void APU_deinit(void)
{

}

void APU_clock(void)
{
	// clock frequency timer
	if (apu.clock_cycles % 4 == 0)
	{
		// channel 1
		apu.channel1.frequency_timer--;

		if (apu.channel1.frequency_timer == 0)
		{
			uint8_t bit0 = apu.channel1.waveform_generator >> 7;
			apu.channel1.waveform_generator <<= 1;
			apu.channel1.waveform_generator |= bit0 & 0x01;
		}
		else if (apu.channel1.frequency_timer == UINT16_MAX)
		{
			uint16_t frequency_timer_reload_value = ~(apu.channel1.frequency_high.bits.frequency_high << 8 | apu.channel1.frequency_low.reg) + 1 & 0x07FF;
			//uint16_t frequency_timer_reload_value = 2047 + 1 - (apu.channel1.frequency_high.bits.frequency_high << 8 | apu.channel1.frequency_low.reg);
			apu.channel1.frequency_timer = frequency_timer_reload_value;
		}

		// channel 2
		apu.channel2.frequency_timer--;

		if (apu.channel2.frequency_timer == 0)
		{
			uint8_t bit0 = apu.channel2.waveform_generator >> 7;
			apu.channel2.waveform_generator <<= 1;
			apu.channel2.waveform_generator |= bit0 & 0x01;
		}
		else if (apu.channel2.frequency_timer == UINT16_MAX)
		{
			uint16_t frequency_timer_reload_value = ~(apu.channel2.frequency_high.bits.frequency_high << 8 | apu.channel2.frequency_low.reg) + 1 & 0x07FF;
			//uint16_t frequency_timer_reload_value = 2047 + 1 - (apu.channel2.frequency_high.bits.frequency_high << 8 | apu.channel2.frequency_low.reg);
			apu.channel2.frequency_timer = frequency_timer_reload_value;
		}
	}

	// clock length counter
	if (apu.clock_cycles % (CLOCK_FREQUENCY / (FRAME_SEQUENCER_FREQUENCY / 2)) == 0)
	{
		// channel 1
		if (apu.channel1.frequency_high.bits.counter_enable && apu.channel1.length_counter > 0)
		{
			apu.channel1.length_counter--;

			if (apu.channel1.length_counter == 0)
				apu.sound_controller_on_off.bits.channel1_on = 0;
		}

		// channel 2
		if (apu.channel2.frequency_high.bits.counter_enable && apu.channel2.length_counter > 0)
		{
			apu.channel2.length_counter--;

			if (apu.channel2.length_counter == 0)
				apu.sound_controller_on_off.bits.channel2_on = 0;
		}
	}

	// clock volume envelope
	if (apu.clock_cycles % (CLOCK_FREQUENCY / (FRAME_SEQUENCER_FREQUENCY / 8)) == 0)
	{
		// channel 1
		if (apu.channel1.volume_envelope.bits.envelope_period > 0)
		{
			apu.channel1.volume_sweep_counter--;

			if (apu.channel1.volume_sweep_counter == 0)
			{
				if (apu.channel1.volume_envelope.bits.envelope_direction)  // increase volume
				{
					if (apu.channel1.volume < 15)
						apu.channel1.volume++;
				}
				else    // decrease volume
					if (apu.channel1.volume > 0) 
						apu.channel1.volume--;
			}
			else if (apu.channel1.volume_sweep_counter == UINT8_MAX)
				apu.channel1.volume_sweep_counter = apu.channel1.volume_envelope.bits.envelope_period;
		}

		// channel 2
		if (apu.channel2.volume_envelope.bits.envelope_period > 0)
		{
			apu.channel2.volume_sweep_counter--;

			if (apu.channel2.volume_sweep_counter == 0)
			{
				if (apu.channel2.volume_envelope.bits.envelope_direction)  // increase volume
				{
					if (apu.channel2.volume < 15)
						apu.channel2.volume++;
				}
				else    // decrease volume
					if (apu.channel2.volume > 0)
						apu.channel2.volume--;
			}
			else if (apu.channel2.volume_sweep_counter == UINT8_MAX)
				apu.channel2.volume_sweep_counter = apu.channel2.volume_envelope.bits.envelope_period;
		}
	}

	// channels' output

	// channel 1
	if (apu.sound_controller_on_off.bits.channel1_on)
		apu.channel1.digital_output = (apu.channel1.waveform_generator & 0x01) * apu.channel1.volume;  // 0 -- 15
	else
		apu.channel1.digital_output = 0;

	if (apu.channel1.DAC_enabled)
		apu.channel1.DAC = apu.channel1.digital_output * (2.0f / 15) - 1.0f;   // -1.0V -- 1.0V
	else
		apu.channel1.DAC = 0.0f;

	// channel 2
	if (apu.sound_controller_on_off.bits.channel2_on)
		apu.channel2.digital_output = (apu.channel2.waveform_generator & 0x01) * apu.channel2.volume;  // 0 -- 15
	else
		apu.channel2.digital_output = 0;

	if (apu.channel2.DAC_enabled)
		apu.channel2.DAC = apu.channel2.digital_output * (2.0f / 15) - 1.0f;   // -1.0V -- 1.0V
	else
		apu.channel2.DAC = 0.0f;

	apu.SO1_output = 0.0f;
	apu.SO2_output = 0.0f;
	
	// right speaker output
	if (apu.sound_output_terminal_selection.bits.channel1_to_SO1)
		apu.SO1_output += apu.channel1.DAC;
	if (apu.sound_output_terminal_selection.bits.channel2_to_SO1)
		apu.SO1_output += apu.channel2.DAC;
	//if (apu.sound_output_terminal_selection.bits.channel3_to_SO1)
	//	apu.SO1_output += apu.channel3.DAC;
	//if (apu.sound_output_terminal_selection.bits.channel4_to_SO1)
	//	apu.SO1_output += apu.channel4.DAC;

	apu.SO1_output *= apu.channel_control_on_off_volume.bits.S01_output_level + 1;

	// left speaker output
	if (apu.sound_output_terminal_selection.bits.channel1_to_SO2)
		apu.SO2_output += apu.channel1.DAC;
	if (apu.sound_output_terminal_selection.bits.channel2_to_SO2)
		apu.SO2_output += apu.channel2.DAC;
	//if (apu.sound_output_terminal_selection.bits.channel3_to_SO2)
	//	apu.SO2_output += apu.channel3.DAC;
	//if (apu.sound_output_terminal_selection.bits.channel4_to_SO2)
	//	apu.SO2_output += apu.channel4.DAC;

	apu.SO2_output *= apu.channel_control_on_off_volume.bits.S02_output_level + 1;

	// sample DACs
	if (apu.clock_cycles % CYCLES_PER_SAMPLE == 0)
	{
		audio_buffer[audio_buffer_index++] = (apu.SO1_output + 32.0) / 64.0 * 255;
		audio_buffer[audio_buffer_index++] = (apu.SO2_output + 32.0) / 64.0 * 255;

		if (audio_buffer_index == BUFFER_SIZE)
		{
			SDL_QueueAudio(audio_device, audio_buffer, BUFFER_SIZE);

			audio_buffer_index = 0;

			while (SDL_GetQueuedAudioSize(audio_device))
				;
		}
	
	}

	apu.clock_cycles++;
}


// channel 1 registers
uint8_t APU_read_NR10(void)
{
	return apu.channel1.sweep_register.reg;
}

uint8_t APU_read_NR11(void)
{
	return apu.channel1.sound_length_and_duty_cycle.reg;
}

uint8_t APU_read_NR12(void)
{
	return apu.channel1.volume_envelope.reg;
}

uint8_t APU_read_NR13(void)
{
	return apu.channel1.frequency_low.reg;
}

uint8_t APU_read_NR14(void)
{
	return apu.channel1.frequency_high.reg;
}

void APU_write_NR10(uint8_t value)
{

}

void APU_write_NR11(uint8_t value)
{
	apu.channel1.sound_length_and_duty_cycle.reg = value;

	apu.channel1.length_counter = ~apu.channel1.sound_length_and_duty_cycle.bits.sound_length + 1 & 0x3F;
	//apu.channel1.length_counter = 64 - apu.channel1.sound_length_and_duty_cycle.bits.sound_length;

	switch (apu.channel1.sound_length_and_duty_cycle.bits.duty_cycle)
	{
		case 0:
			apu.channel1.waveform_generator = 0x01;  // 0000.00001
			break;
		case 1:
			apu.channel1.waveform_generator = 0x81;  // 1000.0001
			break;
		case 2:
			apu.channel1.waveform_generator = 0x87;  // 1000.0111
			break;
		case 3:
			apu.channel1.waveform_generator = 0x7E;  // 0111.1110
			break;
	}
}

void APU_write_NR12(uint8_t value)
{
	apu.channel1.volume_envelope.reg = value;

	apu.channel1.volume = apu.channel1.volume_envelope.bits.initial_volume;
	apu.channel1.volume_sweep_counter = apu.channel1.volume_envelope.bits.envelope_period;

	if (apu.channel1.volume_envelope.bits.initial_volume == 0)
	{
		apu.channel1.DAC_enabled = 0;
		apu.sound_controller_on_off.bits.channel1_on = 0;
	}
	else
		apu.channel1.DAC_enabled = 1;
}

void APU_write_NR13(uint8_t value)
{
	apu.channel1.frequency_low.reg = value;

	uint16_t frequency_timer_reload_value = ~(apu.channel1.frequency_high.bits.frequency_high << 8 | apu.channel1.frequency_low.reg) + 1 & 0x07FF;
	//uint16_t frequency_timer_reload_value = 2047 + 1 - (apu.channel1.frequency_high.bits.frequency_high << 8 | apu.channel1.frequency_low.reg);
	apu.channel1.frequency_timer = frequency_timer_reload_value;
}

void APU_write_NR14(uint8_t value)
{
	apu.channel1.frequency_high.reg = value;

	uint16_t frequency_timer_reload_value = ~(apu.channel1.frequency_high.bits.frequency_high << 8 | apu.channel1.frequency_low.reg) + 1 & 0x07FF;
	//uint16_t frequency_timer_reload_value = 2047 + 1 - (apu.channel1.frequency_high.bits.frequency_high << 8 | apu.channel1.frequency_low.reg);
	apu.channel1.frequency_timer = frequency_timer_reload_value;

	if (apu.channel1.frequency_high.bits.initial == 1)
	{
		if (apu.channel1.length_counter == 0)
			apu.channel1.length_counter = 63;

		uint16_t frequency_timer_reload_value = ~(apu.channel1.frequency_high.bits.frequency_high << 8 | apu.channel1.frequency_low.reg) + 1 & 0x07FF;
		//uint16_t frequency_timer_reload_value = 2047 + 1 - (apu.channel1.frequency_high.bits.frequency_high << 8 | apu.channel1.frequency_low.reg);
		apu.channel1.frequency_timer = frequency_timer_reload_value;

		apu.channel1.volume = apu.channel1.volume_envelope.bits.initial_volume;
		apu.channel1.volume_sweep_counter = apu.channel1.volume_envelope.bits.envelope_period;

		apu.sound_controller_on_off.bits.channel1_on = 1;

		if (!apu.channel1.DAC_enabled)  
			apu.sound_controller_on_off.bits.channel1_on = 0;
	}
}

// channel 2 registers
uint8_t APU_read_NR21(void)
{
	return apu.channel2.sound_length_and_duty_cycle.reg;
}

uint8_t APU_read_NR22(void)
{
	return apu.channel2.volume_envelope.reg;
}

uint8_t APU_read_NR23(void)
{
	return apu.channel2.frequency_low.reg;
}

uint8_t APU_read_NR24(void)
{
	return apu.channel2.frequency_high.reg;
}

void APU_write_NR21(uint8_t value)
{
	apu.channel2.sound_length_and_duty_cycle.reg = value;

	apu.channel2.length_counter = ~apu.channel2.sound_length_and_duty_cycle.bits.sound_length + 1 & 0x3F;
	//apu.channel2.length_counter = 64 - apu.channel2.sound_length_and_duty_cycle.bits.sound_length;

	switch (apu.channel2.sound_length_and_duty_cycle.bits.duty_cycle)
	{
		case 0:
			apu.channel2.waveform_generator = 0x01;  // 0000.00001
			break;
		case 1:
			apu.channel2.waveform_generator = 0x81;  // 1000.0001
			break;
		case 2:
			apu.channel2.waveform_generator = 0x87;  // 1000.0111
			break;
		case 3:
			apu.channel2.waveform_generator = 0x7E;  // 0111.1110
			break;
	}
}

void APU_write_NR22(uint8_t value)
{
	apu.channel2.volume_envelope.reg = value;

	apu.channel2.volume = apu.channel2.volume_envelope.bits.initial_volume;
	apu.channel2.volume_sweep_counter = apu.channel2.volume_envelope.bits.envelope_period;

	if (apu.channel2.volume_envelope.bits.initial_volume == 0)
	{
		apu.channel2.DAC_enabled = 0;
		apu.sound_controller_on_off.bits.channel2_on = 0;
	}
	else
		apu.channel2.DAC_enabled = 1;
}

void APU_write_NR23(uint8_t value)
{
	apu.channel2.frequency_low.reg = value;

	uint16_t frequency_timer_reload_value = ~(apu.channel2.frequency_high.bits.frequency_high << 8 | apu.channel2.frequency_low.reg) + 1 & 0x07FF;
	//uint16_t frequency_timer_reload_value = 2047 + 1 - (apu.channel2.frequency_high.bits.frequency_high << 8 | apu.channel2.frequency_low.reg);
	apu.channel2.frequency_timer = frequency_timer_reload_value;
}

void APU_write_NR24(uint8_t value)
{
	apu.channel2.frequency_high.reg = value;

	uint16_t frequency_timer_reload_value = ~(apu.channel2.frequency_high.bits.frequency_high << 8 | apu.channel2.frequency_low.reg) + 1 & 0x07FF;
	//uint16_t frequency_timer_reload_value = 2047 + 1 - (apu.channel2.frequency_high.bits.frequency_high << 8 | apu.channel2.frequency_low.reg);
	apu.channel2.frequency_timer = frequency_timer_reload_value;

	if (apu.channel2.frequency_high.bits.initial == 1)
	{
		if (apu.channel2.length_counter == 0)
			apu.channel2.length_counter = 63;

		uint16_t frequency_timer_reload_value = ~(apu.channel2.frequency_high.bits.frequency_high << 8 | apu.channel2.frequency_low.reg) + 1 & 0x07FF;
		//uint16_t frequency_timer_reload_value = 2047 + 1 - (apu.channel2.frequency_high.bits.frequency_high << 8 | apu.channel2.frequency_low.reg);
		apu.channel2.frequency_timer = frequency_timer_reload_value;

		apu.channel2.volume = apu.channel2.volume_envelope.bits.initial_volume;
		apu.channel2.volume_sweep_counter = apu.channel2.volume_envelope.bits.envelope_period;

		apu.sound_controller_on_off.bits.channel2_on = 1;

		if (!apu.channel2.DAC_enabled)
			apu.sound_controller_on_off.bits.channel2_on = 0;
	}
}

// APU control registers
uint8_t APU_read_NR50(void)
{
	return apu.channel_control_on_off_volume.reg;
}

uint8_t APU_read_NR51(void)
{
	return apu.sound_output_terminal_selection.reg;
}

uint8_t APU_read_NR52(void)
{
	return apu.sound_controller_on_off.reg;
}

void APU_write_NR50(uint8_t value)
{
	apu.channel_control_on_off_volume.reg = value;
}

void APU_write_NR51(uint8_t value)
{
	apu.sound_output_terminal_selection.reg = value;
}

void APU_write_NR52(uint8_t value)
{
	apu.sound_controller_on_off.reg = value & 0x80;

	if (!apu.sound_controller_on_off.bits.sound_controller_on)   // if sound controller is disabled all registers are written to 0x00 and only NR52 is accessible (all writes are ignored)
	{
		apu.channel1.sweep_register.reg = 0x00;
		apu.channel1.sound_length_and_duty_cycle.reg = 0x00;
		apu.channel1.volume_envelope.reg = 0x00;
		apu.channel1.frequency_low.reg = 0x00;
		apu.channel1.frequency_high.reg = 0x00;

		apu.channel2.sound_length_and_duty_cycle.reg = 0x00;
		apu.channel2.volume_envelope.reg = 0x00;
		apu.channel2.frequency_low.reg = 0x00;
		apu.channel2.frequency_high.reg = 0x00;

		apu.channel3.sound_on_off.reg = 0x00;
		apu.channel3.sound_length.reg = 0x00;
		apu.channel3.output_level_select.reg = 0x00;
		apu.channel3.frequency_low.reg = 0x00;
		apu.channel3.frequency_high.reg = 0x00;

		apu.channel4.sound_length.reg = 0x00;
		apu.channel4.volume_envelope.reg = 0x00;
		apu.channel4.polynomial_counter.reg = 0x00;
		apu.channel4.counter_consecutive_initial.reg = 0x00;

		apu.sound_controller_on_off.reg = 0x00;
		apu.sound_output_terminal_selection.reg = 0x00;
		apu.channel_control_on_off_volume.reg = 0x00;
	}
	else     // enable sound controller
	{
		apu.clock_cycles = 0;

		switch (apu.channel1.sound_length_and_duty_cycle.bits.duty_cycle)
		{
			case 0:    
				apu.channel1.waveform_generator = 0x01;  
				break;
			case 1:    
				apu.channel1.waveform_generator = 0x81; 
				break;
			case 2:     
				apu.channel1.waveform_generator = 0x87;  
				break;
			case 3:     
				apu.channel1.waveform_generator = 0x7E;  
				break;
		}

		switch (apu.channel2.sound_length_and_duty_cycle.bits.duty_cycle)
		{
			case 0:     
				apu.channel2.waveform_generator = 0x01; 
				break;
			case 1:     
				apu.channel2.waveform_generator = 0x81;  
				break;
			case 2:    
				apu.channel2.waveform_generator = 0x87; 
				break;
			case 3:    
				apu.channel2.waveform_generator = 0x7E; 
				break;
		}
	}
}
