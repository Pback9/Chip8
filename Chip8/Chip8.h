#pragma once

class chip8 {
public:
	chip8();
	~chip8();

	bool drawFlag;

	void emulateCycle();
	bool loadGame(const char *filename);

	unsigned char  gfx[64 * 32];	// Total amount of pixels: 2048
	unsigned char  key[16];

private:
	unsigned short pc;				// Program counter
	unsigned short opcode;			// Current opcode
	unsigned short index;			// Index register
	unsigned short sp;				// Stack pointer

	unsigned char  V_register[16];			// V-regs (V0-VF)
	unsigned short stack[16];		// Stack (16 levels)
	unsigned char  memory[4096];	// Memory (size = 4k)		

	unsigned char  delay_timer;		// Delay timer
	unsigned char  sound_timer;		// Sound timer		

	void init();


};