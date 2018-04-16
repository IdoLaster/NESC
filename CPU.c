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
    //TODO: Change back the status flag after done with nestest.
    cpu->registers.status = 0x24;
    cpu->registers.a = 0;
    cpu->registers.x = 0;
    cpu->registers.y = 0;
    cpu->registers.sp = 0xFD;
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
    uint16_t operand;
    uint16_t value = 0;
    printf("0x%x\n",op_code);
    size_t increamentPC = 0;
    switch(op_code){
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
            //TODO: Check if turnign on bits 4 and 5 is needed.
            value = cpu->registers.status;
            value |= (1 << 4);
            value |= (1 << 5);
            PUSH8(cpu, value);
            increamentPC++;
            break;
        case 0x09:;
            // ORA - Ors the opearnd with a.
            operand = READ8(cpu->ram, cpu->registers.pc+1);
            cpu->registers.a |= operand;
            FIXFLAGS(cpu->registers.a, cpu->registers.status);
            increamentPC+=2;
            break;
        case 0x10:;
            // BPL - Branch if positive, branching if the negative flag is clear.
            // Addressing mode: Relative.
            operand = READ8(cpu->ram, cpu->registers.pc+1);
            if(!NEGATIVESET(cpu->registers.status)){
                increamentPC+=operand;
            }
            increamentPC+=2;
            break;
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
        case 0x18:;
            // CLC - Clear carry flag.
            cpu->registers.status = cpu->registers.status & 0b11111110;
            increamentPC++;
            break;
        case 0x20:;
            // JSR stands for Jump to Subroutine;
            // Pushs the pc+3 to the stack and then jumping to the given operand.
            PUSH16(cpu, cpu->registers.pc+2);
            //printf("JSR PUSHING: 0x%x\n",cpu->registers.pc+3);
            operand = READ16_ABS(cpu->ram, (cpu->registers.pc + 1));
            cpu->registers.pc = operand;
            break;
        case 0x24:;
            // BIT - Bit Test
            // This instructions is used to test if one or more bits are set in a target memory location.
            // The mask pattern in A is ANDed with the value in memory to set or clear the zero flag,
            // but the result is not kept. Bits 7 and 6 of the value from memory are copied into the N and V flags.
            operand = READ8_ZP(cpu->ram,READ8(cpu->ram, cpu->registers.pc+1));
            value = cpu->registers.a & operand;
            FIXFLAGS(value, cpu->registers.status);
            if(CHECK_BIT(operand,6)){
                SETOVERFLOW(cpu->registers.status);
            }else {
                CLEAROVERFLOW(cpu->registers.status);
            }if(CHECK_BIT(operand,7)){
                SETNEGATIVE(cpu->registers.status);
            }else{
                 CLEARNEGATIVE(cpu->registers.status);
            }
            increamentPC+=2;
            break;
        case 0x28:;
            // PLP - Pull a value from the stack to the status register.
            // The wiki says we should ignore bit 4 and 5
            uint8_t status_register = POP8(cpu);
            for (int i = 0; i < 7; ++i) {
                if(i == 4 || i ==5){
                    continue;
                }
                if(CHECK_BIT(status_register, i)){
                    SET_BIT(cpu->registers.status, i);
                }else{
                    CLEAR_BIT(cpu->registers.status, i);
                }
            }
            increamentPC++;
            break;
        case 0x29:;
            // AND - Preforms a bitwise AND with a and the operand
            // Addressing mode: Immediate.
            operand = READ8(cpu->ram,cpu->registers.pc+1);
            cpu->registers.a &= operand;
            FIXFLAGS(cpu->registers.a, cpu->registers.status);
            increamentPC+=2;
            break;
        case 0x30:;
            // BMI - Branch if minues - branchs if the negative flag is set.
            // Addressing mode: Relative
            operand = READ8(cpu->ram, cpu->registers.pc+1);
            if(NEGATIVESET(cpu->registers.status)){
                increamentPC+=operand;
            }
            increamentPC+=2;
            break;
        case 0x38:;
            //SEC - SEt carry, setting the carry flag enable.
            cpu->registers.status = cpu->registers.status | 0b1;
            increamentPC++;
            break;
        case 0x40:;
            // RTI - Return from Interrupt.
            // Pulling the status flag from the stack as well as the pc.
            status_register = POP8(cpu);
            for (int i = 0; i < 7; ++i) {
                if(i == 4 || i ==5){
                    continue;
                }
                if(CHECK_BIT(status_register, i)){
                    SET_BIT(cpu->registers.status, i);
                }else{
                    CLEAR_BIT(cpu->registers.status, i);
                }
            }
            uint16_t return_to = POP16(cpu);
            cpu->registers.pc = return_to;
            break;
        case 0x48:;
            // PHA - Push the A register
            PUSH8(cpu, cpu->registers.a);
            increamentPC++;
            break;
        case 0x49:;
            // EOR - Exclusive or with A and the operand.
            // Addressing mode: Immediate
            operand = READ8(cpu->ram, cpu->registers.pc+1);
            cpu->registers.a ^= operand;
            FIXFLAGS(cpu->registers.a, cpu->registers.status);
            increamentPC+=2;
            break;
        case 0x4C:;
            // JMP - jumps to given address.
            // Addressing mode: Abs.
            operand = READ16_ABS(cpu->ram, (cpu->registers.pc + 1));
            printf("JUMPING TO: 0x%x\n", operand);
            cpu->registers.pc = operand;
            break;
        case 0x50:;
            // BVC - Branch if overflow is clear.
            // Addressing mode: Relative.
            operand = READ8(cpu->ram, cpu->registers.pc+1);
            if(!OVERFLOWSET(cpu->registers.status)){
                increamentPC+=operand;
            }
            increamentPC+=2;
            break;
        case 0x60:;
            // RTS - Return from subroutine
            return_to = POP16(cpu);
            printf("RETURNING TO: 0x%x\n", return_to);
            cpu->registers.pc = return_to+1;
            break;
        case 0x68:;
            // PLA - Pull an 8bit value from the stack to the A register.
            value = POP8(cpu);
            cpu->registers.a = value;
            FIXFLAGS(cpu->registers.a, cpu->registers.status);
            increamentPC++;
            break;
        case 0x69:;
            // ADC - Add with carry
            // Addressing mode: Immediate.
            operand = READ8(cpu->ram, cpu->registers.pc+1);
            uint16_t sum = cpu->registers.a + operand + CARRYSET(cpu->registers.status);
            uint8_t overflow = (cpu->registers.a ^ sum) & (operand ^ sum) & 0x80;
            if(overflow){
                SETOVERFLOW(cpu->registers.status);
            }else{
                CLEAROVERFLOW(cpu->registers.status);
            }
            if(sum > 255){
                SETCARRY(cpu->registers.status);
            }else{
                CLEARCARRY(cpu->registers.status);
            }
            cpu->registers.a = sum & 0xFF;
            FIXFLAGS(cpu->registers.a, cpu->registers.status);
            increamentPC+=2;
            break;
        case 0x70:;
            // BVS - Branch if overflow is set
            // Addressing mode: Relative.
            operand = READ8(cpu->ram, cpu->registers.pc+1);
            if(OVERFLOWSET(cpu->registers.status)){
                increamentPC+=operand;
            }
            increamentPC+=2;
            break;
        case 0x78:;
            // SEI - Set intrerrupt Disable.
            SETINTERRUPTDISABLE(cpu->registers.status);
            increamentPC+=1;
            break;
        case 0x85:;
            // STA - Stores the a register at a given address.
            // Addressing mode: Zero-page
            operand = READ8(cpu->ram, cpu->registers.pc+1);
            WRITE8_ZP(cpu->ram, operand, cpu->registers.a);
            increamentPC+=2;
            break;
        case 0x86:;
            // STX - Stores the X register to given address.
            // Addressing mode: Zero Page.
            operand = READ8_ZP(cpu->ram, cpu->registers.pc + 1);
            WRITE8(cpu->ram, operand,cpu->registers.x);
            increamentPC+=2;
            break;
        case 0x88:;
            // DEY - Decrement Y Register by 1.
            cpu->registers.y--;
            FIXFLAGS(cpu->registers.y, cpu->registers.status);
            increamentPC++;
            break;
        case 0x8A:;
            // TXA - Transfer X register to A.
            cpu->registers.a = cpu->registers.x;
            FIXFLAGS(cpu->registers.a, cpu->registers.status);
            increamentPC++;
            break;
        case 0x8E:;
            // STX - Stores the X register to given address.
            // Addressing mode: Absolute.
            operand = READ16(cpu->ram, cpu->registers.pc + 1);
            WRITE8(cpu->ram, operand, cpu->registers.x);
            increamentPC+=3;
            break;
        case 0x90:;
            // BCC - Branch if carry
            operand = READ8_ABS(cpu->ram, cpu->registers.pc + 1);
            if(!(cpu->registers.status & 0b1)){
                increamentPC += operand;
                printf("BRANCHING(0x%x)\n", operand);
            }
            increamentPC+= 2;
            break;
        case 0x98:;
            // TYA - Transfer Y register to A register.
            cpu->registers.a = cpu->registers.y;
            FIXFLAGS(cpu->registers.a, cpu->registers.status);
            increamentPC++;
            break;
        case 0x9A:;
            // TXS - Transer X to stack pointer.
            cpu->registers.sp = cpu->registers.x;
            increamentPC++;
            break;
        case 0xA0:;
            // LDY - Loads a value to Y
            // Addressing mode: Immediate.
            operand = READ8(cpu->ram, cpu->registers.pc + 1);
            cpu->registers.y = operand;
            FIXFLAGS(cpu->registers.y, cpu->registers.status);
            increamentPC+=2;
            break;
        case 0xA2:;
            // LDX - Loads a value to x
            // Addressing mode: Immediate.
            operand = READ8(cpu->ram, cpu->registers.pc + 1);
            FIXZERO(operand, cpu->registers.status);
            FIXNEGATIVE(operand, cpu->registers.status);
            cpu->registers.x = operand;
            increamentPC+=2;
            break;
        case 0xA8:;
            // TAY - Transfer A register to Y.
            cpu->registers.y = cpu->registers.a;
            FIXFLAGS(cpu->registers.y, cpu->registers.status);
            increamentPC++;
            break;
        case 0xA9:;
            // LDA - Loads a value to A.
            // Addressing mode: Immediate.
            operand = READ8(cpu->ram, cpu->registers.pc + 1);
            cpu->registers.a = operand;
            FIXFLAGS(cpu->registers.a, cpu->registers.status);
            increamentPC+=2;
            break;
        case 0xAA:;
            // TAX - Transfer A register to X register.
            cpu->registers.x = cpu->registers.a;
            FIXFLAGS(cpu->registers.x, cpu->registers.status);
            increamentPC++;
            break;
        case 0xAD:;
            // LDA - Loads a value to A register.
            // Addressing mode: Absolute.
            operand = READ16(cpu->ram, cpu->registers.pc+1);
            value = READ8(cpu->ram, operand);
            cpu->registers.a = value;
            FIXFLAGS(cpu->registers.a, cpu->registers.status);
            increamentPC+=3;
            break;
        case 0xAE:;
            // LDX - Load a value to x.
            // Addressing mode: Absolute.
            operand = READ16(cpu->ram, cpu->registers.pc+1);
            value = READ8(cpu->ram, operand);
            cpu->registers.x = value;
            FIXFLAGS(cpu->registers.x, cpu->registers.status);
            increamentPC+=3;
            break;
        case 0xB0:;
            // BCS - Branch on Carry Set, it's jumping if the carry is set.
            // Addressing mode: Immediate (I belive).
            operand = READ8_ABS(cpu->ram, cpu->registers.pc + 1);
            if(cpu->registers.status & 0b1){
                increamentPC += operand;
                printf("BRANCHING(0x%x)\n", operand);
            }
            increamentPC+= 2;
            break;
        case 0xB8:;
            // CLV - Clear overflow flag.
            CLEAROVERFLOW(cpu->registers.status);
            increamentPC++;
            break;
        case 0xBA:;
            // TSX - Transer stack pointer  to x.
            cpu->registers.x = cpu->registers.sp;
            FIXFLAGS(cpu->registers.x, cpu->registers.status);
            increamentPC++;
            break;
        case 0xC0:;
            // CPY - Compare with Y, this one is immidate addressing mode.
            operand = READ8(cpu->ram, cpu->registers.pc+1);
            if(cpu->registers.y >= operand){
                SETCARRY(cpu->registers.status);
            }else{
                CLEARCARRY(cpu->registers.status);
            }
            if(cpu->registers.y == operand){
                SETZERO(cpu->registers.status);
            }else{
                CLEARZERO(cpu->registers.status);
            }
            FIXNEGATIVE(cpu->registers.y - operand, cpu->registers.status);
            increamentPC+=2;
            break;
        case 0xC8:;
            // INY - Increase Y register by 1.
            cpu->registers.y++;
            FIXFLAGS(cpu->registers.y, cpu->registers.status);
            increamentPC++;
            break;
        case 0xC9:;
            // CMP - Compares the contents of the a register with the operand and turns on/off flags accordingly
            // Addressing mode Immediate.
            operand = READ8(cpu->ram, cpu->registers.pc+1);
            if(cpu->registers.a >= operand){
                SETCARRY(cpu->registers.status);
            }else{
                CLEARCARRY(cpu->registers.status);
            }
            if(cpu->registers.a == operand){
                SETZERO(cpu->registers.status);
            }else{
                CLEARZERO(cpu->registers.status);
            }
            FIXNEGATIVE(cpu->registers.a - operand, cpu->registers.status);
            increamentPC+=2;
            break;
        case 0xCA:;
            // DEX - Decrement X register by 1.
            cpu->registers.x--;
            FIXFLAGS(cpu->registers.x, cpu->registers.status);
            increamentPC++;
            break;
        case 0xD0:;
            // BNE - Branch if not equal, branchs if the zero flag is clear.
            if(!ZEROSET(cpu->registers.status)){
                operand = READ8_ABS(cpu->ram, cpu->registers.pc + 1);
                increamentPC += operand;
                printf("BRANCHING(0x%x)\n", operand);
            }
            increamentPC += 2;
            break;
        case 0xD8:;
            // CLD - Clear decimal mode.
            CLEARDECIMALMODE(cpu->registers.status);
            increamentPC++;
            break;
        case 0xE0:;
            // CPX - Compares the contents of the x register with the operand and turns on/off flags accordingly
            // Addressing mode Immediate.
            operand = READ8(cpu->ram, cpu->registers.pc+1);
            if(cpu->registers.x >= operand){
                SETCARRY(cpu->registers.status);
            }else{
                CLEARCARRY(cpu->registers.status);
            }
            if(cpu->registers.x == operand){
                SETZERO(cpu->registers.status);
            }else{
                CLEARZERO(cpu->registers.status);
            }
            FIXNEGATIVE(cpu->registers.x - operand, cpu->registers.status);
            increamentPC+=2;
            break;
        case 0xE8:;
            // INX - Increase the X register by 1.
            cpu->registers.x++;
            FIXFLAGS(cpu->registers.x, cpu->registers.status);
            increamentPC++;
            break;
        case 0xE9:;
            // SBC - Substract with carry.
            operand = READ8(cpu->ram, cpu->registers.pc+1) ^ 0xFF;
            sum = cpu->registers.a + operand + CARRYSET(cpu->registers.status);
            overflow = (cpu->registers.a ^ sum) & (operand ^ sum) & 0x80;
            if(overflow){
                SETOVERFLOW(cpu->registers.status);
            }else{
                CLEAROVERFLOW(cpu->registers.status);
            }
            if(sum > 255){
                SETCARRY(cpu->registers.status);
            }else{
                CLEARCARRY(cpu->registers.status);
            }
            cpu->registers.a = sum & 0xFF;
            FIXFLAGS(cpu->registers.a, cpu->registers.status);
            increamentPC+=2;
            break;
            break;
        case 0xEA:;
            // Other NOP instuction, does N-O-T-H-I-N-G.
            increamentPC++;
            break;
        case 0xF0:;
            // BEQ - Branch if equal, or if the zero flag is set.
            // Addressing mode: Immediate I think.
            operand = READ8_ABS(cpu->ram, cpu->registers.pc + 1);
            if(cpu->registers.status & 0b10){
                increamentPC += operand;
                printf("BRANCHING(0x%x)\n", operand);
            }
            increamentPC+= 2;
            break;
        case 0xF8:;
            // SED - Set Decimal flag.
            SETDECIMALMODE(cpu->registers.status);
            increamentPC+=1;
            break;
        default:
            printf("Unimplemented opcode: 0x%x\n", op_code);
            return 0;
    }
    cpu->registers.pc += increamentPC;
    return 1;
}

void PUSH8(CPU *cpu, uint8_t value){
    //printf("---PUSHING VALUE: 0x%x---\n",value);
    cpu->ram->memory[0x100+cpu->registers.sp] = value;
    cpu->registers.sp--;
}
void PUSH16(CPU *cpu, uint16_t value){
    uint8_t lower = value & 0xFF;
    uint8_t higher = value >> 8;
    PUSH8(cpu, higher);
    PUSH8(cpu, lower);
}

uint8_t POP8(CPU *cpu){
    cpu->registers.sp++;
    uint8_t value = cpu->ram->memory[0x100+cpu->registers.sp];
    return value;
}

uint16_t POP16(CPU *cpu){
    uint8_t lower = POP8(cpu);
    uint8_t higher = POP8(cpu);
    return (higher << 8) | (lower & 0xff);
}

uint8_t STACK_TOP(CPU *cpu){
    return cpu->ram->memory[0x100+cpu->registers.sp];
}