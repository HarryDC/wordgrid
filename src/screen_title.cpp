/*******************************************************************************************
*
*   WordGrid
*   Simple Word Puzzle Game
*   (C) Harald Scheirich 2024
*   WordGrid is is licensed under an unmodified zlib/libpng license see LICENSE
*
********************************************************************************************/

#include "raylib.h"
#include "raygui.h"
#include "screens.h"
#include "raylib-extras.h"

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int _frames_counter = 0;
static int _finish_screen = 0;

//----------------------------------------------------------------------------------
// Title Screen Functions Definition
//----------------------------------------------------------------------------------

// Title Screen Initialization logic
void init_title_screen(void)
{
    // TODO: Initialize TITLE screen variables here!
    _frames_counter = 0;
    _finish_screen = 0;

    g_game = Game{};
}

enum AnchorType {
    ANCHOR_NONE,
    ANCHOR_CENTER_H = 0x1,
    ANCHOR_CENTER_V = 0x1 << 1,
    ANCHOR_TOP = 0x1 << 2,
    ANCHOR_BOTTOM = 0x1 << 3,
    ANCHOR_LEFT = 0x1 << 4,
    ANCHOR_RIGHT = 0x1 << 5
};

static const char* body_text = "Choose your letters wisely and keep going as long as you can !";

// Title Screen Update logic
void update_title_screen(void)
{
    // TODO: Update TITLE screen variables here!

    // Press enter or tap to change to GAMEPLAY screen
    if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
    {
        //finishScreen = 1;   // OPTIONS
        _finish_screen = 2;   // GAMEPLAY
    }
}

// Title Screen Draw logic
void draw_title_screen(void)
{
    float y = 20;
    DrawTextCenteredHorizontally(g_font_large, "WORDGRID", y, 1.0, DARKGREEN);
    y += g_font_large.baseSize + 12;
    DrawTextCenteredHorizontally(g_font_small, body_text, y, 1.0, BLACK);

    y = GetScreenHeight() / 2.0f;
    float x = GetScreenWidth() / 4.0f;

    if (GuiButton(Rectangle{ .x = x - 100, .y = y, .width = 200, .height = 60 }, "Time Attack")) 
    {
        g_game.mode = MODE_TIMEATTACK;
        _finish_screen = 2;
    };
    if (GuiButton(Rectangle{ .x = x * 3 - 100, .y = y, .width = 200, .height = 60 }, "Move Attack")) 
    {
        g_game.mode = MODE_MOVEATTACK;
        _finish_screen = 2;
    };
}

// Title Screen Unload logic
void unload_title_screen(void)
{
    // TODO: Unload TITLE screen variables here!
}

// Title Screen should finish?
int finish_title_screen(void)
{
    return _finish_screen;
}