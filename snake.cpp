#include "raylib.h"

#include <vector>
#include <string>

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600
#define WINDOW_NAME "Snake"

#define FRAME_RATE 60

#define BOARD_WIDTH 30
#define BOARD_HEIGHT 30

#define CELL_WIDTH WINDOW_WIDTH / BOARD_WIDTH
#define CELL_HEIGHT WINDOW_HEIGHT / BOARD_HEIGHT

using std::vector;
using std::string;
using std::to_string;

enum cell {
    CELL_EMPTY,
    CELL_SNAKE,
    CELL_FOOD
};

Color cell_colors[3] = {
    RAYWHITE,
    BLUE,
    RED
};

struct coordinates {
    int x;
    int y;
};

int direction = KEY_D;
bool game_over = false;
int frame_count = 0;
int move_rate = 60;

cell board[BOARD_WIDTH][BOARD_HEIGHT] = {CELL_EMPTY};

vector<coordinates> snake;

void draw_board()
{
    for (int col = 0; col < BOARD_WIDTH; col++) {
        for (int row = 0; row < BOARD_HEIGHT; row++) {
            cell current_cell = board[col][row];
            DrawRectangle(
                col * CELL_WIDTH,
                row * CELL_HEIGHT,
                CELL_WIDTH,
                CELL_HEIGHT,
                cell_colors[current_cell]
            ); 
        }
    }
    string snake_size = "Score: " + to_string(snake.size());
    DrawText(snake_size.c_str(), 10, 10, 20, BLACK);
}

void init_snake()
{
    coordinates head = {
        GetRandomValue(0, BOARD_WIDTH),
        GetRandomValue(0, BOARD_HEIGHT)
    };

    snake.push_back(head);
    board[head.x][head.y] = CELL_SNAKE;
}

void update_snake()
{
    coordinates old_head = snake[0];
    coordinates new_head = old_head;

    switch (direction) {
        case KEY_A:
            new_head.x -= 1;
            break;
        case KEY_D: 
            new_head.x += 1;
            break;
        case KEY_W:
            new_head.y -= 1;
            break;
        case KEY_S:
            new_head.y += 1;
            break;
    };

    if (new_head.x < 0 | new_head.x > BOARD_WIDTH | new_head.y < 0 | new_head.y > BOARD_HEIGHT) {
        game_over = true;
    }

    snake.insert(snake.begin(), new_head); 
    board[new_head.x][new_head.y] = CELL_SNAKE;

    coordinates old_tail = snake.back();
    board[old_tail.x][old_tail.y] = CELL_EMPTY;
    snake.pop_back();
}

int main()
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME);
    SetWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    SetTargetFPS(FRAME_RATE);
    SetExitKey(KEY_ESCAPE);
    SetTraceLogLevel(LOG_DEBUG);

    init_snake();

    while (!WindowShouldClose())
    {
        frame_count++;

        if (game_over) {
            continue;
        }

        int key_pressed = GetKeyPressed();
        if (key_pressed != 0) {
            direction = key_pressed;
        }

        if (frame_count % move_rate == 0) {
            update_snake(); 
        }

        BeginDrawing();

        ClearBackground(RAYWHITE);
        draw_board();

        EndDrawing();
    } 
}
