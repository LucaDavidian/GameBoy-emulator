#ifndef __DMA_H__
#define __DMA_H__

#include <stdint.h>

extern int DMA_active;

void DMA_start(uint8_t page);
void DMA_copy(void);

#endif 