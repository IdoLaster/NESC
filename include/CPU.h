//
// Created by Ido on 12/16/2017.
//
#ifndef NES_CPU_H
#define NES_CPU_H

// Some macros to more easliy work with the status flag register thingy.
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))
#define CLEAR_BIT(var,pos) var &= ~(1 << (pos))
#define SET_BIT(var,pos) var |= (1 << pos)

#define ZEROSET(flag) CHECK_BIT(flag,1)
#define CARRYSET(flag) CHECK_BIT(flag,0)
#define INTERRUPTDISABLE(flag) CHECK_BIT(flag,2)
#define DECIMALMODESET(flag) CHECK_BIT(flag,3)
#define OVERFLOWSET(flag) CHECK_BIT(flag,6)
#define NEGATIVESET(flag) CHECK_BIT(flag,7)

#define FIXZERO(val, flag) {if(val==0){flag=flag|0b10;}else{flag=flag&0b11111101;}}

#define SETOVERFLOW(flag) SET_BIT(flag, 6)
#define SETNEGATIVE(flag) SET_BIT(flag, 7)
#define SETDECIMALMODE(flag) SET_BIT(flag, 3)
#define SETINTERRUPTDISABLE(flag) SET_BIT(flag, 2)
#define SETZERO(flag) SET_BIT(flag, 1)
#define SETCARRY(flag) SET_BIT(flag, 0)

#define CLEAROVERFLOW(flag) CLEAR_BIT(flag, 6)
#define CLEARNEGATIVE(flag) CLEAR_BIT(flag, 7)
#define CLEARDECIMALMODE(flag) CLEAR_BIT(flag, 3)
#define CLEARINTERRUPTDISABLE(flag) CLEAR_BIT(flag, 2)
#define CLEARZERO(flag) CLEAR_BIT(flag, 1)
#define CLEARCARRY(flag) CLEAR_BIT(flag, 0)

#define FIXNEGATIVE(val, flag) {if(CHECK_BIT(val,7)){SETNEGATIVE(flag);}else{CLEARNEGATIVE(flag);}}
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
