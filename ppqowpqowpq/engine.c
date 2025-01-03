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


/* ================= main() =================== */
int main(void) {
	srand((unsigned int)time(NULL)); // 랜덤 초기화
	init();
	intro();

	while (1) { // 메인 루프 시작
		KEY key = get_key();
		handle_key_input(key); // 키 입력 처리


		// 샌드웜 동작 추가
		move_sandworm();

		// 화면 출력
		display(resource, map, cursor);
		Sleep(TICK);
		sys_clock += 10; // 시스템 시간 증가
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
	srand((unsigned int)time(NULL)); // 랜덤 초기화

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

	// 랜덤 자원 초기화
	resource.spice = rand() % 100 + 50;       // 현재 스파이스 (50~149 랜덤)
	resource.spice_max = rand() % 200 + 100;  // 최대 스파이스 (100~299 랜덤)
	resource.population = rand() % 50 + 20;  // 현재 인구 (20~69 랜덤)
	resource.population_max = rand() % 100 + 50; // 최대 인구 (50~149 랜덤)

	// 초기 상태 설정
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
				display_message("Not enough spice");
			}
		}
		else {
			// 커서 위치가 본진이 아닌 경우
			display_message("Not enough spice");
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


DIRECTION get_direction_to(POSITION from, POSITION to) {
	if (from.row < to.row) return d_down;      // 아래로 이동
	if (from.row > to.row) return d_up;        // 위로 이동
	if (from.column < to.column) return d_right; // 오른쪽으로 이동
	if (from.column > to.column) return d_left;  // 왼쪽으로 이동
	return d_stay;  // 현재 위치와 목표가 동일
}


//샌드웜 움직이는 함수
void move_sandworm(void) {
	static POSITION sandworm_positions[10]; // 최대 10개의 샌드웜 위치를 저장
	static int num_sandworms = 0;           // 샌드웜 개수 추적
	static int sandworm_move_timers[10] = { 0 }; // 각 샌드웜의 이동 타이머

	// 초기 샌드웜 위치를 맵에서 한 번만 저장
	if (num_sandworms == 0) {
		for (int i = 0; i < MAP_HEIGHT; i++) {
			for (int j = 0; j < MAP_WIDTH; j++) {
				if (map[0][i][j] == 'W') { // 샌드웜 위치 찾기
					sandworm_positions[num_sandworms++] = (POSITION){ i, j };
				}
			}
		}
	}

	// 각 샌드웜에 대해 이동 처리
	for (int k = 0; k < num_sandworms; k++) {
		if (sys_clock < sandworm_move_timers[k]) {
			continue; // 이동 주기가 되지 않으면 대기
		}

		sandworm_move_timers[k] = sys_clock + 500; // 500ms 후 다음 이동 가능

		POSITION sandworm_pos = sandworm_positions[k];
		POSITION closest_unit = find_closest_unit(sandworm_pos);

		if (closest_unit.row != -1 && closest_unit.column != -1) {
			// 유닛 방향으로 이동
			DIRECTION move_dir = get_direction_to(sandworm_pos, closest_unit);

			// 다음 위치 계산
			POSITION next_pos = pmove(sandworm_pos, move_dir);

			// 다음 위치에 지형이 있다면 회피
			char next_obj = map[0][next_pos.row][next_pos.column];
			if (next_obj == 'B' || next_obj == 'P' || next_obj == 'S' ||
				next_obj == 'R' || next_obj == '#') {

				// 회피: 가능한 다른 방향으로 이동 시도
				DIRECTION directions[] = { d_up, d_down, d_left, d_right };
				for (int i = 0; i < 4; i++) {
					POSITION alt_pos = pmove(sandworm_pos, directions[i]);
					if (map[0][alt_pos.row][alt_pos.column] == ' ') {
						next_pos = alt_pos;
						break;
					}
				}
			}

			// 스파이스 매장지 생성 확률 (10%)
			if (rand() % 100 < 10) {
				map[0][sandworm_pos.row][sandworm_pos.column] = 'S'; // 현재 위치에 스파이스 매장지 생성
			}
			else {
				map[0][sandworm_pos.row][sandworm_pos.column] = ' '; // 현재 위치 비우기
			}

			next_obj = map[0][next_pos.row][next_pos.column];
			if (next_obj != ' ' && next_obj != 'W' && next_obj != 'B' &&
				next_obj != 'P' && next_obj != 'S' && next_obj != 'R' && next_obj != '#') {
				// 유닛 발견 -> 잡아먹음
				map[0][next_pos.row][next_pos.column] = 'W'; // 유닛 자리로 샌드웜 이동
				sandworm_positions[k] = next_pos; // 새로운 위치 저장
			}
			else if (next_obj == ' ') {
				// 빈칸일 경우 이동
				map[0][next_pos.row][next_pos.column] = 'W'; // 다음 위치로 이동
				sandworm_positions[k] = next_pos; // 새로운 위치 저장
			}
		}
	}
}


POSITION find_closest_unit(POSITION sandworm_pos) {
	POSITION closest_unit = { -1, -1 };  // 초기값: 유닛 없음
	int min_distance = INT_MAX;

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			char obj = map[0][i][j];
			if (obj != 'W' && obj != ' ' && obj != 'B' &&
				obj != 'P' && obj != 'S' && obj != 'R' && obj != '#') {
				// 유닛 발견 (H 포함)
				POSITION unit_pos = { i, j };
				int distance = abs(sandworm_pos.row - i) + abs(sandworm_pos.column - j); // 맨해튼 거리

				if (distance < min_distance) {
					min_distance = distance;
					closest_unit = unit_pos;
				}
			}
		}
	}

	return closest_unit;
}

