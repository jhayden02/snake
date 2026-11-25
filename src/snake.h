#pragma once

#include "raylib.h"

#include <deque>

using std::deque;

constexpr const char *const WINDOW_NAME { "Snake" };
constexpr int BOARD_WIDTH { 600 };
constexpr int BOARD_HEIGHT { 600 };
constexpr int BOARD_SIZE { 30 };
constexpr int WINDOW_WIDTH { 900 };
constexpr int WINDOW_HEIGHT { 600 };

constexpr int CELL_HEIGHT { BOARD_HEIGHT / BOARD_SIZE };
constexpr int CELL_WIDTH { BOARD_WIDTH / BOARD_SIZE };

constexpr int CELL_OUTLINE_PIXELS { 1 };
constexpr Color CELL_OUTLINE_COLOR { 0, 0, 0, 50 };

constexpr Color EMPTY_CELL_COLOR { 80, 80, 80, 255 };
constexpr Color SNAKE_BACKGROUND_COLOR { 40, 40, 40, 255 };
constexpr Color APPLE_COLOR { 255, 60, 95, 255 };

constexpr int FRAME_RATE { 60 };
constexpr int BASE_MOVE_RATE { 5 };

struct cell {
    int col;
    int row;
};

enum state {
    STATE_READY,
    STATE_PLAYING,
    STATE_GAME_OVER
};

struct game {
    int curr_direction { 0 };
    int next_direction { 0 };

    int frame_count { 0 };
    int move_count { 0 };
    int move_rate { BASE_MOVE_RATE };

    bool game_over { false };

    deque<cell> snake { {BOARD_SIZE / 2, BOARD_SIZE / 2}  };
    cell apple;
};

bool cells_intersect(cell a, cell b);
bool cell_intersects_snake(cell c, deque<cell> *snake);
bool snake_head_intersects_body(deque<cell> *snake);
bool cell_outside_board(cell c);

void draw_apple(game *g);
void draw_snake(deque<cell> *snake);
void draw_board();

void draw_right_hud(game *g);

void spawn_apple(game *g);
void update_snake(game *g);
void update_next_direction(game *g);
void init_game(game *g);
