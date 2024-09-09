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
#include "screens.h"

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int _frames_counter = 0;
static int _finish_screen = 0;

struct Letters {
    // ASCII only for now
    static const int count = 27;
    Texture2D texture;
    Rectangle rectangles[count] = { 0 };
    float scale;
};

static Letters _letters;

struct Board {
    Texture2D texture_space;
    float space_scale;
    int rows;
    int columns;
    Vector2 board_position = {0};
    // Fixed space increase if we really need more
    char letters[32] = { 0 };
    Vector2 well_position = { 0 };
    static const int max_well_letters = 5;
    char well[max_well_letters] = { 0 };
};

static Board _board;

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

static void letters_unload(Letters* letters) {
    UnloadTexture(letters->texture);
    letters->texture = { 0 };
}

static void letters_draw(Letters* letters, char c, Vector2 pos, float scale)
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
        board->letters[i] = 'a';
    }

    for (int i = 0; i < board->max_well_letters; ++i) {
        board->well[i] = 'b';
    }
}

static void board_unload(Board* board) {
    UnloadTexture(board->texture_space);
    board->texture_space = { 0 };
}

static void board_draw(Board* board, Vector2 board_position, Vector2 well_position)
{
    const int space_size = board->texture_space.width * board->space_scale;
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
        letters_draw(&_letters, board->well[i], Vector2{ well_position.x + letter_margin,well_position.y + i * space_size + letter_margin }, .25f);
    }
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

}

// Gameplay Screen Update logic
void update_game_screen(void)
{    // Press enter or tap to change to ENDING screen
    if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
    {
        _finish_screen = 1;
    }
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

    board_draw(&_board, Vector2(20, 20), Vector2(400, 20));
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