#include <stdlib.h>
#include <stdio.h>

#include "include/ROM.h"
#include "include/CPU.h"
#include "include/RAM.h"

int main(int argc, char *argv[]){
    CPU *cpu = malloc(sizeof *cpu);
    ROM *rom = malloc(sizeof *rom);
    RAM *ram = malloc(sizeof *ram);

    rom->path = "/share/NES/NESC/ROMS/nestest.nes";
    if(!load_rom(rom)){
        printf("ROM Loaded incorrectly.\n");
        return 0;
    }

    printf("Detected iNES formatted ROM!: %s\n", rom->path);

    cpu->rom = rom;
    cpu->ram = ram;
    cpu->registers.pc = 0xc000;
    power_up(cpu);
    printf("PC:0x%x, A:0x%x, Y:0x%x, X:0x%x, Status:0x%x, SP: 0x%x, Opcode: ", cpu->registers.pc, cpu->registers.a, cpu->registers.y,
           cpu->registers.x, cpu->registers.status, cpu->registers.sp);
    int run = 1;
    while(run){
        run = cpu_step(cpu);
        printf("PC:0x%x, A:0x%x, Y:0x%x, X:0x%x, Status:0x%x, SP: 0x%x, Opcode: ", cpu->registers.pc, cpu->registers.a, cpu->registers.y,
        cpu->registers.x, cpu->registers.status, cpu->registers.sp);
    }
    return 1;
}