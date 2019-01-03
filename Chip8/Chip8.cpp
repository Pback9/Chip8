#include <stdio.h>
#include <stdlib.h>
#include "Chip8.h"


unsigned char chip8_fontset[80] =
{
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

chip8::chip8() {
	// empty
}

chip8::~chip8() {
	//empty
}

void chip8::init() {
	pc = 0x200;
	opcode = 0;
	index = 0;
	sp = 0;

	for (int i = 0; i < 80; i++)
		memory[i] = chip8_fontset[i];

	delay_timer = 0;
	sound_timer = 0;

	draw_flag = true;
}

bool chip8::loadGame(const char *filename) {

	init();
	printf("Loading: %s\n", filename);

	FILE *game_file;
	errno_t err;

	if ((err = fopen_s(&game_file, filename, "rb")) != 0) {
		fputs("Could not open file", stderr);
		return false;
	}

	fseek(game_file, 0, SEEK_END);
	long file_size = ftell(game_file);
	rewind(game_file);
	printf("Filesize: %d\n", file_size);

	char *buffer = (char*)malloc(sizeof(char) * file_size);

	if (buffer == NULL) {
		fputs("Could not allocate memory", stderr);
		return false;
	}

	size_t result = fread(buffer, 1, file_size, game_file);

	if (result != file_size) {
		fputs("Could not read file", stderr);
		return false;
	}

	if ((4096 - 512) > file_size) {
		for (int i = 0; i < file_size; i++)
			memory[i + 512] = buffer[i];
	}

	else
		printf("ROM is too large");

	
	fclose(game_file);
	free(buffer);
	return true;
}

void chip8::emulateCycle() {

	// fetch opcode
	opcode = memory[pc] << 8 | memory[pc + 1];

	// process opcode
	switch (opcode & 0xF000) {
	case 0x0000:
		switch (opcode & 0x000F) {
		case 0x0000: // clear the screen
			for (int i = 0; i < 2048; i++)
				gfx[i] = 0x0;
			draw_flag = true;
			pc += 2;
			break;

		case 0x000E: // return from subroutine
			sp--;
			pc = stack[sp];
			pc += 2;
			break;

		default:
			printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
		}
		break;

	case 0x1000: // 0x1NNN: jump to NNN
		pc = opcode & 0x0FFF;
		break;

	case 0x2000: // 0x2NNN calls subroutine NNN
		stack[sp] = pc;
		sp++;
		pc = opcode & 0x0FFF;
		break;



	}
			
}