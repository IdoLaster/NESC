#include <stdint.h>
#include <stdio.h>

#include "ROM.h"
#include "RAM.h"

#ifndef NES_ADDRESSING_H
#define NES_ADDRESSING_H

uint8_t READ8_ZP(RAM *ram, uint8_t addr);
void WRITE8_ZP(RAM *ram, uint8_t addr, uint8_t value);
uint8_t READ8_ABS(RAM *ram, uint16_t addr);
uint16_t READ8_INDIRECT_X(RAM *ram, uint16_t addr, uint8_t x_register);
void WRITE8_INDIRECT_X(RAM *ram, uint16_t addr, uint8_t x_register, uint8_t value);
uint16_t READ16_ABS(RAM *ram, uint16_t addr);
#endif //NES_ADDRESSING_H
