#include <stdint.h>

#ifndef NES_ROM_H
#define NES_ROM_H

struct ROM{
    char *path;
    int prg_size;
    int chr_size;
    uint8_t *prg_rom;
} ROM;

int load_rom(struct ROM *rom);

uint8_t ROM_READ8(struct ROM *rom, uint16_t addr);
uint16_t ROM_READ16(struct ROM *rom, uint16_t addr);

#endif //NES_ROM_H
