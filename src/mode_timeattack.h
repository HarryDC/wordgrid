#include "raylib.h"
#include "raylib-extras.h"

#include "screens.h"

struct ModeTimeAttackParameters {
    float initial_time = 300;
    float words_to_increase = 5;
    float time_increase = 30;
};

struct ModeTimeAttackLayout {
    Vector2 clock_pos = Vector2{ 500, 20};
    Vector2 text_pos = Vector2{ 680, 20 };
};

struct ModeTimeAttack {
    float time_remaining;
    int next_increase;
    ModeTimeAttackParameters parameters;
    ModeTimeAttackLayout layout;
};

static ModeTimeAttack _mode_timeattack;

static void mode_timeattack_init() {
    _mode_timeattack.time_remaining = _mode_timeattack.parameters.initial_time;
    _mode_timeattack.next_increase = _mode_timeattack.parameters.words_to_increase;
}

static void mode_timeattack_draw(Game* game) {
    int minutes = (int)_mode_timeattack.time_remaining / 60;
    int seconds = (int)_mode_timeattack.time_remaining - minutes * 60;

    const char* text = TextFormat("%02d:%02d", minutes, seconds);

    DrawTextV(text, _mode_timeattack.layout.clock_pos, 64, BLACK);

    Vector2 pos = _mode_timeattack.layout.text_pos;
    text = TextFormat("Total Words: %d", game->word_count);
    DrawTextV(text, pos, 16, BLACK);
    pos.y += 32;
    text = TextFormat("Timebonus in %d words", _mode_timeattack.next_increase- game->word_count);
    DrawTextV(text, pos, 16, BLACK);
}

static bool mode_timeattack_update(Game* game) {
    _mode_timeattack.time_remaining -= GetFrameTime();
    if (_mode_timeattack.time_remaining < 0) {
        return false;
    }
    if (_mode_timeattack.next_increase < game->word_count) {
        _mode_timeattack.time_remaining += _mode_timeattack.parameters.time_increase;
        // TODO Play Sound
        _mode_timeattack.next_increase += _mode_timeattack.parameters.words_to_increase;
    }

    return true;
}