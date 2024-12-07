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
//static KEY last_key = k_none; // ������ �Էµ� Ű
//static clock_t last_key_time = 0; //������ ����Ű �Է� 
//static DIRECTION last_dir = d_stay; //������ ����� ����
//int selected_object = -1; //���õ� ������Ʈ ID

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

		// ���� ������Ʈ ����
		sample_obj_move();

		// ȭ�� ���
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
	// layer 0(map[0])�� ���� ����
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

	// layer 1(map[1])�� ��� �α�(-1�� ä��)
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			map[1][i][j] = -1;
		}
	}
	// object sample
	/*map[1][obj.pos.row][obj.pos.column] = 'o';*/
	//�ʱ� ���� ����
	display_initial_state(map);
}

//---------------------Ŀ�� �̵�---------------------------
void cursor_move(DIRECTION dir, bool is_double_click) {
	int move_distance = is_double_click ? 3 : 1; // �⺻ 1ĭ �̵�, ����Ŭ���̸� 3ĭ

	for (int i = 0; i < move_distance; i++) {
		POSITION curr = cursor.current;
		POSITION new_pos = pmove(curr, dir);

		// �� ���� ����
		if (1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 &&
			1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2) {

			// 1. ���� ��ġ ����
			char prev_char = backbuf[curr.row][curr.column];

			// ���ڿ� ���� ���� ���� ����
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

			// ���� ��ġ�� ���ڿ� ���� ����
			printc(padd(map_pos, curr), prev_char, prev_color);

			// 2. �� ��ġ�� �̵�
			cursor.previous = cursor.current;
			cursor.current = new_pos;

			// 3. �� ��ġ�� Ŀ�� ���
			char new_char = backbuf[new_pos.row][new_pos.column];
			printc(padd(map_pos, new_pos), new_char, COLOR_CURSOR);
		}
		else {
			break; // ������ ����� �̵� �ߴ�
		}
	}
}

bool is_base(POSITION pos) {
	return map[1][pos.row][pos.column] == 'B';
}

POSITION get_adjacent_position(POSITION base_pos, char object_type) {
	// �⺻������ -1, -1�� ��ȯ�� ���и� ��Ÿ��
	POSITION invalid_pos = { -1, -1 };

	// ���� �ֺ� 4���� Ȯ��
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
	static KEY last_key = k_none;       // ������ �Էµ� Ű
	static clock_t last_key_time = 0;  // ������ Ű �Է� �ð�

	DIRECTION dir = d_stay;
	bool is_double_click = false;

	if (is_arrow_key(key)) {
		// ����Ŭ�� ����
		if (key == last_key && (clock() - last_key_time < CLOCKS_PER_SEC / 4)) {
			is_double_click = true;
		}

		last_key = key;
		last_key_time = clock();

		dir = ktod(key);
		cursor_move(dir, is_double_click);  // Ŀ�� �̵� ȣ��
	}

	if (key == k_space) {
		// �����̽��� �Է� �ÿ��� ����â ������Ʈ
		char terrain = backbuf[cursor.current.row][cursor.current.column];
		switch (terrain) {
		case ' ':
			selected_object = 0; // �� ���� (�縷 ����)
			break;
		case 'B':
			selected_object = 1; // ����
			break;
		case 'W':
			selected_object = 2; // �����
			break;
		case 'H':
			selected_object = 3; // �Ϻ�����
			break;
		case 'S':
			selected_object = 4; // �����̽� ������
			break;
		case 'P':
			selected_object = 5; // ����
			break;
		case 'R':
			selected_object = 6; // ����
			break;
		default:
			selected_object = -1; // ���õ��� ����
			break;
		}
		display_object_info(selected_object); // ����â ������Ʈ
	}

	if (key == k_esc) {
		// ���� ���
		selected_object = -1;
		display_object_info(selected_object);  // ����â �ʱ�ȭ
	}

	if (key == 'H' || key == 'h') {
		// �Ϻ����� ���� ����
		char current_obj = map[0][cursor.current.row][cursor.current.column];
		if (current_obj == 'B') {
			// Ŀ�� ��ġ�� ����(`B`)�� ���
			bool harvester_created = false;

			// ���� �ֺ� ��ġ�� Ž���Ͽ� �� ������ �Ϻ����� ��ġ
			for (int dr = -1; dr <= 1 && !harvester_created; dr++) {
				for (int dc = -1; dc <= 1 && !harvester_created; dc++) {
					// ���� ��ġ ����
					if (dr == 0 && dc == 0) continue;

					int new_row = cursor.current.row + dr;
					int new_col = cursor.current.column + dc;

					// �� ���� �� �� ���� Ȯ��
					if (new_row >= 0 && new_row < MAP_HEIGHT &&
						new_col >= 0 && new_col < MAP_WIDTH &&
						map[0][new_row][new_col] == ' ') {

						map[0][new_row][new_col] = 'H'; // �Ϻ����� ��ġ
						harvester_created = true;
						display_message("A new harvester ready");
					}
				}
			}

			// �ֺ��� �� ������ ���� ���
			if (!harvester_created) {
				display_message("Cannot create harvester here!");
			}
		}
		else {
			// Ŀ�� ��ġ�� ������ �ƴ� ���
			display_message("Cannot create harvester here!");
		}
	}
}

// �Ϻ����� ���� �Լ�
void create_harvester(POSITION base_position) {
	// ���� ��ó�� �Ϻ����� ��ġ
	POSITION new_harvester_pos = { base_position.row + 1, base_position.column }; // �Ʒ��ʿ� ��ġ
	if (1 <= new_harvester_pos.row && new_harvester_pos.row <= MAP_HEIGHT - 2 &&
		map[0][new_harvester_pos.row][new_harvester_pos.column] == ' ') {
		map[0][new_harvester_pos.row][new_harvester_pos.column] = 'H'; // �Ϻ����� ��ġ
		display_message("A new harvester ready"); // �ý��� �޽��� ���
	}
	else {
		display_message("Not enough spice"); // ��ġ ���� �޽���
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

