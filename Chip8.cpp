#include <iostream>
#include <vector>
#include <stdlib.h>
#include "Chip8.h"
//初始化内存与寄存器
void chip8::initialize() {
	gfxRefresh = false;
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
	memcpy(memory, chip8_fontset, sizeof(chip8_fontset));
	//初始化计时器
}
void chip8::loadGame(const char* rom) {
	std::ifstream romf(rom, std::ios::binary);   // 一定要加 binary
	if (!romf) {
		std::cerr << "failed to open " << rom << std::endl;
		exit(-1);
	}

	// 获取文件大小（必须在 binary 模式下进行）
	romf.seekg(0, std::ios::end);
	size_t filesiz = romf.tellg();
	romf.seekg(0, std::ios::beg);

	// 一次性读入内存偏移 512 处
	romf.read(reinterpret_cast<char*>(&memory[512]), filesiz);

	if (!romf) {
		std::cerr << "failed to read ROM" << std::endl;
		exit(-1);
	}
	romf.close();
}
//每个周期执行一次
void chip8::emulateCycle() {
	//获取opcode
	opcode = memory[pc] << 8 | memory[pc + 1];
	pc += 2;
	//解码/执行opcode
	gfxRefresh = false;
	handleopcode();
	//更新计时器
	if (delay_timer > 0) { --delay_timer; }
	if (sound_timer > 0) { /*播放beep*/--sound_timer; }
	render();
}
void chip8::printCode() {
	for (int i = 0; i < MAX_MEMORY; i++) {
		printf("%02X ", memory[i]);
	}
}
//void chip8::render() {
//	system("cls");
//	for (int i = 0; i < DISPLAY_HEIGHT; i++) {
//		for (int j = 0; j < DISPLAY_WIDTH; j++) {
//			if (gfx[j][i] == 0) {
//				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x00);
//			}
//			else {
//				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0xF0);
//			}
//
//			printf("%c", gfx[j][i] == 0 ? ' ' : ' ');
//		}
//		printf("\n");
//	}
//}
void chip8::render() {
	if (!gfxRefresh)return;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	for (int y = 0; y < DISPLAY_HEIGHT; y++) {
		// 直接把光标放到每一行的开头
		SetConsoleCursorPosition(hConsole, { 0, (SHORT)y });
		for (int x = 0; x < DISPLAY_WIDTH; x++) {
			SetConsoleTextAttribute(hConsole, gfx[x][y] ? 0xF0 : 0x00);
			printf(" ");   // 用背景色填充一个字符宽度
		}
	}
	// 将光标移到画面外，避免闪烁感
	SetConsoleCursorPosition(hConsole, { 0, (SHORT)DISPLAY_HEIGHT });
}
void setupConsole(int width, int height) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	// 隐藏光标
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(hConsole, &cursorInfo);
	cursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(hConsole, &cursorInfo);

	// 设置窗口大小和缓冲区大小，禁止滚动条
	SMALL_RECT windowRect = { 0, 0, (SHORT)(width - 1), (SHORT)(height - 1) };
	SetConsoleWindowInfo(hConsole, TRUE, &windowRect);
	COORD bufferSize = { (SHORT)width, (SHORT)height };
	SetConsoleScreenBufferSize(hConsole, bufferSize);
}
int main() {
	setupConsole(DISPLAY_WIDTH, DISPLAY_HEIGHT);
	chip8 c8;
	const char* fn = "IBM Logo.ch8";
	c8.loadGame(fn);
	//c8.printCode();
	while (1) {
		c8.emulateCycle();
	}
	return 0;
}