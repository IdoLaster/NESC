#include "include/RAM.h"

uint8_t READ8(struct RAM *ram, uint16_t addr){
    return ram->memory[addr];
}

uint16_t READ16(struct RAM *ram, uint16_t addr){
    uint16_t value;
    uint8_t low = ram->memory[addr];
    addr++;
    uint8_t high = ram->memory[addr];
    value = (high << 8) | low;
    return value;
}

void WRITE8(struct RAM *ram, uint16_t addr, uint8_t value){
    ram->memory[addr] = value;
}

void WRITE16(struct RAM *ram, uint16_t addr, uint16_t value){
 // First we will need to sawp endinness, it's done simply by:
    uint16_t swapped = (value>>8) | (value<<8);
    ram->memory[addr] = swapped;
}