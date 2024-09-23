/*******************************************************************************************
*
*   WordGrid
*   Simple Word Puzzle Game
*   (C) Harald Scheirich 2024
*   WordGrid is is licensed under an unmodified zlib/libpng license see LICENSE
*
********************************************************************************************/

#pragma once

#include "raylib.h"

extern Font g_default_font;

inline void DrawTextDefault(const char* text, float x, float y, Color color) {
    DrawTextEx(g_default_font, text, Vector2(x, y), (float)g_default_font.baseSize, 1.0f, color);
}

inline void DrawTextDefaultV(const char* text, Vector2 pos, Color color) {
    DrawTextEx(g_default_font, text, pos, (float)g_default_font.baseSize, 1.0f, color);
}

inline void FontSetDefaultFont(Font default_font)
{
    g_default_font = default_font;
}

// Utility function to draw a text line that's centered horizontally on the screen
inline void DrawTextCenteredHorizontally(Font font, const char* text, float y, float spacing, Color color)
{
    Vector2 pos = MeasureTextEx(font, text, (float)font.baseSize * 1.0f, 1.0f);
    pos.y = y;
    pos.x = (GetScreenWidth() - pos.x) / 2.0f;
    DrawTextEx(font, text, pos, (float)font.baseSize, 1.0f, color);
}