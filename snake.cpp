#include "raylib.h"

#include <deque>
#include <string>

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600
#define WINDOW_NAME "Snake"

#define FRAME_RATE 60

#define BOARD_WIDTH 15
#define BOARD_HEIGHT 15

#define CELL_WIDTH WINDOW_WIDTH / BOARD_WIDTH
#define CELL_HEIGHT WINDOW_HEIGHT / BOARD_HEIGHT

#define BASE_MOVE_RATE 10

using std::deque;
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

enum state {
    STATE_READY,
    STATE_PLAYING,
    STATE_GAME_OVER
};

struct coordinates {
    int x;
    int y;
};

state game_state = STATE_READY;

int direction = 0;
int frame_count = 0;

cell board[BOARD_WIDTH][BOARD_HEIGHT] = {CELL_EMPTY};

deque<coordinates> snake;

void reset_board()
{
    for (int col = 0; col < BOARD_WIDTH; col++)  {
    for (int row = 0; row < BOARD_HEIGHT; row++) {
        board[col][row] = CELL_EMPTY;
    }
    }
}

void draw_board()
{
    for (int col = 0; col < BOARD_WIDTH; col++)  {
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

void spawn_food()
{
    coordinates food = {
        GetRandomValue(0, BOARD_WIDTH - 1),
        GetRandomValue(0, BOARD_HEIGHT - 1)
    };
    board[food.x][food.y] = CELL_FOOD;
}

void reset_snake()
{
    snake.clear();
    coordinates head = {
        BOARD_WIDTH / 2,
        BOARD_HEIGHT / 2
    };

    snake.push_back(head);
    board[head.x][head.y] = CELL_SNAKE;
    direction = 0;
}

void update_snake()
{
    coordinates old_head = snake.front();
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

    bool snake_hit_wall = (new_head.x < 0) or
                          (new_head.y < 0) or
                          (new_head.x > BOARD_WIDTH - 1) or
                          (new_head.y > BOARD_HEIGHT - 1);

    bool snake_hit_itself = board[new_head.x][new_head.y] == CELL_SNAKE;
    bool snake_ate_food = board[new_head.x][new_head.y] == CELL_FOOD;

    if (snake_hit_wall or snake_hit_itself) {
        game_state = STATE_GAME_OVER;
        return;
    }

    snake.push_front(new_head);  
    board[new_head.x][new_head.y] = CELL_SNAKE;

    coordinates old_tail = snake.back();
    board[old_tail.x][old_tail.y] = CELL_EMPTY;

    if (snake_ate_food) {
        spawn_food();
    } else {
        snake.pop_back();
    }
}

void reset_game()
{
    reset_snake();
    reset_board();
    spawn_food();
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
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME);
    SetWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
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

        EndDrawing();
    } 
}
