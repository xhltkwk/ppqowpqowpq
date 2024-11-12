/*
*  display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "common.h"

// 표시할 색상 정의. 대충 맞춰 뒀는데, 취향껏 추가하거나 변경하기
#define COLOR_DEFAULT	15
#define COLOR_CURSOR	112
#define COLOR_RESOURCE  112
// 색상 정의 추가
#define COLOR_BLUE     9   // 아트레이디스 (플레이어) - 푸른색
#define COLOR_RED      12  // 하코넨 (AI) - 빨간색
#define COLOR_YELLOW   14  // 샌드웜 - 황토색
#define COLOR_BLACK    0   // 장판 - 검은색
#define COLOR_ORANGE   6   // 스파이스 - 주황색
#define COLOR_GRAY     8   // 기타 지형 - 회색
#define COLOR_WHITE    15

// 지금은 자원, 맵, 커서만 표시
// 앞으로 화면에 표시할 내용들 여기에 추가하기
void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor
);

#endif
#pragma once
