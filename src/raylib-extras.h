#pragma once

#include "raylib.h"

extern Font g_default_font;

inline void DrawTextV(const char* text, Vector2 pos, Color color) {
    DrawTextEx(g_default_font, text, pos, (float)g_default_font.baseSize, 1.0f, color);
}

inline void FontSetDefaultFont(Font default_font)
{
    g_default_font = default_font;
}