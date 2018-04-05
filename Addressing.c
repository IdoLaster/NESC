#include "include/Addressing.h"

uint8_t READ8_ZP(RAM *ram, uint16_t addr){
    // This is for the Zero-page Addressing mode, The zero page is the first 256 bytes of the RAM.
    // So when we refer to Zero-page addressing, we get the address of the ram from the rom and the value
    // Sitting there.
    return ram->memory[addr];
}

uint8_t READ8_ABS(RAM *ram, uint16_t addr){
    return ram->memory[addr];
}

uint16_t READ16_ABS(RAM *ram, uint16_t addr){
    uint8_t lower = ram->memory[addr];
    uint8_t higher = ram->memory[addr + 1];
    return (higher << 8) | (lower & 0xff);
}