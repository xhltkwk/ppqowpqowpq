#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "common.h"
#include "io.h"
#include "display.h"

void init(void);
void intro(void);
void outro(void);
void cursor_move(DIRECTION dir, bool is_double_click);
void handle_key_input(KEY key);
void sample_obj_move(void);
POSITION sample_obj_next_position(void);
void create_harvester(POSITION base_position);
//static KEY last_key = k_none; // 마지막 입력된 키
//static clock_t last_key_time = 0; //마지막 방향키 입력 
//static DIRECTION last_dir = d_stay; //마지막 방향기 방향
//int selected_object = -1; //선택된 오브젝트 ID

/* ================= control =================== */
int sys_clock = 0;		// system-wide clock(ms)
CURSOR cursor = { { 1, 1 }, {1, 1} };
int selected_object = -1;

/* ================= game data =================== */
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };

RESOURCE resource = {
	.spice = 0,
	.spice_max = 0,
	.population = 0,
	.population_max = 0
};

OBJECT_SAMPLE obj = {

	.pos = {1, 1},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.repr = 'o',
	.speed = 300,
	.next_move_time = 300

};


/* ================= main() =================== */
int main(void) {
	srand((unsigned int)time(NULL));

	init();
	intro();

	while (1) {
		KEY key = get_key();
		handle_key_input(key);

		// 샘플 오브젝트 동작
		sample_obj_move();

		// 화면 출력
		display(resource, map, cursor);
		Sleep(TICK);
		sys_clock += 10;
	}
}

/* ================= subfunctions =================== */
void intro(void) {
	printf("DUNE 1.5\n");
	Sleep(2000);
	system("cls");
}

void outro(void) {
	printf("exiting...\n");
	exit(0);
}

void init(void) {
	// layer 0(map[0])에 지형 생성
	for (int j = 0; j < MAP_WIDTH; j++) {
		map[0][0][j] = '#';
		map[0][MAP_HEIGHT - 1][j] = '#';
	}

	for (int i = 1; i < MAP_HEIGHT - 1; i++) {
		map[0][i][0] = '#';
		map[0][i][MAP_WIDTH - 1] = '#';
		for (int j = 1; j < MAP_WIDTH - 1; j++) {
			map[0][i][j] = ' ';
		}
	}

	// layer 1(map[1])은 비워 두기(-1로 채움)
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			map[1][i][j] = -1;
		}
	}
	// object sample
	/*map[1][obj.pos.row][obj.pos.column] = 'o';*/
	//초기 상태 설정
	display_initial_state(map);
}

//---------------------커서 이동---------------------------
void cursor_move(DIRECTION dir, bool is_double_click) {
	int move_distance = is_double_click ? 3 : 1; // 기본 1칸 이동, 더블클릭이면 3칸

	for (int i = 0; i < move_distance; i++) {
		POSITION curr = cursor.current;
		POSITION new_pos = pmove(curr, dir);

		// 맵 범위 검증
		if (1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 &&
			1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2) {

			// 1. 이전 위치 복원
			char prev_char = backbuf[curr.row][curr.column];

			// 문자에 따라 원래 색상 복원
			int prev_color = COLOR_DEFAULT;
			switch (prev_char) {
			case 'B': prev_color = COLOR_BLUE; break;
			case 'H': prev_color = COLOR_RED; break;
			case 'W': prev_color = COLOR_YELLOW; break;
			case 'P': prev_color = COLOR_WHITE; break;
			case 'S': prev_color = COLOR_ORANGE; break;
			case 'R': prev_color = COLOR_GRAY; break;
			default: prev_color = COLOR_DEFAULT; break;
			}

			// 이전 위치에 문자와 색상 복원
			printc(padd(map_pos, curr), prev_char, prev_color);

			// 2. 새 위치로 이동
			cursor.previous = cursor.current;
			cursor.current = new_pos;

			// 3. 새 위치에 커서 출력
			char new_char = backbuf[new_pos.row][new_pos.column];
			printc(padd(map_pos, new_pos), new_char, COLOR_CURSOR);
		}
		else {
			break; // 범위를 벗어나면 이동 중단
		}
	}
}

bool is_base(POSITION pos) {
	return map[1][pos.row][pos.column] == 'B';
}

POSITION get_adjacent_position(POSITION base_pos, char object_type) {
	// 기본적으로 -1, -1을 반환해 실패를 나타냄
	POSITION invalid_pos = { -1, -1 };

	// 본진 주변 4방향 확인
	POSITION directions[] = { { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } };
	for (int i = 0; i < 4; i++) {
		POSITION adjacent = padd(base_pos, directions[i]);

		if (1 <= adjacent.row && adjacent.row < MAP_HEIGHT - 1 &&
			1 <= adjacent.column && adjacent.column < MAP_WIDTH - 1 &&
			map[1][adjacent.row][adjacent.column] == ' ') {
			return adjacent;
		}
	}
	return invalid_pos;
}

