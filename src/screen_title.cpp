/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Title Screen Functions Definitions (Init, Update, Draw, Unload)
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
#include "raygui.h"
#include "screens.h"

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
    // TODO: Draw TITLE screen here!
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), GRAY);
    Vector2 pos = { 20, 10 };
    DrawTextEx(g_font_small, "WORD GRID", pos, g_font_small.baseSize*3.0f, 4, DARKGREEN);

    float y = GetScreenHeight() / 3.0f;
    float x = GetScreenWidth() / 2.0 - 100.0;

    if (GuiButton(Rectangle{ .x = x, .y = y, .width = 200, .height = 60 }, "Time Attack")) 
    {
        g_game_settings.mode = MODE_TIMEATTACK;
        _finish_screen = 2;
    };
    if (GuiButton(Rectangle{ .x = x, .y = y + 100, .width = 200, .height = 60 }, "Count Attack")) 
    {
        ///
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