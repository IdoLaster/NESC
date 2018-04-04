#include <stdint.h>

#ifndef NES_ROM_H
#define NES_ROM_H

typedef struct ROM{
    char *path;
    int prg_size;
    int chr_size;
    uint8_t *prg_rom;
} ROM;

int load_rom(ROM *rom);

uint8_t ROM_READ8(ROM *rom, uint16_t addr);
uint16_t ROM_READ16(ROM *rom, uint16_t addr);

#endif //NES_ROM_H
