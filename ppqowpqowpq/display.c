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

extern int selected_object;

void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void display_cursor(CURSOR cursor);
void display_message(const char* message); // �ý��� �޽��� ��� �Լ�
void display_object_info(int object_id); // ��ü ���� ��� �Լ�
void display_commands(); // ��ɾ� ��� �Լ�
void move_cursor_to(int x, int y); // Ŀ�� �̵�



void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor)
{  
	// �� ������ ȭ�鿡 ���
	display_resource(resource);               // �ڿ� ���� ���
	display_map(map);                         // �� ���
	display_cursor(cursor);                   // Ŀ�� ���
	display_message("Game Start!");           // �ý��� �޽��� ���
	display_object_info(selected_object);     // ������Ʈ ID ���� ��� (selected_object�� ���ǵǾ� �־�� �մϴ�)
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
				case 'B': color = COLOR_BLUE; break;
				case 'H': color = COLOR_RED; break;
				case 'W': color = COLOR_YELLOW; break;
				case 'P': color = COLOR_WHITE; break;
				case '5': color = COLOR_ORANGE; break;
				case 'R': color = COLOR_GRAY; break;
				}

				printc(padd(map_pos, pos), backbuf[i][j], color);
			}
			frontbuf[i][j] = backbuf[i][j];
		}
	}
}


// frontbuf[][]���� Ŀ�� ��ġ�� ���ڸ� ���� �ٲ㼭 �״�� �ٽ� ���
void display_cursor(CURSOR cursor) {
	POSITION prev = cursor.previous;
	POSITION curr = cursor.current;

	// ���� ��ġ�� ���ڿ� ���� ���� ����
	char ch_prev = frontbuf[prev.row][prev.column];
	int color_prev = COLOR_DEFAULT; // �⺻ ����
	switch (ch_prev) {
	case 'B': color_prev = COLOR_BLUE; break;
	case 'H': color_prev = COLOR_RED; break;
	case 'W': color_prev = COLOR_YELLOW; break;
	case '5': color_prev = COLOR_ORANGE; break;
	case 'R': color_prev = COLOR_GRAY; break;
	}
	printc(padd(map_pos, prev), ch_prev, color_prev); // ���� �������� ���
}

//�ý��� �޽��� ȭ�鿡 ���
// �ý��� �޽��� ȭ�鿡 ��� �Լ� (display_message�� �ϰ��� �ְ� ���)
void display_message(const char* message) {
	move_cursor_to(system_message_pos.x, system_message_pos.y);  // �޽����� ����� ��ġ�� �̵�
	set_color(COLOR_DEFAULT);
	gotoxy(system_message_pos);
	printf("[System Message]: %s", message);
}

// ������Ʈ ���� ȭ�鿡 ���
void display_object_info(int object_id) {
	// ����â ��ǥ�� Ŀ���� �̵�
	move_cursor_to(status_pos.x, status_pos.y);
	set_color(COLOR_DEFAULT);
	gotoxy(status_pos);
	// ����â ������ ���
	printf("[Object Info]: ������Ʈ ID %d�� ���� ǥ��", object_id); // "[��ü ����]: ���õ� ���� ���� ǥ��"
}

// ��ɾ� â�� ����� ��ġ�� �ʵ��� ����
void display_commands() {
	move_cursor_to(command_pos.x, command_pos.y);
	gotoxy(command_pos);
	printf("[Commands]: Move, Attack, Defend"); // [��ɾ�]: �̵�, ����, ���
}

// ����â�� ����� �Լ�
void clear_status_display() {
	move_cursor_to(status_pos.x, status_pos.y);
	set_color(COLOR_DEFAULT);
	gotoxy(status_pos);
	printf("                     "); // �� �������� �����
}

// �ʱ� ���¸� �����Ͽ� ����ϴ� �Լ�
void display_initial_state(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	// �ʱ� ���� �� ����
	map[0][16][2] = 'B';      // ��Ʈ���̵� ����
	map[0][16][1] = 'B';
	map[0][15][2] = 'B';      // ��Ʈ���̵� ����
	map[0][15][1] = 'B';
	map[0][16][4] = 'P';      // ��Ʈ���̵� ����
	map[0][16][3] = 'P';
	map[0][15][4] = 'P';      // ��Ʈ���̵� ����
	map[0][15][3] = 'P';
	map[0][14][1] = 'H';      // ��Ʈ���̵� �Ϻ�����

	map[0][1][56] = 'P';     // ���ڳ� ����
	map[0][2][56] = 'P';
	map[0][1][55] = 'P';
	map[0][2][55] = 'P';
	map[0][1][57] = 'B';     // ���ڳ� ����
	map[0][1][58] = 'B';
	map[0][2][57] = 'B';     // ���ڳ� ����
	map[0][2][58] = 'B';
	map[0][3][58] = 'H';     // ���ڳ� �Ϻ�����

	map[0][12][1] = '5';      // �����̽� ������ (�»��)
	map[0][5][58] = '5';      // �����̽� ������ (���ϴ�)

	map[0][5][20] = 'R';      // ����
	map[0][5][21] = 'R';
	map[0][6][20] = 'R';      // ����
	map[0][6][21] = 'R';
	map[0][10][15] = 'R';      // ����
	map[0][13][45] = 'R';      // ����
	map[0][12][25] = 'R';      // ����
	map[0][12][26] = 'R';
	map[0][13][25] = 'R';      // ����
	map[0][13][26] = 'R';
	map[0][6][40] = 'R';      // ����

	map[0][4][12] = 'W';      // ����� (���)
	map[0][11][35] = 'W';      // ����� (�ϴ�)
}