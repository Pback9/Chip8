#include <stdio.h>
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
}

bool chip8::loadGame(const char *filename) {

	init();
	printf("Loading: %s\n", filename);

	FILE *game_file = fopen(filename, "rb");

	fseek(game_file, 0, SEEK_END);
	long file_size = ftell(game_file);
	printf("Filesize: %d\n", file_size);
}