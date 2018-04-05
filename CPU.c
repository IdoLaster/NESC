#include <stdio.h>
#include <stdint.h>

#include "include/CPU.h"

void power_up(CPU *cpu){
    /*
     * By the offical wiki:
     *  P = $34[1] (IRQ disabled) <= Status flag.
        A, X, Y = 0 <= Genral purpse registers.
        SP = $FD <= Stack pointer.

        TODO: I have actually implemented this, but still need to check if it's fine.
        $4017 = $00 (frame irq enabled)
        $4015 = $00 (all channels disabled)
        $4000-$400F = $00 (not sure about $4010-$4013)
        Full cpu memory map:
        $0000	$800	2KB of work RAM
        $0800	$800	Mirror of $000-$7FF
        $1000	$800	Mirror of $000-$7FF
        $1800	$800	Mirror of $000-$7FF
        $2000	8	PPU Ctrl Registers
        $2008	$1FF8	*Mirror of $2000-$2007
        $4000	$20	Registers (Mostly APU)
        $4020	$1FDF	Cartridge Expansion ROM
        $6000	$2000	SRAM
        $8000	$4000	PRG-ROM
        $C000	$4000	PRG-ROM
     */
    cpu->registers.status = 0x34;
    cpu->registers.a = 0;
    cpu->registers.x = 0;
    cpu->registers.y = 0;
    cpu->registers.sp = 0;
    WRITE16(cpu->ram, 0x4017, 0x00);
    WRITE16(cpu->ram, 0x4015, 0x00);
    // We need to duplicate rom PRG to 0x8000 and 0xc000
    for (int i = 0; i < cpu->rom->prg_size; ++i) {
        cpu->ram->memory[0x8000 + i] = cpu->rom->prg_rom[i];
        cpu->ram->memory[0xC000 + i] = cpu->rom->prg_rom[i];
    }
}

int cpu_step(CPU *cpu){
    uint8_t op_code = READ8(cpu->ram, cpu->registers.pc);
    printf("0x%x\n",op_code);
    size_t increamentPC = 0;
    switch(op_code){
        case 0:;
            //BRK
            // TODO: Understand and implement br(ea)k.
            // According to nesdev.com the brk opcode is 2 byte long, so we need to increase PC by one more.
            PUSH8(cpu, (uint8_t) cpu->registers.pc >> 8);
            PUSH8(cpu, (uint8_t) cpu->registers.pc & 0xFF);
            PUSH8(cpu, cpu->registers.status);
            cpu->registers.status |= 0b100;
            increamentPC++;
            break;
        case 0x03:;
            // SLO instruction, preforms a ASL (Shift left) And then OR with A.
            break;
        case 0x07:;
            // SLO instruction, preforms a ASL (Shift left) And then OR with A.
            // Addressing Mode: Zero-Page.
            uint8_t value = READ8_ZP(cpu->ram, (cpu->registers.pc) + 1);
            if (value & 0b10000000){
                cpu->registers.status & 0b00000001;
            }else{
                cpu->registers.status & 0b00000000;
            }
            value << 1;
            cpu->registers.a | value;
            increamentPC += 2;
            break;
        case 0x08:;
            // PHP:
            // Pushing prossecor status flag.
            PUSH8(cpu, cpu->registers.status);
            increamentPC++;
            break;
        case 0x0C:;
            // NOP - No OPeration, doing nothing.
            increamentPC++;
            break;
        case 0x0D:;
            // ORA abs - Bitwise or with A register.
            uint16_t abs_address = ROM_READ16(cpu->rom, (cpu->registers.pc) + 1);
            uint8_t or_with = READ8_ABS(cpu->ram, abs_address);
            cpu->registers.a | or_with;
            increamentPC+=3;
            break;
        case 0x20:;
            // TODO: Implement JSR: I think i've implemented this, I am not sure if the address I am jumping to
            // TODO: is correct.
            // JSR stands for Jump to Subroutine;
            // JSR Takes 2 byte operand, so we are going to read them too.
            PUSH16(cpu, cpu->registers.pc+4);
            uint16_t operand = READ16_ABS(cpu->ram, (cpu->registers.pc + 1));
            cpu->registers.pc = operand;
            break;
        case 0x4C:;
            // JMP - jumps to given address.
            // Addressing mode: Abs.
            operand = READ16_ABS(cpu->ram, (cpu->registers.pc + 1));
            printf("JUMPING TO: 0x%x\n", operand);
            cpu->registers.pc = operand;
            break;
        case 0x70:;
            // TODO: Implement BVS:
            // Branch on oVerflow  Set
            // Currently I know it's taking a operand which is think it's the label it's going to jump to.
            // So we will read it from rom too.
            operand = ROM_READ8(cpu->rom, (cpu->registers.pc) + 1);
            // Since it's 2 byte instuction, we need to increment PC by one more.
            increamentPC++;
            break;
        case 0x86:;
            // STX - Stores the A register to given address.
            // Addressing mode: Zero Page.
            operand = READ8_ZP(cpu->ram, cpu->registers.pc + 1);
            WRITE8(cpu->ram, operand,cpu->registers.x);
            increamentPC+=2;
            break;
        case 0xA2:;
            // LDX - Loads a value to x
            // Addressing mode: Immediate.
            operand = READ8(cpu->ram, cpu->registers.pc + 1);
            cpu->registers.x = operand;
            increamentPC+=2;
            break;
        case 0xC0:;
            // CPY - Compare with Y, this one is immidate addressing mode.
            operand = ROM_READ8(cpu->rom, (cpu->registers.pc) + 1);
            if(cpu->registers.y < operand){
                cpu->registers.status |= 0b10000000;
            }else if(cpu->registers.y == operand){
                cpu->registers.status |=0b00000011;
            }else if(cpu->registers.y > operand){
                cpu->registers.status |=0b00000001;
            }
            increamentPC += 2;
            break;
        default:
            printf("Unimplemented opcode: 0x%x\n", op_code);
            return 0;
    }
    cpu->registers.pc += increamentPC;
    return 1;
}

void PUSH8(CPU *cpu, uint8_t value){
    cpu->ram->memory[0x100+cpu->registers.sp] = value;
    cpu->registers.sp++;
}

void PUSH16(CPU *cpu, uint16_t value){
    PUSH8(cpu, (uint8_t) value >> 8);
    PUSH8(cpu, (uint8_t) value & 0xFF);
}

uint8_t POP(CPU *cpu){
    uint8_t value = cpu->ram->memory[0x100+cpu->registers.sp];
    cpu->registers.sp--;
    return value;
}
