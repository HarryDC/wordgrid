#include "raylib.h"

inline void DrawTextV(const char* text, Vector2 pos, int fontSize, Color color) {
    DrawText(text, pos.x, pos.y, fontSize, color);
}