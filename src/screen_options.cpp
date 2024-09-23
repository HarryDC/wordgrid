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

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int _frames_counter = 0;
static int _finish_screen = 0;

//----------------------------------------------------------------------------------
// Options Screen Functions Definition
//----------------------------------------------------------------------------------

// Options Screen Initialization logic
void init_options_screen(void)
{
    // TODO: Initialize OPTIONS screen variables here!
    _frames_counter = 0;
    _finish_screen = 0;
}

// Options Screen Update logic
void update_options_screen(void)
{
    // TODO: Update OPTIONS screen variables here!
}

// Options Screen Draw logic
void draw_options_screen(void)
{
    // TODO: Draw OPTIONS screen here!
}

// Options Screen Unload logic
void unload_options_screen(void)
{
    // TODO: Unload OPTIONS screen variables here!
}

// Options Screen should finish?
int finish_options_screen(void)
{
    return _finish_screen;
}