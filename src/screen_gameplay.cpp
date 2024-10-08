/*******************************************************************************************
*
*   WordGrid
*   Simple Word Puzzle Game
*   (C) Harald Scheirich 2024
*   WordGrid is is licensed under an unmodified zlib/libpng license see LICENSE
*
********************************************************************************************/

#include "raylib.h"
#include "raymath.h"
#include "raygui.h"
#include "screens.h"

#include "dictionary.h"
#include "modes.h"

#include <stdio.h>
#include <string.h>

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

enum CheckResult {
    CHECK_RESULT_NONE = 0x0,
    CHECK_RESULT_HORIZONTAL = 0x1,
    CHECK_RESULT_VERTICAL = 0x1 << 1,
    CHECK_RESULT_BOTH = CHECK_RESULT_HORIZONTAL | CHECK_RESULT_VERTICAL,
};

enum Specials {
    SPECIAL_CLEAR_COLUMN,
    SPECIAL_CLEAR_ROW,
    SPECIAL_CLEAR_TILE,
    SPECIAL_COUNT
};

using GameModeCall = void(*)();
using GameModeUpdateCall = bool(*)(Game*);
using GameModeDrawCall = void(*)(Game*);

GameModeCall mode_init_calls[MODE_COUNT] = { mode_timeattack_init, mode_moveattack_init };
GameModeUpdateCall mode_update_calls[MODE_COUNT] = { mode_timeattack_update, mode_moveattack_update };
GameModeDrawCall mode_draw_calls[MODE_COUNT] = { mode_timeattack_draw, mode_moveattack_draw };
GameModeCall mode_unload_calls[MODE_COUNT] = { mode_timeattack_unload, mode_moveattack_unload };
 
Action _current_action = Action::None;

struct Letters {
    static const int count = 27;
    Texture2D texture;
    Rectangle rectangles[count] = { 0 };
    Rectangle specials[SPECIAL_COUNT] = { 0 };
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

typedef struct Animation {
    int letter;
    Vector2 start;
    Vector2 end;
    Vector2 current;
    float current_time;
    float total_time;
} Animation;

Animation _animation;

static Board _board;

struct DragInfo {
    bool is_dragging = false;
    int original_index = -1;
    char letter = -1;
    Vector2 position{ 0 };
};

static DragInfo _drag_info;

static Dictionary _dictionary;

static char _help_text[] = "Form words by dragging tiles from the line of tiles into the grid, when a row or a column is "
"filled the word is removed and you get a score. Words can be made from left to right or from "
"top to bottom.\n\nThere are three special tiles, you can activate them by dragging them onto the board "
"'x' will remove one tile, '|' will remove a whole column and '-' will remove a row.\n\n There are two "
"game modes, Time Attack and Move Attack, in Time Attack your play time is limited but can be extended "
"by making words, in Move Attack your number of moves is limited but you can get more by making words.\n\n"
"By pushing `Refresh` you can swap out the list of letters that is available to you but you can only do "
"that as many times as indicated in the button\n\n"
"Have Fun and Good Luck!";

static bool _show_help = false;

float ease_out_cubic(float t) {
    float val = 1.0f - t;
    return 1.f - val * val * val;
}

static void letters_init(Letters *letters, const char* filename) {
    int letter_width = 256;
    int letter_height = 256;
    int row_count = 7;
    int row_gap = 2;
    int col_count = 4;
    int col_gap = 2;
    const char letter_order[27] = { 
        'Y', 'R', 'K', 'D',
        'X', 'Q', 'J', 'C',
        'W', 'P', 'I', 'B',
        'V', 'O', 'H', 'A',
        'U', 'N', 'G', ' ',
        'T', 'M', 'F', 'L',
        'S', 'Z', 'E'
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
            int index = letter_order[row * col_count + col] - 'A';
            // Space will be less than 1, skip the last rect
            // if (letter_order[row * col_count + col] == ' ') index = 0;

            if (index < letters->count) letters->rectangles[index] = rect;
        }
    }

    // Init special tiles
    for (int i = 0; i < SPECIAL_COUNT; ++i) {
        Rectangle rect = {
            .x = (float)(4 * (letter_width + col_gap)),
            .y = (float)(i * (letter_height + row_gap)),
            .width = (float)letter_width,
            .height = (float)letter_height,
        };
        letters->specials[i] = rect;
    }
}

static void letters_unload(Letters* letters) {
    UnloadTexture(letters->texture);
    letters->texture = { 0 };
}

