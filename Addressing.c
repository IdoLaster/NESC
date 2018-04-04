#include "include/Addressing.h"

uint8_t READ8_ZP(ROM *rom, RAM *ram, uint16_t rom_addr){
    // This is for the Zero-page Addressing mode, The zero page is the first 256 bytes of the RAM.
    // So when we refer to Zero-page addressing, we get the address of the ram from the rom and the value
    // Sitting there.
    uint8_t zp_address = ROM_READ8(rom, rom_addr);

    return ram->memory[zp_address];
}

uint8_t READ_ABS(RAM *ram, uint16_t addr){
    return ram->memory[addr];
}