void handle_key_input(KEY key) {
	static KEY last_key = k_none;       // 마지막 입력된 키
	static clock_t last_key_time = 0;  // 마지막 키 입력 시간

	DIRECTION dir = d_stay;
	bool is_double_click = false;

	if (is_arrow_key(key)) {
		// 더블클릭 감지
		if (key == last_key && (clock() - last_key_time < CLOCKS_PER_SEC / 4)) {
			is_double_click = true;
		}

		last_key = key;
		last_key_time = clock();

		dir = ktod(key);
		cursor_move(dir, is_double_click);  // 커서 이동 호출
	}

	if (key == k_space) {
		// 스페이스바 입력 시에만 상태창 업데이트
		char terrain = backbuf[cursor.current.row][cursor.current.column];
		switch (terrain) {
		case ' ':
			selected_object = 0; // 빈 지형 (사막 지형)
			break;
		case 'B':
			selected_object = 1; // 본진
			break;
		case 'W':
			selected_object = 2; // 샌드웜
			break;
		case 'H':
			selected_object = 3; // 하베스터
			break;
		case 'S':
			selected_object = 4; // 스파이스 매장지
			break;
		case 'P':
			selected_object = 5; // 장판
			break;
		case 'R':
			selected_object = 6; // 바위
			break;
		default:
			selected_object = -1; // 선택되지 않음
			break;
		}
		display_object_info(selected_object); // 상태창 업데이트
	}

	if (key == k_esc) {
		// 선택 취소
		selected_object = -1;
		display_object_info(selected_object);  // 상태창 초기화
	}

	if (key == 'H' || key == 'h') {
		// 하베스터 생성 로직
		char current_obj = map[0][cursor.current.row][cursor.current.column];
		if (current_obj == 'B') {
			// 커서 위치가 본진(`B`)일 경우
			bool harvester_created = false;

			// 본진 주변 위치를 탐색하여 빈 공간에 하베스터 배치
			for (int dr = -1; dr <= 1 && !harvester_created; dr++) {
				for (int dc = -1; dc <= 1 && !harvester_created; dc++) {
					// 현재 위치 제외
					if (dr == 0 && dc == 0) continue;

					int new_row = cursor.current.row + dr;
					int new_col = cursor.current.column + dc;

					// 맵 범위 및 빈 공간 확인
					if (new_row >= 0 && new_row < MAP_HEIGHT &&
						new_col >= 0 && new_col < MAP_WIDTH &&
						map[0][new_row][new_col] == ' ') {

						map[0][new_row][new_col] = 'H'; // 하베스터 배치
						harvester_created = true;
						display_message("A new harvester ready");
					}
				}
			}

			// 주변에 빈 공간이 없는 경우
			if (!harvester_created) {
				display_message("Cannot create harvester here!");
			}
		}
		else {
			// 커서 위치가 본진이 아닌 경우
			display_message("Cannot create harvester here!");
		}
	}
}

// 하베스터 생성 함수
void create_harvester(POSITION base_position) {
	// 본진 근처에 하베스터 배치
	POSITION new_harvester_pos = { base_position.row + 1, base_position.column }; // 아래쪽에 배치
	if (1 <= new_harvester_pos.row && new_harvester_pos.row <= MAP_HEIGHT - 2 &&
		map[0][new_harvester_pos.row][new_harvester_pos.column] == ' ') {
		map[0][new_harvester_pos.row][new_harvester_pos.column] = 'H'; // 하베스터 배치
		display_message("A new harvester ready"); // 시스템 메시지 출력
	}
	else {
		display_message("Not enough spice"); // 배치 실패 메시지
	}
}

bool is_object(POSITION pos) {
	return map[1][pos.row][pos.column] != -1;
}

int get_object_id(POSITION pos) {
	return map[1][pos.row][pos.column];
}


/* ================= sample object movement =================== */
POSITION sample_obj_next_position(void) {
	POSITION diff = psub(obj.dest, obj.pos);
	DIRECTION dir;

	if (diff.row == 0 && diff.column == 0) {
		obj.dest = (obj.dest.row == 1 && obj.dest.column == 1) ?
			(POSITION) {
			MAP_HEIGHT - 2, MAP_WIDTH - 2
		} :
			(POSITION) {
			1, 1
		};
			return obj.pos;
	}

	dir = (abs(diff.row) >= abs(diff.column)) ?
		((diff.row >= 0) ? d_down : d_up) :
		((diff.column >= 0) ? d_right : d_left);

	POSITION next_pos = pmove(obj.pos, dir);

	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 &&
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 &&
		map[1][next_pos.row][next_pos.column] < 0) {
		return next_pos;
	}
	return obj.pos;
}

void sample_obj_move(void) {
	if (sys_clock <= obj.next_move_time) return;

	map[1][obj.pos.row][obj.pos.column] = -1;
	obj.pos = sample_obj_next_position();
	map[1][obj.pos.row][obj.pos.column] = obj.repr;

	obj.next_move_time = sys_clock + obj.speed;
}

