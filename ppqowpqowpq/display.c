/*
*  display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/

#include "display.h"
#include "io.h"

// 출력할 내용들의 좌상단(topleft) 좌표
const POSITION resource_pos = { 0, 0 };
const POSITION map_pos = { 1, 0 };
const POSITION status_pos = { 0,70 }; // 상태창  위치
const POSITION command_pos = { 20,70 }; // 명령창의  위치
const POSITION system_message_pos = { 20,0 }; // 시스템 메시지 위치


char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };

extern int selected_object;

void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void display_cursor(CURSOR cursor);
void display_message(const char* message); // 시스템 메시지 출력 함수
void display_object_info(int object_id); // 객체 정보 출력 함수
void display_commands(); // 명령어 출력 함수
void move_cursor_to(int x, int y); // 커서 이동



void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor)
{  
	// 각 영역을 화면에 출력
	display_resource(resource);               // 자원 정보 출력
	display_map(map);                         // 맵 출력
	display_cursor(cursor);                   // 커서 출력
	display_message("Game Start!");           // 시스템 메시지 출력
	display_object_info(selected_object);     // 오브젝트 ID 정보 출력 (selected_object가 정의되어 있어야 합니다)
	display_commands();
}

// 자원 상태를 화면에 출력하는 함수
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

				// 문자에 따라 색상 설정
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


// frontbuf[][]에서 커서 위치의 문자를 색만 바꿔서 그대로 다시 출력
void display_cursor(CURSOR cursor) {
	POSITION prev = cursor.previous;
	POSITION curr = cursor.current;

	// 이전 위치의 문자와 원래 색상 복원
	char ch_prev = frontbuf[prev.row][prev.column];
	int color_prev = COLOR_DEFAULT; // 기본 색상
	switch (ch_prev) {
	case 'B': color_prev = COLOR_BLUE; break;
	case 'H': color_prev = COLOR_RED; break;
	case 'W': color_prev = COLOR_YELLOW; break;
	case '5': color_prev = COLOR_ORANGE; break;
	case 'R': color_prev = COLOR_GRAY; break;
	}
	printc(padd(map_pos, prev), ch_prev, color_prev); // 원래 색상으로 출력
}

//시스템 메시지 화면에 출력
// 시스템 메시지 화면에 출력 함수 (display_message로 일관성 있게 사용)
void display_message(const char* message) {
	move_cursor_to(system_message_pos.x, system_message_pos.y);  // 메시지를 출력할 위치로 이동
	set_color(COLOR_DEFAULT);
	gotoxy(system_message_pos);
	printf("[System Message]: %s", message);
}

// 오브젝트 정보 화면에 출력
void display_object_info(int object_id) {
	// 상태창 좌표로 커서를 이동
	move_cursor_to(status_pos.x, status_pos.y);
	set_color(COLOR_DEFAULT);
	gotoxy(status_pos);
	// 상태창 내용을 출력
	printf("[Object Info]: 오브젝트 ID %d의 정보 표시", object_id); // "[객체 정보]: 선택된 유닛 정보 표시"
}

// 명령어 창의 출력이 겹치지 않도록 조정
void display_commands() {
	move_cursor_to(command_pos.x, command_pos.y);
	gotoxy(command_pos);
	printf("[Commands]: Move, Attack, Defend"); // [명령어]: 이동, 공격, 방어
}

// 상태창을 지우는 함수
void clear_status_display() {
	move_cursor_to(status_pos.x, status_pos.y);
	set_color(COLOR_DEFAULT);
	gotoxy(status_pos);
	printf("                     "); // 빈 공간으로 덮어씌움
}

// 초기 상태를 설정하여 출력하는 함수
void display_initial_state(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	// 초기 상태 맵 설정
	map[0][16][2] = 'B';      // 아트레이디스 본진
	map[0][16][1] = 'B';
	map[0][15][2] = 'B';      // 아트레이디스 본진
	map[0][15][1] = 'B';
	map[0][16][4] = 'P';      // 아트레이디스 장판
	map[0][16][3] = 'P';
	map[0][15][4] = 'P';      // 아트레이디스 장판
	map[0][15][3] = 'P';
	map[0][14][1] = 'H';      // 아트레이디스 하베스터

	map[0][1][56] = 'P';     // 하코넨 장판
	map[0][2][56] = 'P';
	map[0][1][55] = 'P';
	map[0][2][55] = 'P';
	map[0][1][57] = 'B';     // 하코넨 본진
	map[0][1][58] = 'B';
	map[0][2][57] = 'B';     // 하코넨 본진
	map[0][2][58] = 'B';
	map[0][3][58] = 'H';     // 하코넨 하베스터

	map[0][12][1] = '5';      // 스파이스 매장지 (좌상단)
	map[0][5][58] = '5';      // 스파이스 매장지 (우하단)

	map[0][5][20] = 'R';      // 바위
	map[0][5][21] = 'R';
	map[0][6][20] = 'R';      // 바위
	map[0][6][21] = 'R';
	map[0][10][15] = 'R';      // 바위
	map[0][13][45] = 'R';      // 바위
	map[0][12][25] = 'R';      // 바위
	map[0][12][26] = 'R';
	map[0][13][25] = 'R';      // 바위
	map[0][13][26] = 'R';
	map[0][6][40] = 'R';      // 바위

	map[0][4][12] = 'W';      // 샌드웜 (상단)
	map[0][11][35] = 'W';      // 샌드웜 (하단)
}