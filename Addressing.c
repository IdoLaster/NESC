#include "include/Addressing.h"

uint8_t READ8_ZP(RAM *ram, uint8_t addr){
    // This is for the Zero-page Addressing mode, The zero page is the first 256 bytes of the RAM.
    // So when we refer to Zero-page addressing, we get the address of the ram from the rom and the value
    // Sitting there.
    return ram->memory[addr];
}

void WRITE8_ZP(RAM *ram, uint8_t addr, uint8_t value){
    ram->memory[addr] = value;
}
uint8_t READ8_ABS(RAM *ram, uint16_t addr){
    return ram->memory[addr];
}

uint16_t READ16_ABS(RAM *ram, uint16_t addr){
    uint8_t lower = ram->memory[addr];
    uint8_t higher = ram->memory[addr + 1];
    return (higher << 8) | (lower & 0xff);
}

uint16_t READ8_INDIRECT_X(RAM *ram, uint16_t addr, uint8_t x_register){
    uint8_t lower_location = ram->memory[addr] + x_register;
    uint8_t lower = READ8(ram, lower_location);
    uint8_t higher_location = ram->memory[addr] + x_register + 1;
    if(higher_location % 0x100 == 0){
        higher_location = lower_location - 0xFF;
    }
    uint8_t higher = READ8(ram, higher_location);
    uint16_t address = (higher << 8) | (lower & 0xff);
    uint8_t value = READ8(ram, address);
    return value;
}

void WRITE8_INDIRECT_X(RAM *ram, uint16_t addr, uint8_t x_register, uint8_t value){
    uint8_t lower_location = ram->memory[addr] + x_register;
    uint8_t lower = READ8(ram, lower_location);
    uint8_t higher_location = ram->memory[addr] + x_register + 1;
    if(higher_location % 0x100 == 0){
        higher_location = lower_location - 0xFF;
    }
    uint8_t higher = READ8(ram, higher_location);
    uint16_t address = (higher << 8) | (lower & 0xff);
    WRITE8(ram, address, value);
}

uint16_t READ8_INDIRECT_Y(RAM *ram, uint16_t addr, uint8_t y_register){
    uint8_t lower_location = ram->memory[addr];
    uint8_t higher_location = lower_location+1;
    if (higher_location % 0x100 == 0){
        higher_location = lower_location - 0xFF;
    }
    uint8_t lower = READ8(ram, lower_location);
    uint8_t higher = READ8(ram, higher_location);
    uint16_t address = (higher << 8) | (lower & 0xFF);
    if(address % 0xFFFF == 0){
        address = y_register - 1;
    }else{
        address+=y_register;
    }
    uint8_t value = READ8(ram, address);
    return value;
}

void WRITE8_INDIRECT_Y(RAM *ram, uint16_t addr, uint8_t y_register, uint8_t value){
    uint8_t lower_location = ram->memory[addr];
    uint8_t lower = READ8(ram, lower_location);
    uint8_t higher_location = ram->memory[addr] + 1;
    if(higher_location % 0x100 == 0){
        higher_location = lower_location - 0xFF;
    }
    uint8_t higher = READ8(ram, higher_location);
    uint16_t address = (higher << 8) | (lower & 0xff);
    if(address % 0xFFFF == 0){
        address = y_register - 1;
    }else{
        address+=y_register;
    }
    WRITE8(ram, address, value);
}

uint16_t INDIRECT_JMP(RAM *ram, uint16_t addr){
    uint16_t lower_location = READ16(ram, addr);
    uint16_t higher_location = READ16(ram, addr) + 1;
    uint8_t lower = READ8(ram, lower_location);
    uint8_t higher = READ8(ram, higher_location);
    if(higher_location % 0x100 == 0){
        higher_location = lower_location - 0xFF;
    }
    printf("[JMP] LOWER: 0x%x HIGHER: 0x%x\n", lower, higher);
    uint16_t address = (higher << 8) | (lower & 0xff);
    return address;
}