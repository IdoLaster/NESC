//
// Created by Ido on 12/16/2017.
//
#ifndef NES_CPU_H
#define NES_CPU_H

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

void PUSH(CPU *cpu, uint8_t value);
uint8_t POP(CPU *cpu);
#endif //NES_CPU_H
