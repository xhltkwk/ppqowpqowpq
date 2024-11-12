/*
*  display.c:
* ȭ�鿡 ���� ������ ���
* ��, Ŀ��, �ý��� �޽���, ����â, �ڿ� ���� ���
* io.c�� �ִ� �Լ����� �����
*/

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "common.h"

// ǥ���� ���� ����. ���� ���� �״µ�, ���ⲯ �߰��ϰų� �����ϱ�
#define COLOR_DEFAULT	15
#define COLOR_CURSOR	112
#define COLOR_RESOURCE  112
// ���� ���� �߰�
#define COLOR_BLUE     9   // ��Ʈ���̵� (�÷��̾�) - Ǫ����
#define COLOR_RED      12  // ���ڳ� (AI) - ������
#define COLOR_YELLOW   14  // ����� - Ȳ���
#define COLOR_BLACK    0   // ���� - ������
#define COLOR_ORANGE   6   // �����̽� - ��Ȳ��
#define COLOR_GRAY     8   // ��Ÿ ���� - ȸ��
#define COLOR_WHITE    15

// ������ �ڿ�, ��, Ŀ���� ǥ��
// ������ ȭ�鿡 ǥ���� ����� ���⿡ �߰��ϱ�
void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor
);

#endif
#pragma once
