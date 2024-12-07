/*
* raw(?) I/O
*/
#include "io.h"
#include "common.h"
#include <windows.h> // Windows-specific functions for console manipulation
#include <conio.h>   // For _kbhit() and _getch()
#include <stdio.h>   // For printf()

void gotoxy(POSITION pos) {
	COORD coord = { pos.column, pos.row }; // 행, 열 반대로 전달
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void set_color(int color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void printc(POSITION pos, char ch, int color) {
	if (color >= 0) {
		set_color(color);
	}
	gotoxy(pos);
	printf("%c", ch);
}

KEY get_key(void) {
	if (!_kbhit()) {  // 입력된 키가 있는지 확인
		return k_none;
	}

	int byte = _getch();    // 입력된 키를 전달 받음
	switch (byte) {
	case 'q': return k_quit;  // 'q'를 누르면 종료
	case 'H': case 'h':return 'H';
	case 27: return k_esc;    // ESC 키 처리
	case 32: return k_space;  // 스페이스바 처리
	case 224:
		byte = _getch();  // MSB 224가 입력되면 1바이트 더 전달받음
		switch (byte) {
		case 72: return k_up;    // 위쪽 화살표
		case 75: return k_left;  // 왼쪽 화살표
		case 77: return k_right; // 오른쪽 화살표
		case 80: return k_down;  // 아래쪽 화살표
		default: return k_undef; // 정의되지 않은 키
		}
	default: return k_undef; // 정의되지 않은 키
	}
}

void move_cursor_to(int x, int y) {
	COORD coord = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

