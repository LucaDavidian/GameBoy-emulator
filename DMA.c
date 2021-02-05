#include "DMA.h"
#include "bus.h"

static uint16_t DMA_source_address;

static uint8_t transferred;
int DMA_active;

void DMA_start(uint8_t page)
{
	DMA_source_address = (uint16_t)page << 8 + 0x00;
	transferred = 0;
	DMA_active = 1;
}

void DMA_copy(void)
{
	uint8_t data = bus_read(DMA_source_address + transferred);
	bus_write(OAM_BASE + transferred, data);
	
	transferred++;

	if (transferred == 160)
		DMA_active = 0;
}