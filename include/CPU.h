//
// Created by Ido on 12/16/2017.
//
#ifndef NES_CPU_H
#define NES_CPU_H

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

#define ZEROSET(flag) CHECK_BIT(flag,0)
#define CARRYSET(flag) CHECK_BIT(flag,1)
#define INTERRUPTDISABLE(flag) CHECK_BIT(flag,2)
#define DECIMALMODESET(flag) CHECK_BIT(flag,3)
#define OVERFLOWSET(flag) CHECK_BIT(flag,6)
#define NEGATIVESET(flag) CHECK_BIT(flag,7)

#define FIXZERO(val, flag) {if(val==0){flag=flag|0b10;}else{flag=flag&0b11111101;}}

#include <stdint.h>
#include "ROM.h"
#include "RAM.h"
#include "Addressing.h"

typedef struct CPU {
    struct registers{
        uint16_t pc; // Program Counter.
        uint8_t sp; // Stack counter.
        uint8_t a; // Accumulator.
        uint8_t x; // Index Register X.
        uint8_t y; // Index Register Y.
        uint8_t status; // The status flag.
    } registers;
    ROM *rom;
    RAM *ram;
} CPU;

void power_up(CPU *cpu);
int cpu_step(CPU *cpu);

void PUSH8(CPU *cpu, uint8_t value);
void PUSH16(CPU *cpu, uint16_t value);

uint8_t POP(CPU *cpu);
#endif //NES_CPU_H
