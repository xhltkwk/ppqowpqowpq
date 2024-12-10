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
	display_object_info(selected_object, cursor, map);  // ������Ʈ ID ���� ���
	display_commands();

	// �ʱ� ���¿����� �� �޽��� ���
	static bool is_message_initialized = false;
	if (!is_message_initialized) {
		display_message(NULL); // �ʱ� ���¿����� �� �޽��� ���
		is_message_initialized = true;
	}
}

// �ڿ� ���¸� ȭ�鿡 ����ϴ� �Լ�
void display_resource(RESOURCE resource) {
	move_cursor_to(resource_pos.x, resource_pos.y);
	set_color(COLOR_RESOURCE);
	gotoxy(resource_pos);
	printf("spice = %d/%d, population = %d/%d",
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
				case 'S': color = COLOR_ORANGE; break;
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
	case 'S': color_prev = COLOR_ORANGE; break;
	case 'R': color_prev = COLOR_GRAY; break;
	}
	printc(padd(map_pos, prev), ch_prev, color_prev); // ���� �������� ���
}

//�ý��� �޽��� ȭ�鿡 ���
void display_message(const char* message) {
	move_cursor_to(system_message_pos.x, system_message_pos.y);  // �޽����� ����� ��ġ�� �̵�
	set_color(COLOR_DEFAULT);
	gotoxy(system_message_pos);
	if (message == NULL || message[0] == '\0') {
		// �޽����� ���� ��� �⺻ ���� ���
		printf("[System Message]:                      "); // �������� ���� �޽��� �����
	}
	else {
		// �޽����� �ִ� ��� ���
		printf("[System Message]: %s", message);
	}
}

// ������Ʈ ���� ȭ�鿡 ���
void display_object_info(int object_id) {  //����â
	move_cursor_to(status_pos.x, status_pos.y);
	set_color(COLOR_DEFAULT);
	gotoxy(status_pos);

	// ���õ� ������Ʈ�� ���� ����â ���
	if (object_id == -1) {
		printf("[Object Info]:                           ");
	}
	else if (object_id == 0) {
		printf("[Object Info]: �縷 ����                   ");
	}
	else {
		
		switch (object_id) {
		case 1:
			printf("[Object Info]: ����(Base)                 "); break;
		case 2:
			printf("[Object Info]: �����(Sandworm)           "); break;
		case 3:
			printf("[Object Info]: �Ϻ�����(Harvester)        "); break;
		case 4:
			printf("[Object Info]: �����̽� ������(spice)     "); break;
		case 5:
			printf("[Object Info]: ����(Plate)               "); break;
		case 6:
			printf("[Object Info]: ����(Rock)                "); break;
		default:
			printf("[Object Info]: �� �� ���� ����           "); break;
		}
	}
}


// ��ɾ� â�� ����� ��ġ�� �ʵ��� ����
void display_commands() { //���â
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

	map[0][12][1] = 'S';      // �����̽� ������ (�»��)
	map[0][5][58] = 'S';      // �����̽� ������ (���ϴ�)

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