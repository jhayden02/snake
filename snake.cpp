#include "raylib.h"

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600
#define WINDOW_NAME "Snake"

#define FRAME_RATE 60

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 10

#define CELL_WIDTH WINDOW_WIDTH / BOARD_WIDTH
#define CELL_HEIGHT WINDOW_HEIGHT / BOARD_HEIGHT

enum cell {
    CELL_EMPTY,
    CELL_BODY,
    CELL_HEAD,
    CELL_FOOD
};

Color cell_colors[4] = {
    RAYWHITE,
    BLUE,
    BLUE,
    RED
};

cell board[BOARD_WIDTH][BOARD_HEIGHT] = {CELL_EMPTY};

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
}

void update_snake()
{

}

int main()
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME);
    SetWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    SetTargetFPS(FRAME_RATE);
    SetExitKey(KEY_ESCAPE);
    SetTraceLogLevel(LOG_DEBUG);

    board[2][3] = CELL_BODY;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        draw_board();
        EndDrawing();
    } 
}
