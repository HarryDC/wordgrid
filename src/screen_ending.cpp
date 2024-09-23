/*******************************************************************************************
*
*   WordGrid
*   Simple Word Puzzle Game
*   (C) Harald Scheirich 2024
*   WordGrid is is licensed under an unmodified zlib/libpng license see LICENSE
*
********************************************************************************************/

#include "raylib.h"
#include "screens.h"
#include "modes.h"
#include "raylib-extras.h"

#include <math.h>

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int _frames_counter = 0;
static int _finish_screen = 0;

//----------------------------------------------------------------------------------
// Ending Screen Functions Definition
//----------------------------------------------------------------------------------

// Ending Screen Initialization logic
void init_ending_screen(void)
{
    // TODO: Initialize ENDING screen variables here!
    _frames_counter = 0;
    _finish_screen = 0;
}

// Ending Screen Update logic
void update_ending_screen(void)
{
    // TODO: Update ENDING screen variables here!

    // Press enter or tap to return to TITLE screen
    if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
    {
        _finish_screen = 1;
    }
}

// Ending Screen Draw logic
void draw_ending_screen(void)
{
    float y = 20;
    float x = 20;
    DrawTextCenteredHorizontally(g_font_large, "GAME OVER", y, 1.0, DARKGREEN);
    y += g_font_large.baseSize + 12;

    int minutes = (int)(g_game.elapsed_time / 60);
    int seconds = (int)(g_game.elapsed_time - minutes * 60.0f);

    switch (g_game.mode) {
    case (MODE_TIMEATTACK):
    {
        const char* text;
        text = TextFormat("You managed to survive for %d minutes and %d seconds.",
            minutes, seconds);
        DrawTextDefault(text, x, y, BLACK);
        y += g_font_small.baseSize + 8;
        
        text = TextFormat("You completed %d words !", g_game.word_count);
        DrawTextDefault(text, x, y, BLACK);
        break;
    }
    case (MODE_MOVEATTACK):
    {
        const char* text;
        text = TextFormat("You played for %d minutes and %d seconds.",
            minutes, seconds);
        y += g_font_small.baseSize + 8;

        text = TextFormat("You completed %d words !", g_game.word_count);
        DrawTextDefault(text, x, y, BLACK);
        break;
    }
    }
    DrawTextCenteredHorizontally(g_font_small,
        "Press any key to return to the title screen", GetScreenHeight() / 4.0f * 3.0f, 1.0, BLACK);
}

// Ending Screen Unload logic
void unload_ending_screen(void)
{
    // TODO: Unload ENDING screen variables here!
}

// Ending Screen should finish?
int finish_ending_screen(void)
{
    return _finish_screen;
}