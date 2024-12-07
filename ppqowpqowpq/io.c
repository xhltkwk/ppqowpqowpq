/*
* raw(?) I/O
*/
#include "io.h"
#include "common.h"
#include <windows.h> // Windows-specific functions for console manipulation
#include <conio.h>   // For _kbhit() and _getch()
#include <stdio.h>   // For printf()

void gotoxy(POSITION pos) {
	COORD coord = { pos.column, pos.row }; // ��, �� �ݴ�� ����
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
	if (!_kbhit()) {  // �Էµ� Ű�� �ִ��� Ȯ��
		return k_none;
	}

	int byte = _getch();    // �Էµ� Ű�� ���� ����
	switch (byte) {
	case 'q': return k_quit;  // 'q'�� ������ ����
	case 'H': case 'h':return 'H';
	case 27: return k_esc;    // ESC Ű ó��
	case 32: return k_space;  // �����̽��� ó��
	case 224:
		byte = _getch();  // MSB 224�� �ԷµǸ� 1����Ʈ �� ���޹���
		switch (byte) {
		case 72: return k_up;    // ���� ȭ��ǥ
		case 75: return k_left;  // ���� ȭ��ǥ
		case 77: return k_right; // ������ ȭ��ǥ
		case 80: return k_down;  // �Ʒ��� ȭ��ǥ
		default: return k_undef; // ���ǵ��� ���� Ű
		}
	default: return k_undef; // ���ǵ��� ���� Ű
	}
}

void move_cursor_to(int x, int y) {
	COORD coord = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

