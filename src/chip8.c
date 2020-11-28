#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chip8.h"


int main(int argc, char *argv[]) {
    Chip8 *chip;

    chip = chip8_init();

    if(argc != 2) {
        fprintf(stderr, "Usage: chip8 rom\n");
        exit(1);
    }

    chip8_load_rom(chip, argv[1]);

    for(;;) {
        if(chip->pc > CHIP8_MEM_SIZE) exit(0);

        chip8_execute_op(chip);
        getchar();

        if(chip->drawflag == 0) {
            chip8_print_screen(chip);
            chip->drawflag = 1;
        }
    }
}


Chip8 *chip8_init() {
    Chip8 *s = calloc(sizeof(Chip8), 1);
    s->mem = calloc(sizeof(CHIP8_MEM_SIZE), 1);
    s->screen = &s->mem[0xF00];
    s->sp = 0xFA0;
    s->pc = 0x200;
    s->drawflag = 1;

    memcpy(s->mem, CHIP8_FONTSET, sizeof(CHIP8_FONTSET));

    return s;
}


void chip8_load_rom(Chip8 *chip, char *rom) {
    FILE *fp;
    int fsize;

    fp = fopen(rom, "r");
    if(fp == NULL) {
        perror("fopen");
        exit(1);
    }

    fseek(fp, 0L, SEEK_END);
    fsize = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    fread(&chip->mem[0x200], 8, fsize, fp);
    fclose(fp);
}

void chip8_execute_op(Chip8 *chip) {
    uint16_t opcode;
    uint16_t pc;
    
    uint16_t nnn;
    uint8_t nn;
    uint8_t x, y, n;

    pc = chip->pc;
    opcode = chip->mem[pc] << 8 | chip->mem[pc + 1];
    printf("0x%02x: 0x%04x ", pc, opcode);

    x  = (opcode & 0x0F00) >> 8;
    y  = (opcode & 0x00F0) >> 4;
    n   = opcode & 0x000F;
    nn  = opcode & 0x00FF;
    nnn = opcode & 0x0FFF;

    switch (opcode & 0xF000) {
        case 0x0000: 
            if(opcode == 0x00E0) { chip8_blank_screen(chip); break;}
        case 0x1000: chip8_jump(chip, nnn); return;
        case 0x2000: printf("CALL\n"); break;
        case 0x3000: printf("SE\n"); break;
        case 0x4000: printf("SNE\n"); break;
        case 0x5000: printf("SE\n"); break;
        case 0x6000: chip8_reg_set(chip, x, nn); break;
        case 0x7000: chip8_reg_add(chip, x, nn); break;
        case 0x8000: printf("MATH\n"); break;
        case 0x9000: printf("SNE\n"); break;
        case 0xA000: chip8_index_set(chip, nnn); break;
        case 0xD000: chip8_draw(chip, x, y, n); break;
        case 0xE000: printf("SKP\n"); break;
        case 0xF000: printf("LD\n"); break;
        default: printf("%04x\n", opcode & 0xF000); break;
    }
    chip->pc += 2;
}

void chip8_blank_screen(Chip8 *chip) {
    printf("screen blanked\n");
    memset(chip->screen, 0, CHIP8_VBUF_SIZE);
}

void chip8_jump(Chip8 *chip, uint16_t loc) {
    printf("pc set to %d\n", loc);
    chip->pc = loc;
}

void chip8_reg_set(Chip8 *chip, uint8_t reg, uint8_t value) {
    printf("set register %d to %d\n", reg, value);
    chip->V[reg] = value;
}

void chip8_reg_add(Chip8 *chip, uint8_t reg, uint8_t value) {
    printf("added %d to register %d\n", value, reg);
    chip->V[reg] += value;
}

void chip8_index_set(Chip8 *chip, uint16_t value) {
    printf("set index register to %d\n", value);
    chip->I = value;
}

uint8_t *chip8_pixel_get(Chip8 *chip, uint8_t x, uint8_t y) {
    uint8_t loc = (y * WIDTH) + x;
    return &chip->screen[loc];
}

void chip8_draw(Chip8 *chip, uint8_t x, uint8_t y, uint8_t n) {
    unsigned short pixel;
    
    chip->V[0xF] = 0;
    for(int yline = 0; yline < n; yline++) {
        pixel = chip->mem[chip->I + yline];
        for(int xline = 0; xline < 8; xline++) {
            if((pixel & (0x80 >> xline)) != 0) {
                if(chip->screen[(x + xline + ((y + yline) * 64))] == 1)
                    chip->V[0xF] = 1;                                 
                chip->screen[x + xline + ((y + yline) * 64)] ^= 1;
            }
        }
    }

    chip->drawflag = 0;
}

void chip8_print_screen(Chip8 *chip) {
    int i;


    system("clear");
    for(i = 0; i < CHIP8_VBUF_SIZE; i++) {
        if(chip->screen[i] == 0) printf("█");
        else printf(" ");
        if(i % 63 == 0 && i != 0) printf("\n");
    }
}
