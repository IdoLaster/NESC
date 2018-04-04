#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/ROM.h"

int load_rom(struct ROM *rom){
    FILE *fp;
    char* header = (char*) malloc(16* sizeof(char));

    fp = fopen(rom->path, "rb");

    // Validating the file.
    if(fp == NULL){
        printf("Couldn't not open rom.\n");
        return 0;
    }
    // Reading the header(first 16 bytes).
    fread(header, 1, sizeof(header), fp);

    if (memcmp(header, "NES\x1A", 4)) {
        printf("Not a valid iNES ROM image!\n");
        return 0;
    }

    memcpy(&rom->prg_size, header+4, 1);
    rom->prg_size = rom->prg_size * 0x4000;
    memcpy(&rom->chr_size, header+5, 1);
    rom->chr_size = rom->chr_size * 0x2000;


    // Copying all the prg rom data to a pointer.
    rom->prg_rom = malloc(rom->prg_size);
    fread(rom->prg_rom, 1, rom->prg_size, fp);


    // Closing the file and freeing memory.
    fclose(fp);
    free(header);
    return 1;
}

uint8_t ROM_READ8(struct ROM *rom, uint16_t addr){
    return rom->prg_rom[addr];
}

uint16_t ROM_READ16(struct ROM *rom, uint16_t addr){
    // This is reading 16 bit of the ROM, if we need to change big to little endian
    // Just need to swap high and low at the line before the return.
    uint16_t value;
    uint8_t low = rom->prg_rom[addr];
    addr++;
    uint8_t high = rom->prg_rom[addr];
    value = (high << 8) | low;
    return value;
}