#include "modes.h"
#include "raylib-extras.h"
#include "screens.h"

static ModeTimeAttack _mode_timeattack;

static Font _clock_font = { 0 };
constexpr int _codepoint_count = 11;
int _clock_codepoints[_codepoint_count] = { '0', '1', '2', '3', '4', '5', '6', '7',  '8', '9', ':' };

void mode_timeattack_init() {
    _mode_timeattack.time_remaining = _mode_timeattack.parameters.initial_time;
    _mode_timeattack.next_increase = _mode_timeattack.parameters.words_to_increase;

    _clock_font = LoadFontEx("resources/fredoka_medium.ttf", 96, _clock_codepoints, _codepoint_count);
}

void mode_timeattack_draw(Game* game) {
    int minutes = (int)_mode_timeattack.time_remaining / 60;
    int seconds = (int)_mode_timeattack.time_remaining - minutes * 60;

    const char* text = TextFormat("%02d:%02d", minutes, seconds);

    DrawTextEx(_clock_font, text, _mode_timeattack.layout.clock_pos, (float)_clock_font.baseSize, 1.0f, BLACK);

    Vector2 pos = _mode_timeattack.layout.text_pos;
    text = TextFormat("Total Words: %d", game->word_count);
    DrawTextDefaultV(text, pos, BLACK);
    pos.y += 32;
    text = TextFormat("%d more words for bonus time", _mode_timeattack.next_increase - game->word_count);
    DrawTextDefaultV(text, pos, BLACK);
}

bool mode_timeattack_update(Game* game) {
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

void mode_timeattack_unload() {

}

void mode_moveattack_init() {
    _mode_moveattack.available_moves = _mode_moveattack.parameters.initial_moves;
    _mode_moveattack.next_increase = _mode_moveattack.parameters.words_to_increase;
}

void mode_moveattack_draw(Game* game) {
    const int line_height = 32;
    Vector2 pos = _mode_moveattack.layout.text_pos;
    const char* text = TextFormat("Total Words: %d", game->word_count);
    DrawTextDefaultV(text, pos, BLACK);
    pos.y += line_height;
    text = TextFormat("More moves in %d words", _mode_moveattack.next_increase - game->word_count);
    DrawTextDefaultV(text, pos, BLACK);
    pos.y += line_height;
    text = TextFormat("You have %d moves left", _mode_moveattack.available_moves - game->move_count);
    DrawTextDefaultV(text, pos, BLACK);

}

bool mode_moveattack_update(Game* game) {
    if (_mode_moveattack.next_increase < game->word_count) {
        _mode_moveattack.available_moves += _mode_moveattack.parameters.move_increase;
        // TODO Play Sound
        _mode_moveattack.next_increase += _mode_moveattack.parameters.words_to_increase;
    }

    if (_mode_moveattack.available_moves - game->move_count <= 0) {
        return false;
    }

    return true;
}

void mode_moveattack_unload() {
    UnloadFont(_clock_font);
}
