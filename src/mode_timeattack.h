#include "raylib.h"
#include "raylib-extras.h"

struct ModeTimeAttackParameters {
    float initial_time = 300;
    float words_to_increase = 5;
    float time_increase = 30;
};

struct ModeTimeAttackLayout {
    Vector2 clock_pos = Vector2{ 40, 40};
};

struct ModeTimeAttack {
    float time_remaining;
    ModeTimeAttackParameters parameters;
    ModeTimeAttackLayout layout;
};

static ModeTimeAttack _mode_timeattack;

static void mode_timeattack_init() {
    _mode_timeattack.time_remaining = _mode_timeattack.parameters.initial_time;
}

static void mode_timeattack_draw() {
    int minutes = (int)_mode_timeattack.time_remaining / 60;
    int seconds = (int)_mode_timeattack.time_remaining - minutes * 60;

    const char* text = TextFormat("%02d:%02d", minutes, seconds);

    DrawTextV(text, _mode_timeattack.layout.clock_pos, 64, BLACK);
}

static void mode_timeattack_update() {
    _mode_timeattack.time_remaining -= GetFrameTime();
    if (_mode_timeattack.time_remaining < 0) {
        // Mode to endgame 
    }
}