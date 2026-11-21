#include "../raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "../raygui.h"

#include <deque>
#include <vector>
#include <string>

#define WINDOW_NAME "Snake"

#define BOARD_WIDTH 600
#define BOARD_HEIGHT 600

#define BOARD_ROWS 30
#define BOARD_COLS 30

#define WINDOW_WIDTH 900
#define WINDOW_HEIGHT 600

#define CELL_HEIGHT (BOARD_HEIGHT / BOARD_ROWS)
#define CELL_WIDTH (BOARD_WIDTH / BOARD_COLS)

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

state game_state { STATE_READY };

int curr_direction{};
int next_direction{};
int frame_count{};
int move_count{};

deque<cell_location> snake;
cell_location apple;

bool locations_intersect(cell_location a, cell_location b)
{
    return (a.col == b.col and a.row == b.row);
}

bool location_intersects_snake(cell_location loc)
{
    bool intersects { false };
    for (int i{}; i < snake.size(); ++i) {
        intersects = locations_intersect(loc, snake[i]);
    }
    return intersects;
}

bool snake_head_intersects_body()
{
    if (snake.size() > 4) {
        for (int i{4}; i < snake.size(); ++i) {
            if(locations_intersect(snake.front(), snake[i])) {
                return true;
            }
        }
    }
    return false;
}

bool location_outside_board(cell_location loc)
{
    bool is_outside {
        (loc.col < 0) or
        (loc.row < 0) or
        (loc.col > BOARD_COLS - 1) or
        (loc.row > BOARD_ROWS - 1)
    };
    return is_outside;
};

void draw_apple()
{
    DrawRectangle(
        (apple.col * CELL_WIDTH) + (CELL_WIDTH / 4),
        (apple.row * CELL_HEIGHT) + (CELL_HEIGHT / 4),
        CELL_HEIGHT / 2,
        CELL_HEIGHT / 2,
        APPLE_COLOR
    );
}

void draw_snake()
{
    for (int i{}; i < snake.size(); ++i) {
        cell_location curr { snake[i] };
        // Draw the center section.
        DrawRectangle(
            (curr.col * CELL_WIDTH) + (CELL_WIDTH / 4),
            (curr.row * CELL_HEIGHT) + (CELL_HEIGHT / 4),
            CELL_HEIGHT / 2,
            CELL_HEIGHT / 2,
            {0, 255, 105, 255}
        );
        
        // Connect this section with the next one toward the tail it exists.
        if (i < snake.size() - 1) {
            cell_location next { snake[i + 1] };
            int x_shift { (next.col - curr.col) * (CELL_WIDTH / 4) };
            int y_shift { (next.row - curr.row) * (CELL_HEIGHT / 4) };
            DrawRectangle(
                (curr.col * CELL_WIDTH) + (CELL_WIDTH / 4) + x_shift,
                (curr.row * CELL_HEIGHT) + (CELL_HEIGHT / 4) + y_shift,
                CELL_HEIGHT / 2,
                CELL_HEIGHT / 2,
                {0, 255, 105, 255}
            );
        }
        // Connect this section with next one toward the head if it exists.
        if (i > 0) {
            cell_location last { snake[i - 1] };
            int x_shift { (last.col - curr.col) * (CELL_WIDTH / 4) };
            int y_shift { (last.row - curr.row) * (CELL_HEIGHT / 4) };
            DrawRectangle(
                (curr.col * CELL_WIDTH) + (CELL_WIDTH / 4) + x_shift,
                (curr.row * CELL_HEIGHT) + (CELL_HEIGHT / 4) + y_shift,
                CELL_HEIGHT / 2,
                CELL_HEIGHT / 2,
                {0, 255, 105, 255}
            );
        }
    }
} 

void draw_board()
{
    DrawRectangle(
        0,
        0,
        BOARD_WIDTH,
        BOARD_HEIGHT,
        EMPTY_CELL_COLOR 
    );
    // Outlines.
    for (int col{}; col < BOARD_COLS; col++)  {
    for (int row{}; row < BOARD_ROWS; row++) { 
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

void draw_right_hud()
{
    GuiLabel(
        (Rectangle){BOARD_WIDTH + 15, 10, 130, 20},
        "Snake Size: "
    );
    GuiLabel(
        (Rectangle){BOARD_WIDTH + 130, 10, 130, 20},
        to_string(snake.size()).c_str()
    );
    GuiLabel(
        (Rectangle){BOARD_WIDTH + 15, 40, 130, 20},
        "Moves: "
    );
    GuiLabel(
        (Rectangle){BOARD_WIDTH + 90, 40, 130, 20},
        to_string(move_count).c_str()
    );
}

void spawn_apple()
{
    // Empty cells.
    vector<cell_location> empty_cells;
    for (int col{}; col < BOARD_COLS; col++) {
    for (int row{}; row < BOARD_ROWS; row++) {
        cell_location cell { col, row };
        if (not location_intersects_snake(cell)) {
            empty_cells.push_back(cell); 
        }
    }
    }
    int random_index { GetRandomValue(0, empty_cells.size() - 1) };
    apple = empty_cells[random_index];
}

void update_snake()
{
    cell_location old_tail { snake.back() };
    cell_location old_head { snake.front() };
    cell_location new_head { old_head };

    switch (next_direction) {
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
    curr_direction = next_direction;
 
    snake.push_front(new_head);
    snake.pop_back();

    if (location_outside_board(new_head) or snake_head_intersects_body()) {
        game_state = STATE_GAME_OVER;
        return;
    }

    if (locations_intersect(snake.front(), apple)) {
        snake.push_back(old_tail);
        spawn_apple();
    }

    ++move_count;
}

void reset_game()
{
    snake.clear();

    cell_location head {
        BOARD_ROWS / 2,
        BOARD_COLS / 2
    };
    snake.push_back(head);

    curr_direction = 0;
    next_direction = 0;
    frame_count = 0;
    move_count = 0;

    spawn_apple();

    game_state = STATE_READY;
}

void update_next_direction()
{ 
    if (IsKeyPressed(KEY_W) and curr_direction != KEY_S) {
        next_direction = KEY_W;
    }
    else if (IsKeyPressed(KEY_A) and curr_direction != KEY_D) {
        next_direction = KEY_A;
    }   
    else if (IsKeyPressed(KEY_S) and curr_direction != KEY_W) {
        next_direction = KEY_S;
    }
    else if (IsKeyPressed(KEY_D) and curr_direction != KEY_A) {
        next_direction = KEY_D;
    }

    if (curr_direction == 0) {
        curr_direction = next_direction;
    }
}

void update()
{
    update_next_direction();
    switch (game_state) {
        case STATE_READY:
            if (curr_direction != 0) {
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
    
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);

    reset_game();

    while (!WindowShouldClose())
    {
        ++frame_count;

        if (IsKeyPressed(KEY_R)) {
            reset_game();
        }

        update();

        BeginDrawing();

        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
        draw_board();
        draw_snake();
        draw_apple();
        draw_right_hud();

        EndDrawing();
    } 
}
