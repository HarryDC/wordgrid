#include "raylib.h"
#include "raylib-extras.h"

#include "screens.h"

struct ModeMoveAttackParameters {
    float initial_moves = 50;
    float words_to_increase = 5;
    float move_increase = 10;
};

struct ModeMoveAttackLayout {
    Vector2 text_pos = Vector2{ 500, 20 };
};

struct ModeMoveAttack {
    int available_moves;
    int next_increase;
    ModeMoveAttackParameters parameters;
    ModeMoveAttackLayout layout;
};

static ModeMoveAttack _mode_moveattack;

static void mode_moveattack_init() {
    _mode_moveattack.available_moves = _mode_moveattack.parameters.initial_moves;
    _mode_moveattack.next_increase = _mode_moveattack.parameters.words_to_increase;
}

static void mode_moveattack_draw(Game* game) {
    const int line_height = 32;
    Vector2 pos = _mode_moveattack.layout.text_pos;
    const char* text = TextFormat("Total Words: %d", game->word_count);
    DrawTextV(text, pos, BLACK);
    pos.y += line_height;
    text = TextFormat("More moves in %d words", _mode_moveattack.next_increase - game->word_count);
    DrawTextV(text, pos, BLACK);
    pos.y += line_height;
    text = TextFormat("You have %d moves left", _mode_moveattack.available_moves - game->move_count);
    DrawTextV(text, pos, BLACK);

}

static bool mode_moveattack_update(Game* game) {
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

static void mode_moveattack_unload() {
    UnloadFont(_clock_font);
}