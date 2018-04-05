#include <stdint.h>
#include "ROM.h"
#include "RAM.h"

#ifndef NES_ADDRESSING_H
#define NES_ADDRESSING_H

uint8_t READ8_ZP(RAM *ram, uint16_t addr);
uint8_t READ8_ABS(RAM *ram, uint16_t addr);
uint16_t READ16_ABS(RAM *ram, uint16_t addr);
#endif //NES_ADDRESSING_H
