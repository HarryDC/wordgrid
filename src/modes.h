#include "raylib.h"
#include "screens.h"

struct ModeTimeAttackParameters {
    float initial_time = 10;
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

void mode_timeattack_init();
void mode_timeattack_draw(Game* game);
bool mode_timeattack_update(Game* game);
void mode_timeattack_unload();

struct ModeMoveAttackParameters {
    int initial_moves = 50;
    int words_to_increase = 5;
    int move_increase = 10;
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

void mode_moveattack_init();
void mode_moveattack_draw(Game* game);
bool mode_moveattack_update(Game* game);
void mode_moveattack_unload();
