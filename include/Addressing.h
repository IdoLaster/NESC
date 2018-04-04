#include <stdint.h>
#include "ROM.h"
#include "RAM.h"

#ifndef NES_ADDRESSING_H
#define NES_ADDRESSING_H

uint8_t READ8_ZP(ROM *rom, RAM *ram, uint16_t rom_addr);
uint8_t READ_ABS(RAM *ram, uint16_t addr);
#endif //NES_ADDRESSING_H
