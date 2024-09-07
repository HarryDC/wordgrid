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
static int framesCounter = 0;
static int finishScreen = 0;

static Texture2D _letter_texture;
static Rectangle _letters[27] = { 0 };

void init_letter_tiles(const char* filename) {
    int letter_width = 256;
    int letter_height = 256;
    int letters_per_row = 4;
    int row_gap = 2;
    int letters_per_col = 7;
    int col_gap = 2;
    const char letters[27] = { 
        'y', 'r', 'k', 'd',
        'x', 'q', 'j', 'c',
        'w', 'p', 'i', 'b',
        'v', 'o', 'h', 'a',
        'u', 'n', 'g', ' ',
        't', 'm', 'f', 'l',
        's', 'z', 'e'
    };
    _letter_texture = LoadTexture(filename);
    for (int row = 0; row < letters_per_row; ++row) {
        for (int col = 0; col < letters_per_col; ++col) {
            Rectangle rect = {
                .x = (float)(col * (letter_width + col_gap)),
                .y = (float)(row * (letter_height + row_gap)),
                .width = (float)letter_width,
                .height = (float)letter_height,
            };
            int index = letters[row * letters_per_row + col] - 97 + 1;
            // Space will be less than 1, skip the last rect
            if (letters[row * letters_per_row + col] == ' ') index = 0;
            if (index < 27) _letters[index] = rect;
        }
    }
}

void draw_letter(char c, Vector2 pos, int size)
{
    Rectangle target = {
        .x = pos.x,
        .y = pos.y,
        .width = (float)size,
        .height = (float)size,
    };

    int index = c - 96;
    DrawTexturePro(_letter_texture, _letters[index], target, Vector2 { 0, 0 }, 0, WHITE);
}

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------

// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
    // TODO: Initialize GAMEPLAY screen variables here!
    framesCounter = 0;
    finishScreen = 0;

    init_letter_tiles("resources/solid_spritesheet.png");
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    // TODO: Update GAMEPLAY screen variables here!

    // Press enter or tap to change to ENDING screen
    if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
    {
        finishScreen = 1;
    }
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    // TODO: Draw GAMEPLAY screen here!
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), PURPLE);
    Vector2 pos = { 20, 10 };
    DrawTextEx(font, "GAMEPLAY SCREEN", pos, font.baseSize*3.0f, 4, MAROON);
    DrawText("PRESS ENTER or TAP to JUMP to ENDING SCREEN", 130, 220, 20, MAROON);
    Color c = WHITE;
    c.a = 128;
    draw_letter('c', Vector2 { 20, 20 }, 64);
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
    // TODO: Unload GAMEPLAY screen variables here!
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    return finishScreen;
}