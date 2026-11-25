#include "snake.h"
#include "raygui.h"

int main()
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME);
    SetWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    SetTargetFPS(FRAME_RATE);
    SetExitKey(KEY_ESCAPE);
    SetTraceLogLevel(LOG_DEBUG);
    
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);

    game *g = new game();
    spawn_apple(g);

    while (!WindowShouldClose())
    {
        ++g->frame_count;

        if (IsKeyPressed(KEY_R)) {
            delete g;
            g = new game();
            spawn_apple(g);
        }

        if (not g->game_over) {
            update_next_direction(g);
            update_snake(g); 
        }
        
        BeginDrawing();
            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
            draw_board();
            draw_snake(&g->snake);
            draw_apple(g);
            draw_right_hud(g);
        EndDrawing();
    } 
}
