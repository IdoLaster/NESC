#include <stdint.h>

#ifndef NES_RAM_H
#define NES_RAM_H

/*  Address Range Size  Usage
 *  $0000-$07FF	  $0800	2KB internal RAM
    $0800-$0FFF	  $0800	Mirrors of $0000-$07FF
    $1000-$17FF	  $0800
    $1800-$1FFF	  $0800
    $2000-$2007	  $0008	NES PPU registers
    $2008-$3FFF	  $1FF8	Mirrors of $2000-2007 (repeats every 8 bytes)
    $4000-$4017	  $0018	NES APU and I/O registers
    $4018-$401F	  $0008	APU and I/O functionality that is normally disabled. See CPU Test Mode.
    $4020-$FFFF	  $BFE0	Cartridge space: PRG ROM, PRG RAM, and mapper registers (See Note)
 */

typedef struct RAM{
    uint8_t memory[0xFFFF];
} RAM;

uint8_t READ8(RAM *ram, uint16_t addr);
uint16_t READ16(RAM *ram, uint16_t addr);

void WRITE8(RAM *ram, uint16_t addr, uint8_t value);
void WRITE16(RAM *ram, uint16_t addr, uint16_t value);
#endif //NES_RAM_H
