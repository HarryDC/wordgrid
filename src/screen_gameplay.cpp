/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Gameplay Screen Functions Definitions (Init, Update, Draw, Unload)
*
*   Copyright (c) 2014-2022 Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#include "raylib.h"
#include "raymath.h"
#include "screens.h"

#include "dictionary.h"

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int _frames_counter = 0;
static int _finish_screen = 0;

enum class Action {
    None,
    Pickup,
    Drop,
};

Action _current_action = Action::None;

struct Letters {
    // ASCII only for now
    static const int count = 27;
    Texture2D texture;
    Rectangle rectangles[count] = { 0 };
    float scale;
};

static Letters _letters;

struct Layout {
    Vector2 board_pos;
    Rectangle board_rect;
    Vector2 well_pos;
    Rectangle well_rect;
    float tile_size;
};

static Layout _layout;

struct Board {
    Texture2D texture_space;
    float space_scale;
    int rows;
    int columns;
    Vector2 board_position = {0};
    // Fixed space increase if we really need more
    int letters[32] = { 0 };
    Vector2 well_position = { 0 };
    static const int max_well_letters = 5;
    int well[max_well_letters] = { 0 };
};

static Board _board;

struct DragInfo {
    bool is_dragging = false;
    int original_index = -1;
    char letter = -1;
    Vector2 position{ 0 };
};

static DragInfo _drag_info;

static Dictionary _dictionary;

static void letters_init(Letters *letters, const char* filename) {
    int letter_width = 256;
    int letter_height = 256;
    int row_count = 7;
    int row_gap = 2;
    int col_count = 4;
    int col_gap = 2;
    const char letter_order[27] = { 
        'y', 'r', 'k', 'd',
        'x', 'q', 'j', 'c',
        'w', 'p', 'i', 'b',
        'v', 'o', 'h', 'a',
        'u', 'n', 'g', ' ',
        't', 'm', 'f', 'l',
        's', 'z', 'e'
    };
    letters->texture = LoadTexture(filename);
    for (int row = 0; row < row_count; ++row) {
        for (int col = 0; col < col_count; ++col) {
            Rectangle rect = {
                .x = (float)(col * (letter_width + col_gap)),
                .y = (float)(row * (letter_height + row_gap)),
                .width = (float)letter_width,
                .height = (float)letter_height,
            };
            int index = letter_order[row * col_count + col] - 97 + 1;
            // Space will be less than 1, skip the last rect
            if (letter_order[row * col_count + col] == ' ') index = 0;

            if (index < letters->count) letters->rectangles[index] = rect;
        }
    }
}


static int random_letter()
{
    return GetRandomValue('a', 'z');
}

static void letters_unload(Letters* letters) {
    UnloadTexture(letters->texture);
    letters->texture = { 0 };
}

static void letters_draw(Letters* letters, int c, Vector2 pos, float scale)
{
    int index = c - 96;
    Rectangle target = {
        .x = pos.x,
        .y = pos.y,
        .width = (float)216 * scale, // Measure interior space of tile that is being used
        .height = (float)216 * scale,
    };
    DrawTexturePro(letters->texture, letters->rectangles[index], target, Vector2 { 0, 0 }, 0, WHITE);
}

static void board_init(Board* board, const char* filename, int rows, int cols) {
    board->rows = rows;
    board->columns = cols;
    board->texture_space = LoadTexture(filename);
    if (board->texture_space.id == 0) {
        TraceLog(LOG_ERROR, "Failed to load board space from %s/%s", GetWorkingDirectory(), filename);
        return;
    }
    board->space_scale = .25f;
    for (int i = 0; i < rows * cols; ++i) {
        board->letters[i] = -1;
    }

    for (int i = 0; i < board->max_well_letters; ++i) {
        board->well[i] = random_letter();
    }
}

static char board_get_letter(Board* board, int x, int y) {
    if (x < 0 || x >= board->columns || y < 0 || y >= board->rows) {
        TraceLog(LOG_FATAL, "Invalid access to board (%i,%i)", x, y);
        return -1;
    }
    return board->letters[x * board->rows + y];
}

static void board_set_letter(Board* board, int x, int y, char c) {
    if (x < 0 || x >= board->columns || y < 0 || y >= board->rows) {
        TraceLog(LOG_FATAL, "Invalid access to board (%i,%i)", x, y);
        return;
    }
    board->letters[x * board->rows + y] = c;
}

static void board_unload(Board* board) {
    UnloadTexture(board->texture_space);
    board->texture_space = { 0 };
}

