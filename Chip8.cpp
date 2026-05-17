#include <iostream>
#include <vector>
#include <stdlib.h>
#include "Chip8.h"
//初始化内存与寄存器
void chip8::initialize() {
	pc = 0x200;
	opcode = 0;
	I = 0;
	sp = 0;
	//清理显存
	
	//清理栈
	for (int i = 0; i < 16; i++)stack[i] = 0;
	//清理寄存器
	//清理内存

	//加载字体
	for (int i = 0; i < 80; i++)memory[i] = chip8_fontset[i];
	//初始化计时器
}
void chip8::loadGame(const std::vector<uint8_t> rom) {
	int p = 80;
	for (uint8_t x : rom) {
		memory[p++] = x;
	}
}
//每个周期执行一次
void chip8::emulateCycle() {
	//获取opcode
	opcode = memory[pc] << 8 | memory[pc + 1];
	//解码/执行opcode
	handleopcode(opcode);
	//更新计时器
	if (delay_timer > 0) { --delay_timer; }
	if (sound_timer > 0) { /*播放beep*/--sound_timer; }
}