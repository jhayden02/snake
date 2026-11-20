#include "raylib.h"

#include <deque>
#include <vector>
#include <string>

#define WINDOW_NAME "Snake"

#define BOARD_WIDTH 600
#define BOARD_HEIGHT 600

#define BOARD_ROWS 15
#define BOARD_COLS 15

#define CELL_HEIGHT (BOARD_HEIGHT / BOARD_ROWS)
#define CELL_WIDTH (BOARD_WIDTH / BOARD_COLS)

#define CELL_OUTLINE_PIXELS 1
#define CELL_OUTLINE_COLOR {0, 0, 0, 50}

#define FRAME_RATE 60
#define BASE_MOVE_RATE 10

using std::deque;
using std::vector;
using std::string;
using std::to_string;

enum cell_content {
    CELL_EMPTY,
    CELL_SNAKE,
    CELL_FOOD
};

struct cell_location {
    int col;
    int row;
};

Color cell_colors[3] = {
    RAYWHITE,
    {0, 255, 105, 255},
    {255, 60, 95, 255}
};

enum state {
    STATE_READY,
    STATE_PLAYING,
    STATE_GAME_OVER
};

state game_state = STATE_READY;

int direction = 0;
int frame_count = 0;
int move_count = 0;

cell_content board[BOARD_COLS][BOARD_ROWS] = {CELL_EMPTY};

deque<cell_location> snake;

void reset_board()
{
    for (int col = 0; col < BOARD_COLS; col++)  {
    for (int row = 0; row < BOARD_ROWS; row++) {
        board[col][row] = CELL_EMPTY;
    }
    }
}

void draw_board()
{
    for (int col = 0; col < BOARD_COLS; col++)  {
    for (int row = 0; row < BOARD_ROWS; row++) {
        cell_content current_cell = board[col][row]; 
        DrawRectangle(
            col * CELL_WIDTH,
            row * CELL_HEIGHT,
            CELL_WIDTH,
            CELL_HEIGHT,
            cell_colors[current_cell]
        );
        DrawRectangleLines(
            col * CELL_WIDTH,
            row * CELL_HEIGHT,
            CELL_WIDTH,
            CELL_HEIGHT,
            CELL_OUTLINE_COLOR 
        );
    }
    } 
}

void draw_hud()
{
    string snake_size = "Snake Size: " + to_string(snake.size());
    DrawText(snake_size.c_str(), 15, 10, 20, BLACK);
    string moves = "Moves: " + to_string(move_count);
    DrawText(moves.c_str(), 15, 35, 20, BLACK);
}

void spawn_apple()
{
    vector<cell_location> empty_cells;
    for (int col = 0; col < BOARD_COLS; col++)  {
    for (int row = 0; row < BOARD_ROWS; row++) {
        cell_content current_cell = board[col][row];
        if (current_cell == CELL_EMPTY) {
            cell_location coords = {col, row};
            empty_cells.push_back(coords);
        }
    }
    }
    size_t random_index = GetRandomValue(0, empty_cells.size() - 1);
    cell_location spawn = empty_cells.at(random_index);
    board[spawn.col][spawn.row] = CELL_FOOD;
}

void reset_snake()
{
    snake.clear();
    cell_location head = {
        BOARD_ROWS / 2,
        BOARD_COLS / 2
    };
    snake.push_back(head);
    board[head.col][head.row] = CELL_SNAKE;
    direction = 0;
    frame_count = 0;
    move_count = 0;
}

void update_snake()
{
    cell_location old_head = snake.front();
    cell_location new_head = old_head;

    switch (direction) {
        case KEY_A:
            new_head.col -= 1;
            break;
        case KEY_D: 
            new_head.col += 1;
            break;
        case KEY_W:
            new_head.row -= 1;
            break;
        case KEY_S:
            new_head.row += 1;
            break;
    };

    bool snake_hit_wall = (new_head.col < 0) or
                          (new_head.row < 0) or
                          (new_head.col > BOARD_COLS - 1) or
                          (new_head.row > BOARD_ROWS - 1);

    bool snake_hit_itself = board[new_head.col][new_head.row] == CELL_SNAKE;
    bool snake_ate_apple = board[new_head.col][new_head.row] == CELL_FOOD;

    if (snake_hit_wall or snake_hit_itself) {
        game_state = STATE_GAME_OVER;
        return;
    }

    snake.push_front(new_head);  
    board[new_head.col][new_head.row] = CELL_SNAKE;

    cell_location old_tail = snake.back();
    board[old_tail.col][old_tail.row] = CELL_EMPTY;

    if (snake_ate_apple) {
        spawn_apple();
    } else {
        snake.pop_back();
    }

    move_count++;
}

void reset_game()
{
    reset_board();
    reset_snake();
    spawn_apple();
    game_state = STATE_READY;
}

void update_direction()
{
    int key_pressed = GetKeyPressed();

    switch(key_pressed) {
        case 0:
            break;
        case KEY_R:
            reset_game();
            break;
        case KEY_W:
            if (direction != KEY_S) direction = key_pressed;
            break;
        case KEY_A:
            if (direction != KEY_D) direction = key_pressed;
            break;
        case KEY_S:
            if (direction != KEY_W) direction = key_pressed;
            break;
        case KEY_D:
            if (direction != KEY_A) direction = key_pressed;
            break;
    };
}

void update()
{
    update_direction();
    switch (game_state) {
        case STATE_READY:
            if (direction != 0) {
                game_state = STATE_PLAYING;
            }
            break;
        case STATE_PLAYING:
            if (frame_count % BASE_MOVE_RATE == 0) {
                update_snake(); 
            }
            break;
        case STATE_GAME_OVER:
            break;
    };
    
}

int main()
{
    InitWindow(BOARD_WIDTH, BOARD_HEIGHT, WINDOW_NAME);
    SetWindowSize(BOARD_WIDTH, BOARD_HEIGHT);
    SetTargetFPS(FRAME_RATE);
    SetExitKey(KEY_ESCAPE);
    SetTraceLogLevel(LOG_DEBUG);

    reset_game();

    while (!WindowShouldClose())
    {
        frame_count++;

        update();

        BeginDrawing();

        ClearBackground(RAYWHITE);
        draw_board();
        draw_hud();

        EndDrawing();
    } 
}
