#include <stdio.h>
#include <stdlib.h>
#include "Chip8.h"
#include <time.h>


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

	for (int i = 0; i < 16; i++)
		stack[i] = 0;

	for (int i = 0; i < 16; i++)
		key[i] = V[i] = 0;

	for (int i = 0; i < 4096; i++)
		memory[i] = 0;

	for (int i = 0; i < 80; i++)
		memory[i] = chip8_fontset[i];

	delay_timer = 0;
	sound_timer = 0;

	draw_flag = true;

	srand(time(NULL));
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

	case 0x3000: // 0x3XNN Skips the next instruction if VX = NN
		if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
			pc += 4;
		else
			pc += 2;
		break;

	case 0x4000: // 0x4XNN Skips the next instruction if VX != NN
		if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
			pc += 4;
		else
			pc += 2;
		break;

	case 0x5000: // 0x5XY0 Skips the next instruction if VX = VY
		if (V[(opcode & 0x0F00) >> 8] == V[opcode & 0x00F0] >> 4)
			pc += 4;
		else
			pc += 2;
		break;

	case 0x6000: // 0x6XNN Sets VX to NN
		V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
		pc += 2;
		break;

	case 0x7000: // 0x7XNN Adds NN to VX, carry flag is not changed
		V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
		pc += 2;
		break;

	case 0x8000:
		switch (opcode & 0x000F) {

		case 0x0000: // 0x8XY0 Sets VX to the value of VY
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;

		case 0x0001: // 0x8XY1 Sets VX to VX or VY
			V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;

		case 0x0002: // 0x8XY2 Sets VX to VX and VY
			V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;

		case 0x0003: // 0x8XY3 Sets VX to VX xor VY
			V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;

		case 0x0004: // 0x8XY4 Adds Vy to VX. VF is set to 1 when there is a carry and 0 otherwise
			if (V[opcode & 0x00F0 >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
				V[0xF] = 1; // carry
			else
				V[0xF] = 0; // no carry

			V[opcode & 0x0F00 >> 8] += V[opcode & 0x00F0 >> 4];
			pc += 2;
			break;

		case 0x0005: // 0x8XY5 Subtracts Vy from VX. VF is set to 0 when there is a borrow and 1 otherwise
			if (V[opcode & 0x00F0 >> 4] > V[opcode & 0x0F00 >> 8])
				V[0xF] = 0; // borrow
			else
				V[0xF] = 1; // no borrow

			V[opcode & 0x0F00 >> 8] -= V[opcode & 0x00F0 >> 4];
			pc += 2;
			break;

		case 0x0006: // 0x8XY6 store sthe least significant bit of VX in VF then shifts VX right by 1
			V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
			V[opcode & 0x0F00 >> 8] >>= 1;
			pc += 2;
			break;

		case 0x0007: // 0x8XY7 Sets VX to VY - VX. Vf is set ot 0 when there is a borrow and 1 otherwise
			if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4])	// VY-VX
				V[0xF] = 0; // borrow
			else
				V[0xF] = 1; // no borrow
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
			pc += 2;
			break;

		case 0x000E: // 0x8XYE stores the most significant bit of VX in VF then shifts VX left by 1
			V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
			V[opcode & 0x0F00 >> 8] <<= 1;
			pc += 2;
			break;

		default:
			printf("Unknown opcode [0x8000]: 0x%X\n", opcode);

		}
		break;

	case 0x9000: // 0x9XY0 Skips the next instruction if VX != VY
		if (V[opcode & 0x0F00 >> 8] != V[opcode & 0x00F0 >> 4])
			pc += 4;
		else
			pc += 2;
		break;

	case 0xA000: // 0xANNN Sets index to the address NNN
		index = opcode & 0x0FFF;
		pc += 2;
		break;

	case 0xB000: // 0xBNNN Jumps to the address NNN + V0
		pc = (opcode & 0x0FFF) + V[0];
		break;

	case 0xC000: // 0xCXNN Sets VX to the result of a bitwise and on a random number and NN
		V[opcode & 0x0F00 >> 8] = (rand() % 0xFF) & (opcode & 0x00FF);
		pc += 2;
		break;

	case 0xD000: {// 0xDXYN draws a sprite at coordinate VX, VY, with a width of 8 px and a height of N px
				 // VF is set to 1 if any screen pixels are flipped from set to unset and 0 otherwise
		unsigned short x = V[opcode & 0x0F00 >> 8];
		unsigned short y = V[opcode & 0x00F0 >> 4];
		unsigned short h = opcode & 0x00FF;
		unsigned short pixel;
		V[0xF] = 0;

		for (int y_line = 0; y_line < h; y_line++) {
			pixel = memory[index + y_line];
			for (int x_line = 0; x_line < 8; x_line++) {
				if ((pixel & (0x80 >> x_line)) != 0) {
					if (gfx[(x + x_line + ((y + y_line) * 64))] == 1) {
						V[0xF] = 1;
					}
					gfx[x + x_line + ((y + y_line) * 64)] ^= 1;
				}
			}
		}

		draw_flag = true;
		pc += 2;
	}
	break;

	case 0xE000:
		switch (opcode & 0x00FF) {
		case 0x009E: // 0xEX9E Skips the next instruction if the key in VX is pressed
			if (key[V[opcode & 0x0F00 >> 8]] != 0)
				pc += 4;
			else
				pc += 2;
			break;

		case 0x00A1: // 0xEXA1 Skips the next instruction if the key in VX is not pressed
			if (key[V[opcode & 0x0F00 >> 8]] == 0)
				pc += 4;
			else
				pc += 2;
			break;

		default:
			printf("Unknown opcode [0xE000]: 0x%X\n", opcode);

		}
		break;

	case 0xF000:
		switch (opcode & 0x00FF) {

		case 0x0007: // 0xFX07 sets VX to the value of the delay timer
			V[opcode & 0x0F00 >> 8] = delay_timer;
			pc += 2;
			break;

		case 0x000A: { // 0xFX0A Wait for a key press and store it in VX
			bool key_pressed = false;
			for (int i = 0; i < 16; i++) {
				if (key[i] != 0) {
					V[(opcode & 0x0F00) >> 8] = i;
					key_pressed = true;
				}
			}

			if (!key_pressed)
				return; // if no key press occurs, skip the cycle and try again

			pc += 2;
			break;

		}
		case 0x0015: // 0xFX15 set the delay timer to VX
			delay_timer = V[opcode & 0x0F00 >> 8];
			pc += 2;
			break;

		case 0x0018: // 0xFX18 set the sound timer to VX
			sound_timer = V[opcode & 0x0F00 >> 8];
			pc += 2;
			break;

		case 0x001E: // 0xFX1E Adds VX to the index
			if (index + V[(opcode & 0x0F00) >> 8] > 0xFFF)	// VF is set to 1 when range overflow (I+VX>0xFFF), and 0 when there isn't.
				V[0xF] = 1;
			else
				V[0xF] = 0;
			index += V[(opcode & 0x0F00) >> 8];
			pc += 2;
			break;

		case 0x0029: // 0xFX29 Sets index to the location of the sprite for the character in VX
			index = V[(opcode & 0x0F00) >> 8] * 0x5;
			pc += 2;
			break;

		case 0x0033: // 0xFX33 Stores the binary decimal representation of VX at the addresses I, I+1, and I+2
			memory[index] = V[(opcode & 0x0F00) >> 8] / 100;
			memory[index + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
			memory[index + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
			pc += 2;
			break;

		case 0x0055: // 0xFX55 Stores V0 to VX in memory starting at the index address.
			for (int i = 0; i <= ((opcode & 0x0F00) >> 8); i++)
				memory[index + i] = V[i];

			index += ((opcode & 0x0F00) >> 8) + 1;
			pc += 2;
			break;

		case 0x0065: // 0xFX65 Fills V0 to VX with values starting at the address in the index.
			for (int i = 0; i <= ((opcode & 0x0F00) >> 8); i++)
				V[i] = memory[index + i];

			index += ((opcode & 0x0F00) >> 8) + 1;
			pc += 2;
			break;

		default:
			printf("Unknown opcode [0xF000]: 0x%X\n", opcode);
		}
		break;

	default:
		printf("Unknown opcode [0xF000]: 0x%X\n", opcode);
	}

	// Update timers
	if (delay_timer > 0)
		delay_timer--;

	if (sound_timer > 0) {
		if (sound_timer == 1)
			printf("BEEP!\n");
		sound_timer--;
	}

}

void chip8::debugRender()
{
	// Draw
	for (int y = 0; y < 32; ++y)
	{
		for (int x = 0; x < 64; ++x)
		{
			if (gfx[(y * 64) + x] == 0)
				printf("O");
			else
				printf(" ");
		}
		printf("\n");
	}
	printf("\n");
}