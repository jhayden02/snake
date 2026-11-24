#pragma once

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <deque>
#include <vector>
#include <string>

#define WINDOW_NAME "Snake"

#define BOARD_WIDTH 600
#define BOARD_HEIGHT 600

#define BOARD_SIZE 30

#define WINDOW_WIDTH 900
#define WINDOW_HEIGHT 600

#define CELL_HEIGHT (BOARD_HEIGHT / BOARD_SIZE)
#define CELL_WIDTH (BOARD_WIDTH / BOARD_SIZE)

#define CELL_OUTLINE_PIXELS 1
#define CELL_OUTLINE_COLOR {0, 0, 0, 50}

#define EMPTY_CELL_COLOR { 80, 80, 80, 255 }
#define SNAKE_BACKGROUND_COLOR { 40, 40, 40, 255 }
#define APPLE_COLOR {255, 60, 95, 255}

#define FRAME_RATE 60
#define BASE_MOVE_RATE 5

using std::deque;
using std::vector;
using std::string;
using std::to_string;

struct cell_location {
    int col;
    int row;
};

enum state {
    STATE_READY,
    STATE_PLAYING,
    STATE_GAME_OVER
};

struct game {
    int curr_direction{0};
    int next_direction{0};
    int frame_count{0};
    int move_count{0};
    int move_rate{BASE_MOVE_RATE};
    bool game_over{false};
    deque<cell_location> snake{ {BOARD_SIZE / 2, BOARD_SIZE / 2}  };
    cell_location apple{};
};

bool locations_intersect(cell_location a, cell_location b);
bool location_intersects_snake(cell_location loc, deque<cell_location> *snake);
bool snake_head_intersects_body(deque<cell_location> *snake);
bool location_outside_board(cell_location loc);

void draw_apple(game *g);
void draw_snake(game *g);
void draw_board();

void draw_right_hud();

void spawn_apple(game *g);
void update_snake(game *g);
void update_next_direction(game *g);
void init_game(game *g);
