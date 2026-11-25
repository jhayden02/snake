#define RAYGUI_IMPLEMENTATION

#include "snake.h"
#include "raygui.h"

#include <vector>
#include <string>

using std::vector;
using std::string;
using std::to_string;

bool cells_intersect(cell a, cell b)
{
    return (a.col == b.col and a.row == b.row);
}

bool cell_intersects_snake(cell c, deque<cell> *snake)
{
    for (int i{}; i < snake->size(); ++i) {
        if (cells_intersect(c, snake->at(i))) {
            return true;
        }
    }
    return false;
}

bool snake_head_intersects_body(deque<cell> *snake)
{
    if (snake->size() > 4) {
        for (int i{4}; i < snake->size(); ++i) {
            if(cells_intersect(snake->front(), snake->at(i))) {
                return true;
            }
        }
    }
    return false;
}

bool cell_outside_board(cell c)
{
    bool is_outside {
        (c.col < 0) or
        (c.row < 0) or
        (c.col > BOARD_SIZE - 1) or
        (c.row > BOARD_SIZE - 1)
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

void draw_snake(deque<cell> *snake)
{
    for (int i{}; i < snake->size(); ++i) {
        cell curr { snake->at(i) };
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
            cell next { snake->at(i + 1) };
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
            cell last { snake->at(i - 1) };
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
    vector<cell> empty_cells;
    for (int col{}; col < BOARD_SIZE; col++) {
    for (int row{}; row < BOARD_SIZE; row++) {
        cell c { col, row };
        if (cell_intersects_snake(c, &g->snake)) {
            //TraceLog(LOG_DEBUG, (string){ "Snake: { " + to_string(cell.col) + ", " + to_string(cell.row) + "}" }.c_str());
        } else {
            empty_cells.push_back(c); 
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

    cell old_tail { g->snake.back() };
    cell old_head { g->snake.front() };
    cell new_head { old_head };

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

    if (cell_outside_board(new_head) or snake_head_intersects_body(&g->snake)) {
        g->game_over = true;
        return;
    }

    if (cells_intersect(g->snake.front(), g->apple)) {
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
