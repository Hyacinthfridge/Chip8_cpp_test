#pragma once
#include <iostream>
#include <vector>
#include <stdlib.h>
static unsigned char chip8_fontset[80] =
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
class chip8 {
private:
	uint16_t opcode;//cpu指令，2bytes
	unsigned char memory[4096];//4k内存
	unsigned char V[16];//16个寄存器 V0,V1....VF VF作为进位标志
	unsigned short I;//Index register 索引寄存器
	unsigned short pc;//program counter 程序计数器
	/*
	Memory map
	0x000 ~ 0x1ff chip8解释器（包含字体）
	0x050 ~ 0x0A0 4x5像素字体集合('0'~'F')
	0x200 ~ 0xfff 游戏rom+工作ram
	*/
	unsigned char gfx[64][32];//图像 总共64*32=2048个像素
	//计时器，应以opcode的执行频率倒计时至0
	unsigned char delay_timer;
	unsigned char sound_timer;
	//栈：保存当前pc位置
	unsigned short stack[16];
	unsigned short sp;
	unsigned char key[16];//16按键键盘
	void clear_screen() {
		for (int x = 0; x < 64; x++) {
			for (int y = 0; y < 32; y++) {
				gfx[x][y] = 0;
			}
		}
	}
	void handleopcode(uint16_t opcode) {
		uint8_t x = (opcode >> 8) & 0x0F;
		uint8_t y = (opcode >> 4) & 0x0F;
		uint8_t kk = opcode & 0xFF;
		uint16_t nnn = opcode & 0xFFF;
		switch (opcode & 0xF000) {
		case 0x0000:
			if (opcode == 0x00E0) clear_screen();
			else if (opcode == 0x00EE) {
				if (sp == 0)break;
				pc = stack[--sp];
			}
			//  0nnn 跳转到位于nnn地址处的例程
			//This instruction is only used on the old computers on which Chip-8 was originally implemented. It is ignored by modern interpreters.
			break;
		case 0x1000:
			pc = nnn;
			break;
		case 0x2000:
			stack[sp++] = pc;
			pc = nnn;
			break;
		case 0x3000:
			if (V[x] == kk) {
				pc += 2;
			}
			break;
		case 0x4000:
			if (V[x] != kk) {
				pc += 2;
			}
			break;
		case 0x5000:
			if (V[x] == V[y]) {
				pc += 2;
			}
			break;
		case 0x6000:
			V[x] = kk;
			break;
		case 0x7000:
			V[x] = V[x] + kk;
			break;
		case 0x8000:
			switch (opcode & 0x000F) {  // 低 4 位
			case 0x0:
				V[x] = V[y];
				break;
			case 0x1:
				V[x] |= V[y];
				break;
			case 0x2:
				V[x] &= V[y];
				break;
			case 0x3:
				V[x] ^= V[y];
				break;
			case 0x4:
				V[x] += V[y];
				V[0xF] = (V[x] < V[y]) ? 1 : 0;//设置进位标志
				break;
			case 0x5:
				V[0xF] = (V[x] > V[y]) ? 1 : 0;
				V[x] = V[x] - V[y];
				break;
			case 0x6:
				V[0xF] = V[x] & 1;
				V[x] >>= 1;
				break;
			case 0x7:
				V[0xF] = (V[y] > V[x]) ? 1 : 0;
				V[x] = V[y] - V[x];
				break;// V[x] = V[y] - V[x]
			case 0xE:
				V[0xF] = (V[x] >> 7) & 1;
				V[x] <<= 1;
				break;   // VF = (V[x]>>7)&1; V[x] <<= 1
			}
			break;
		case 0x9000:
			if (V[x] != V[y]) {
				pc += 2;
			}
			break;
		case 0xA000:
			I = nnn;
			break;
		case 0xB000:
			pc = nnn + V[0];
			break;
		case 0xC000:
			//TODO Cxkk - RND Vx, byte
			//Set Vx = random byte AND kk.
			//	The interpreter generates a random number from 0 to 255, which is then ANDed with the value kk.The results are stored in Vx.See instruction 8xy2 for more information on AND.
			break;
		case 0xD000:
			break;
		case 0xE000:
			if ((opcode & 0xFF) == 0x9E) {

			}
			else if ((opcode & 0xFF) == 0xA1) {

			}
			break;
		case 0xF000:
			switch (opcode & 0x00FF) {
			case 0x07:
				V[x] = delay_timer;
				break;
			case 0x0A:
				//TODO
				break;
			case 0x15:
				delay_timer = V[x];
				break;
			case 0x18:
				sound_timer = V[x];
				break;
			case 0x1E:
				I = I + V[x];
				break;
			case 0x29:

				break;
			case 0x33:

				break;
			case 0x55:
				break;
			case 0x65:
				break;
			}
			break;
		}

	}
public:
	chip8() { initialize(); }
	~chip8() = default;
	void initialize();
	void loadGame(const std::vector<uint8_t> rom);
	void emulateCycle();
	void setKeys();
};