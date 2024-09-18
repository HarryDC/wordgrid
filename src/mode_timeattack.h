#include "raylib.h"
#include "raylib-extras.h"

#include "screens.h"

struct ModeTimeAttackParameters {
    float initial_time = 300;
    int words_to_increase = 5;
    float time_increase = 30;
};

struct ModeTimeAttackLayout {
    Vector2 clock_pos = Vector2{ 560, 100};
    Vector2 text_pos = Vector2{ 500, 20 };
};

struct ModeTimeAttack {
    float time_remaining;
    int next_increase;
    ModeTimeAttackParameters parameters;
    ModeTimeAttackLayout layout;
};

static ModeTimeAttack _mode_timeattack;


static Font _clock_font = { 0 };
constexpr int _codepoint_count = 11;
int _clock_codepoints[_codepoint_count] = { '0', '1', '2', '3', '4', '5', '6', '7',  '8', '9', ':'};

static void mode_timeattack_init() {
    _mode_timeattack.time_remaining = _mode_timeattack.parameters.initial_time;
    _mode_timeattack.next_increase = _mode_timeattack.parameters.words_to_increase;

    _clock_font = LoadFontEx("resources/fredoka_medium.ttf", 96, _clock_codepoints, _codepoint_count);
}

static void mode_timeattack_draw(Game* game) {
    int minutes = (int)_mode_timeattack.time_remaining / 60;
    int seconds = (int)_mode_timeattack.time_remaining - minutes * 60;

    const char* text = TextFormat("%02d:%02d", minutes, seconds);

    DrawTextEx(_clock_font, text, _mode_timeattack.layout.clock_pos, (float)_clock_font.baseSize, 1.0f, BLACK);

    Vector2 pos = _mode_timeattack.layout.text_pos;
    text = TextFormat("Total Words: %d", game->word_count);
    DrawTextV(text, pos, BLACK);
    pos.y += 32;
    text = TextFormat("%d more words for bonus time", _mode_timeattack.next_increase- game->word_count);
    DrawTextV(text, pos, BLACK);
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

static void mode_timeattack_unload() {

}