static void letters_draw(Letters* letters, int c, Vector2 pos, float scale)
{
    Rectangle source;

    if (c < SPECIAL_COUNT) {
        source = letters->specials[c];
    }
    else {
        //TraceLog(LOG_INFO, "%i", c - 'A');
        source = letters->rectangles[c - 'A'];
    };

    Rectangle target = {
        .x = pos.x,
        .y = pos.y,
        .width = (float)216 * scale, // Measure interior space of tile that is being used
        .height = (float)216 * scale,
    };
    DrawTexturePro(letters->texture, source, target, Vector2 { 0, 0 }, 0, WHITE);
}

static int dictionary_get_letter_or_special(Dictionary* dict) {
    if (GetRandomValue(0, 24) < 1) {
        return GetRandomValue(0, 2);
    }
    else {
        return dictionary_get_random_letter(dict);
    }
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
        board->well[i] = dictionary_get_random_letter(&_dictionary);
    }
}

static Vector2 board_get_well_position(Board* board, int index) {
    // Copied from board_draw
    const float space_size = (float)board->texture_space.width * board->space_scale;
    const int letter_margin = 8; // From image full scale is 32, we're using quarter size => 8
    const float board_scale = .25;
    return Vector2{ .x = _layout.well_pos.x + letter_margin, .y = _layout.well_pos.y + index * space_size + letter_margin };
}

static int board_get_letter(Board* board, int x, int y) {
    if (x < 0 || x >= board->columns || y < 0 || y >= board->rows) {
        TraceLog(LOG_FATAL, "Invalid access to board (%i,%i)", x, y);
        return -1;
    }
    return board->letters[x * board->rows + y];
}

static void board_set_letter(Board* board, int x, int y, int c) {
    if (x < 0 || x >= board->columns || y < 0 || y >= board->rows) {
        TraceLog(LOG_FATAL, "Invalid access to board (%i,%i)", x, y);
        return;
    }
    board->letters[x * board->rows + y] = c;
}

static CheckResult board_check_words(Board* board, Dictionary* dict, int x, int y) {
    // Check horizontal
    // Check vertical 
    int word[6] = { 0 };
    for (int i = 0; i < board->columns; ++i) {
        word[i] = board_get_letter(board, i, y);
    }
    CheckResult result = dictionary_exists(dict, word, 6) ? CHECK_RESULT_HORIZONTAL : CHECK_RESULT_NONE;

    for (int i = 0; i < board->rows; ++i) {
        word[i] = board_get_letter(board, x, i);
    }

    result = (CheckResult)(result | (dictionary_exists(dict, word, 6) ? CHECK_RESULT_VERTICAL : CHECK_RESULT_NONE));
    return result;
}



static void board_clear_words(Board* board, int x, int y, CheckResult where) {
    if ((where & CHECK_RESULT_HORIZONTAL) != 0) {
        for (int i = 0; i < board->columns; ++i) {
            board_set_letter(board, i, y, -1);
        }
    }
    if ((where & CHECK_RESULT_VERTICAL) != 0) {
        for (int i = 0; i < board->rows; ++i) {
            board_set_letter(board, x, i, -1);
        }
    }
}

void board_drop_tile(Board* board, int x, int y, int letter) {
    switch (letter) {
    case SPECIAL_CLEAR_TILE:
        board_set_letter(board, x, y, -1);
        break;
    case SPECIAL_CLEAR_ROW:
        board_clear_words(board, x, y, CHECK_RESULT_HORIZONTAL);
        break;
    case SPECIAL_CLEAR_COLUMN:
        board_clear_words(board, x, y, CHECK_RESULT_VERTICAL);
        break;
    default:
        board_set_letter(board, x, y, letter);
    }
}

static void board_reset(Board* board) {
    for (int i = 0; i < 32; ++i) {
        board->letters[i] = -1;
    }
}

static void board_reset_well(Board* board) {
    for (int i = 0; i < board->max_well_letters; ++i) {
        board->well[i] = dictionary_get_letter_or_special(&_dictionary);
    }
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

static void input_update(DragInfo* dragging) {
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && dragging->is_dragging) {
        _current_action = Action::Drop;
    }
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !dragging->is_dragging) {
        _current_action = Action::Pickup;
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
            if (board->well[index] >= 0) 
            {
                drag->is_dragging = true;
                drag->letter = board->well[index];
                drag->original_index = index;
                board->well[index] = -1;
                drag->position = mouse_pos;
            }
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
            if (letter == -1 || drag->letter < SPECIAL_COUNT) {
                board_drop_tile(board, x, y, drag->letter);
                board->well[drag->original_index] = dictionary_get_letter_or_special(&_dictionary);
                CheckResult result = board_check_words(board, &_dictionary, x, y);
                board_clear_words(board, x, y, result);
                g_game.move_count += 1;
                if (result == CHECK_RESULT_BOTH) {
                    g_game.word_count += 2;
                }
                else if (result != CHECK_RESULT_NONE) {
                    g_game.word_count += 1;
                }
                drop_success = true;
            }
        }

        if (drop_success == false) {
            _animation.letter = drag->letter;
            _animation.current_time = 0;
            _animation.total_time = .5f;
            _animation.start = drag->position;
            _animation.end = board_get_well_position(board, drag->original_index);
        }

        drag->is_dragging = false;
    }

    if (drag->is_dragging) {
        drag->position = mouse_pos;
    }

    _current_action = Action::None;
}

