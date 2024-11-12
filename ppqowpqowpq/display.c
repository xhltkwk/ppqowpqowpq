/*
*  display.c:
* ȭ�鿡 ���� ������ ���
* ��, Ŀ��, �ý��� �޽���, ����â, �ڿ� ���� ���
* io.c�� �ִ� �Լ����� �����
*/

#include "display.h"
#include "io.h"

// ����� ������� �»��(topleft) ��ǥ
const POSITION resource_pos = { 0, 0 };
const POSITION map_pos = { 1, 0 };
const POSITION status_pos = { 0,70 }; // ����â  ��ġ
const POSITION command_pos = { 20,70 }; // ���â��  ��ġ
const POSITION system_message_pos = { 20,0 }; // �ý��� �޽��� ��ġ


char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };



void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void display_cursor(CURSOR cursor);
void display_system_message(); // �ý��� �޽��� ��� �Լ�
void display_object_info(); // ��ü ���� ��� �Լ�
void display_commands(); // ��ɾ� ��� �Լ�
void move_cursor_to(int x, int y); // Ŀ�� �̵�



void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor)
{  
	
	// �� ������ ȭ�鿡 ���
	display_resource(resource);
	display_map(map);
	display_cursor(cursor);
	display_system_message();
	display_object_info();
	display_commands();

}


// �ڿ� ���¸� ȭ�鿡 ����ϴ� �Լ�
void display_resource(RESOURCE resource) {
	move_cursor_to(resource_pos.x, resource_pos.y);
	set_color(COLOR_RESOURCE);
	gotoxy(resource_pos);
	printf("spice = %d/%d, population=%d/%d",
		resource.spice, resource.spice_max,
		resource.population, resource.population_max);
}


// subfunction of draw_map()
void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]) {
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			for (int k = 0; k < N_LAYER; k++) {
				if (src[k][i][j] >= 0) {
					dest[i][j] = src[k][i][j];
				}
			}
		}
	}
}


void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	project(map, backbuf);

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (frontbuf[i][j] != backbuf[i][j]) {
				POSITION pos = { i, j };
				char ch = backbuf[i][j];

				// ���ڿ� ���� ���� ����
				int color = COLOR_DEFAULT;
				switch (ch) {
				case 'B': color = (pos.column < MAP_WIDTH / 2) ? COLOR_BLUE : COLOR_RED; break;
				case 'H': color = (pos.column < MAP_WIDTH / 2) ? COLOR_BLUE : COLOR_RED; break;
				case 'W': color = COLOR_YELLOW; break;
				case 'P': color = COLOR_WHITE; break;
				case '5': color = COLOR_ORANGE; break;
				case 'R': color = COLOR_GRAY; break;
				}

				printc(padd(map_pos, pos), ch, color);
			}
			frontbuf[i][j] = backbuf[i][j];
		}
	}
}


// frontbuf[][]���� Ŀ�� ��ġ�� ���ڸ� ���� �ٲ㼭 �״�� �ٽ� ���
void display_cursor(CURSOR cursor) {
	POSITION prev = cursor.previous;
	POSITION curr = cursor.current;

	char ch = frontbuf[prev.row][prev.column];
	printc(padd(map_pos, prev), ch, COLOR_DEFAULT);

	ch = frontbuf[curr.row][curr.column];
	printc(padd(map_pos, curr), ch, COLOR_CURSOR);
}

//�ý��� �޽��� ȭ�鿡 ���
void display_system_message() {
	move_cursor_to(system_message_pos.x, system_message_pos.y);
	gotoxy(system_message_pos);
	printf("[System Message]:Game Start!"); // "[�ý��� �޽���] : ���� ����!"
}

void display_object_info() {
	// ����â ��ǥ�� Ŀ���� �̵�
	move_cursor_to(status_pos.x, status_pos.y);
	gotoxy(status_pos);
	// ����â ������ ���
	printf("[Object Info]:Displaying selected unit information"); // "[��ü ����]: ���õ� ���� ���� ǥ��"
}

// ��ɾ� â�� ����� ��ġ�� �ʵ��� ����
void display_commands() {
	move_cursor_to(command_pos.x, command_pos.y);
	gotoxy(command_pos);
	printf("[Commands]:Move, Attack, Defend"); // [��ɾ�]: �̵�, ����, ���
}

// �ʱ� ���¸� �����Ͽ� ����ϴ� �Լ�
void display_initial_state(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	// �ʱ� ���� �� ����
	map[0][16][2] = 'B';      // ��Ʈ���̵� ����
	map[0][16][4] = 'P';      // ��Ʈ���̵� ����
	map[0][15][1] = 'H';      // ��Ʈ���̵� �Ϻ�����

	map[0][1][54] = 'P';     // ���ڳ� ����
	map[0][1][57] = 'B';     // ���ڳ� ����
	map[0][2][58] = 'H';     // ���ڳ� �Ϻ�����

	map[0][12][1] = '5';      // �����̽� ������ (�»��)
	map[0][5][58] = '5';      // �����̽� ������ (���ϴ�)

	map[0][5][20] = 'R';      // ����
	map[0][10][15] = 'R';      // ����
	map[0][13][45] = 'R';      // ����
	map[0][12][25] = 'R';      // ����
	map[0][6][40] = 'R';      // ����

	map[0][4][12] = 'W';      // ����� (���)
	map[0][11][35] = 'W';      // ����� (�ϴ�)
}

