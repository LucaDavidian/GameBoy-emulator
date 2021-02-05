#include "joypad.h"
#include "SDL2/SDL.h"

static uint8_t joypad;

uint8_t joypad_read(void)
{
	const uint8_t *keyboard_state = SDL_GetKeyboardState(NULL);

	int right = keyboard_state[SDL_SCANCODE_RIGHT];
	int left = keyboard_state[SDL_SCANCODE_LEFT];
	int up = keyboard_state[SDL_SCANCODE_UP];
	int down = keyboard_state[SDL_SCANCODE_DOWN];

	int A = keyboard_state[SDL_SCANCODE_A];
	int B = keyboard_state[SDL_SCANCODE_S];
	int select = keyboard_state[SDL_SCANCODE_Q];
	int start = keyboard_state[SDL_SCANCODE_W];

	if (joypad >> 4 == 2)
		return ~(down << 3 | up << 2 | left << 1 | right) & 0x0F;
	else if (joypad >> 4 == 1)
		return ~(start << 3 | select << 2 | B << 1 | A) & 0x0F;
	else
		return 0xFF;
}

void joypad_write(uint8_t data)
{
	joypad = joypad & 0xCF | data;
}