bool animation_update(Animation* anim) {
    
    if (anim->letter < 0) return false;

    anim->current_time += GetFrameTime();
    if (anim->current_time > anim->total_time) {
        return true;
    }
    float val = ease_out_cubic(anim->current_time / anim->total_time);
    anim->current = Vector2Add(anim->start, Vector2Scale(Vector2Subtract(anim->end, anim->start), val));
    return false;
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

    g_game.refresh_count = 5;

    _dictionary = dictionary_load("resources/text/en/words.txt");
    dictionary_load_distribution(&_dictionary, "resources/text/en/distribution.txt");

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

    // Init the game mode
    mode_init_calls[g_game.mode]();
}

// Gameplay Screen Update logic
void update_game_screen(void)
{
    if (_show_help) return;

    g_game.elapsed_time += GetFrameTime();

    input_update(&_drag_info);
    drag_update(&_drag_info, &_board);
    if (animation_update(&_animation) == true) {
        _board.well[_drag_info.original_index] = _animation.letter;
        _animation.letter = -1;
    }

    bool run_again = mode_update_calls[g_game.mode](&g_game);
    if (!run_again) {
        _finish_screen = 1;
    }
}

// Gameplay Screen Draw logic
void draw_game_screen(void)
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), WHITE);

    Rectangle button_rect = { .x = 500, .y = 0, .width = (float)GetScreenWidth() - 500 - 20,
    .height = (float)g_font_small.baseSize + 8 };

    float button_spacing = button_rect.height + 8;
    // Should just draw from bottom to top ...
    button_rect.y = GetScreenHeight() - 20 - 3 * button_spacing;
    board_draw(&_board, _layout.board_pos, _layout.well_pos);
    if (_drag_info.is_dragging) {
        letters_draw(&_letters, _drag_info.letter, _drag_info.position, 0.25f);
    }

    if (_animation.letter > 0) {
        letters_draw(&_letters, _animation.letter, _animation.current, 0.25f);
    }

    //if (GuiButton(Rectangle{ .x = 500, .y = 300, .width = 100, .height = 40 }, "Reset Board")) {
    //    board_reset(&_board);
    //}

    if (_show_help || g_game.refresh_count <= 0) GuiDisable();
    if (GuiButton(button_rect, TextFormat("Refresh (%d)", g_game.refresh_count))) {
        --g_game.refresh_count;
        board_reset_well(&_board);
    }
    if (!_show_help) GuiEnable();

    button_rect.y += button_spacing;

    if (GuiButton(button_rect, "Help")) {
        _show_help = true;
    }
    
    button_rect.y += button_spacing;

    if (GuiButton(button_rect, "Quit")) {
        _finish_screen = 2;
    }

    
    if (_show_help) GuiEnable();

    mode_draw_calls[g_game.mode](&g_game);

    if (_show_help) {
        GuiSetStyle(DEFAULT, TEXT_ALIGNMENT_VERTICAL, TEXT_ALIGN_TOP);   // WARNING: Word-wrap does not work as expected in case of no-top alignment
        GuiSetStyle(DEFAULT, TEXT_WRAP_MODE, TEXT_WRAP_WORD);            // WARNING: If wrap mode enabled, text editing is not supported
        Rectangle box = Rectangle{ .x = 40, .y = 40, 
            .width = (float)GetScreenWidth() - 80, .height = (float)GetScreenHeight() - 120 };
        GuiPanel(box, nullptr);
        GuiTextBox(box , _help_text, (int)strlen(_help_text), false);
        GuiSetStyle(DEFAULT, TEXT_WRAP_MODE, TEXT_WRAP_NONE);
        GuiSetStyle(DEFAULT, TEXT_ALIGNMENT_VERTICAL, TEXT_ALIGN_MIDDLE);

        Rectangle button = Rectangle{ .x = (float)GetScreenWidth() / 2.0f - 40, .y = box.y + box.height + 8,
        .width = 80, .height = 40 };
        _show_help = !GuiButton(button, "Close");
    }
}

// Gameplay Screen Unload logic
void unload_game_screen(void)
{
    letters_unload(&_letters);
    board_unload(&_board);
    dictionary_unload(&_dictionary);
}

// Gameplay Screen should finish?
int finish_game_screen(void)
{
    return _finish_screen;
}