static void board_draw(Board* board, Vector2 board_position, Vector2 well_position)
{
    const float space_size = (float)board->texture_space.width * board->space_scale;
    const int letter_margin = 8; // From image full scale is 32, we're using quarter size => 8
    const float board_scale = .25;

    // TODO #optimization unit sprite sheet into one and draw from one texture 

    for (int i = 0; i < board->rows; ++i) {
        float x = board_position.x + i * space_size;
        for (int j = 0; j < board->columns; ++j) {
            float y = board_position.y + j * space_size;
            DrawTextureEx(board->texture_space, Vector2{ x, y }, 0, board_scale, WHITE);
        }
    }

    for (int i = 0; i < board->max_well_letters; ++i) {
        DrawTextureEx(board->texture_space, Vector2{ well_position.x, well_position.y + i * space_size }, 0, board_scale, WHITE);
    }

    for (int i = 0; i < board->rows; ++i) {
        float x = board_position.x + i * space_size + letter_margin;
        for (int j = 0; j < board->columns; ++j) {
            float y = board_position.y + j * space_size + letter_margin;
            int letter = board->letters[i * board->columns + j];
            if (letter > 0) {
                letters_draw(&_letters, letter, Vector2{ x,y }, .25f);
            }
        }
    }

    for (int i = 0; i < board->max_well_letters; ++i) {
        if (board->well[i] >= 0)
        {
           letters_draw(&_letters, board->well[i], Vector2{ well_position.x + letter_margin,well_position.y + i * space_size + letter_margin }, .25f);
        }
    }
}

static void input_update() {
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        _current_action = Action::Drop;
        TraceLog(LOG_INFO, "Mouse Released");
    }
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        _current_action = Action::Pickup;
        TraceLog(LOG_INFO, "Mouse Pressed");
    }
}

static void drag_update(DragInfo* drag, Board* board) {
    Vector2 mouse_pos = GetMousePosition();

    if (_current_action == Action::Pickup) {
        if (CheckCollisionPointRec(mouse_pos, _layout.well_rect)) {
            float dist = mouse_pos.y - _layout.well_rect.y;
            int index = (int)(dist / _layout.tile_size);
            if (index < 0 || index > board->max_well_letters) {
                TraceLog(LOG_WARNING, "Mouse pickup error, index wrong [%s], ", index);
                return;
            }
            drag->is_dragging = true;
            drag->letter = board->well[index];
            drag->original_index = index;
            board->well[index] = -1;
            drag->position = mouse_pos;
        }
    }
    else if (_current_action == Action::Drop) {
        bool drop_success = false;
        if (CheckCollisionPointRec(mouse_pos, _layout.board_rect)) {
            Vector2 dist = Vector2Scale(
                Vector2Subtract(mouse_pos, _layout.board_pos), 1.0f/(float)_layout.tile_size);
            int x = (int)dist.x;
            int y = (int)dist.y;
            char letter = board_get_letter(board, x, y);
            if (letter == -1) {
                drop_success = true;
                board_set_letter(board, x, y, drag->letter);
                board->well[drag->original_index] = random_letter();

                // Check Word horizontal, CheckWord vertical
                // Remove if match
            }
        }

        if (drop_success == true) {
            // TODO add random letter to well
        }
        else {
            board->well[drag->original_index] = drag->letter;
        }

        drag->is_dragging = false;
    }

    if (drag->is_dragging) {
        drag->position = mouse_pos;
    }

    _current_action = Action::None;
}

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------

// Gameplay Screen Initialization logic
void init_game_screen(void)
{
    // TODO: Initialize GAMEPLAY screen variables here!
    _frames_counter = 0;
    _finish_screen = 0;

    letters_init(&_letters, "resources/solid_spritesheet.png");
    board_init(&_board, "resources/tile_space.png", 5, 5);

    float tile_size = _board.texture_space.width * 0.25f; // Assumes square

    _layout.board_rect = Rectangle{
        .x = 20, .y = 20,
        .width = tile_size * _board.columns,
        .height = tile_size * _board.rows
    };
    _layout.board_pos = Vector2{ _layout.board_rect.x, _layout.board_rect.y };

    _layout.well_rect = Rectangle{
        .x = 400, .y = 20,
        .width = tile_size,
        .height = tile_size * _board.max_well_letters
    };
    _layout.well_pos = Vector2{ _layout.well_rect.x, _layout.well_rect.y };
    _layout.tile_size = tile_size;

    _dictionary = dictionary_load("resources/text/en/words.txt");
}

// Gameplay Screen Update logic
void update_game_screen(void)
{    // Press enter or tap to change to ENDING screen
    //if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
    //{
    //    _finish_screen = 1;
    //}
    input_update();
    drag_update(&_drag_info, &_board);
}

// Gameplay Screen Draw logic
void draw_game_screen(void)
{
    static float time = 1.0;
    static char c = 'a';
    time -= GetFrameTime();
    if (time < 0) {
        time = 1.0;
        ++c;
    }
    // TODO: Draw GAMEPLAY screen here!
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), WHITE);

    board_draw(&_board, _layout.board_pos, _layout.well_pos);
    if (_drag_info.is_dragging) {
        letters_draw(&_letters, _drag_info.letter, _drag_info.position, 0.25f);
    }
}

// Gameplay Screen Unload logic
void unload_game_screen(void)
{
    letters_unload(&_letters);
    board_unload(&_board);
}

// Gameplay Screen should finish?
int finish_game_screen(void)
{
    return _finish_screen;
}