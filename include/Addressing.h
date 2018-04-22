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
uint16_t READ8_INDIRECT_Y(RAM *ram, uint16_t addr, uint8_t y_register);

void WRITE8_INDIRECT_X(RAM *ram, uint16_t addr, uint8_t x_register, uint8_t value);
void WRITE8_INDIRECT_Y(RAM *ram, uint16_t addr, uint8_t y_register, uint8_t value);

uint16_t READ16_ABS(RAM *ram, uint16_t addr);
uint16_t INDIRECT_JMP(RAM *ram, uint16_t addr);

uint8_t READ8_ABS_X(RAM *ram, uint16_t addr, uint8_t x);

uint8_t READ8_ABS_Y(RAM *ram, uint16_t addr, uint8_t y);
void WRITE8_ABS_Y(RAM *ram, uint16_t addr, uint8_t y, uint8_t value);

uint8_t READ8_ZP_X(RAM *ram, uint8_t addr, uint8_t x);
void WRITE8_ZP_X(RAM *ram, uint8_t addr, uint8_t x, uint8_t value);
#endif //NES_ADDRESSING_H
