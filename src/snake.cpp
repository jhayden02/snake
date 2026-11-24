#include "snake.h"

bool locations_intersect(cell_location a, cell_location b)
{
    return (a.col == b.col and a.row == b.row);
}

bool location_intersects_snake(cell_location loc, deque<cell_location> *snake)
{
    for (int i{}; i < snake->size(); ++i) {
        if (locations_intersect(loc, snake->at(i))) {
            return true;
        }
    }
    return false;
}

bool snake_head_intersects_body(deque<cell_location> *snake)
{
    if (snake->size() > 4) {
        for (int i{4}; i < snake->size(); ++i) {
            if(locations_intersect(snake->front(), snake->at(i))) {
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
        (loc.col > BOARD_SIZE - 1) or
        (loc.row > BOARD_SIZE - 1)
    };
    return is_outside;
};

void draw_apple(game *g)
{
    DrawRectangle(
        (g->apple.col * CELL_WIDTH) + (CELL_WIDTH / 4),
        (g->apple.row * CELL_HEIGHT) + (CELL_HEIGHT / 4),
        CELL_HEIGHT / 2,
        CELL_HEIGHT / 2,
        APPLE_COLOR
    );
}

void draw_snake(deque<cell_location> *snake)
{
    for (int i{}; i < snake->size(); ++i) {
        cell_location curr { snake->at(i) };
        // Draw the center section.
        DrawRectangle(
            (curr.col * CELL_WIDTH) + (CELL_WIDTH / 4),
            (curr.row * CELL_HEIGHT) + (CELL_HEIGHT / 4),
            CELL_HEIGHT / 2,
            CELL_HEIGHT / 2,
            {0, 255, 105, 255}
        );
        
        // Connect this section with the next one toward the tail it exists.
        if (i < snake->size() - 1) {
            cell_location next { snake->at(i + 1) };
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
            cell_location last { snake->at(i - 1) };
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
    for (int col{}; col < BOARD_SIZE; col++)  {
    for (int row{}; row < BOARD_SIZE; row++) { 
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

void draw_right_hud(game *g)
{
    GuiLabel(
        (Rectangle){BOARD_WIDTH + 15, 10, 130, 20},
        "Snake Size: "
    );
    GuiLabel(
        (Rectangle){BOARD_WIDTH + 130, 10, 130, 20},
        to_string(g->snake.size()).c_str()
    );
    GuiLabel(
        (Rectangle){BOARD_WIDTH + 15, 40, 130, 20},
        "Moves: "
    );
    GuiLabel(
        (Rectangle){BOARD_WIDTH + 90, 40, 130, 20},
        to_string(g->move_count).c_str()
    );
}

void spawn_apple(game *g)
{
    //TraceLog(LOG_DEBUG, (string){ "Apple #" + to_string(snake->size()) }.c_str());
    // Empty cells.
    vector<cell_location> empty_cells;
    for (int col{}; col < BOARD_SIZE; col++) {
    for (int row{}; row < BOARD_SIZE; row++) {
        cell_location cell { col, row };
        if (location_intersects_snake(cell, &g->snake)) {
            //TraceLog(LOG_DEBUG, (string){ "Snake: { " + to_string(cell.col) + ", " + to_string(cell.row) + "}" }.c_str());
        } else {
            empty_cells.push_back(cell); 
        }
    }
    }
    int random_index { GetRandomValue(0, empty_cells.size() - 1) };
    g->apple = empty_cells[random_index];
    //TraceLog(LOG_DEBUG, (string){ "Apple: { " + to_string(apple.col) + ", " + to_string(apple.row) + "}" }.c_str());
}

void update_snake(game *g)
{
    // Return if the frame count is not a multiple of the move rate or if the player has not moved yet.
    if (g->frame_count % g->move_rate or g->curr_direction == 0) {
        return;
    }

    cell_location old_tail { g->snake.back() };
    cell_location old_head { g->snake.front() };
    cell_location new_head { old_head };

    switch (g->next_direction) {
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
    g->curr_direction = g->next_direction;
 
    g->snake.push_front(new_head);
    g->snake.pop_back();

    if (location_outside_board(new_head) or snake_head_intersects_body(&g->snake)) {
        g->game_over = true;
        return;
    }

    if (locations_intersect(g->snake.front(), g->apple)) {
        g->snake.push_back(old_tail);
        spawn_apple(g);
    }

    ++g->move_count;
}

void update_next_direction(game *g)
{ 
    if (IsKeyPressed(KEY_W) and g->curr_direction != KEY_S) {
        g->next_direction = KEY_W;
    }
    else if (IsKeyPressed(KEY_A) and g->curr_direction != KEY_D) {
        g->next_direction = KEY_A;
    }   
    else if (IsKeyPressed(KEY_S) and g->curr_direction != KEY_W) {
        g->next_direction = KEY_S;
    }
    else if (IsKeyPressed(KEY_D) and g->curr_direction != KEY_A) {
        g->next_direction = KEY_D;
    }

    if (g->curr_direction == 0) {
        g->curr_direction = g->next_direction;
    }
}

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

        update_next_direction(g);

        if (not g->game_over) {
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
