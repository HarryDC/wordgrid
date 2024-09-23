/*******************************************************************************************
*
*   WordGrid
*   Simple Word Puzzle Game
*   (C) Harald Scheirich 2024
*   WordGrid is is licensed under an unmodified zlib/libpng license see LICENSE
*
********************************************************************************************/

#ifndef SCREENS_H
#define SCREENS_H

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum GameScreen { UNKNOWN = -1, LOGO = 0, TITLE, OPTIONS, GAMEPLAY, ENDING } GameScreen;

//----------------------------------------------------------------------------------
// Colors 
//----------------------------------------------------------------------------------
// https://coolors.co/ed6a5a-ffd07b-fdb833-296eb4-1789fc
//<palette>
//<color name = "Bittersweet" hex = "ed6a5a" r = "237" g = "106" b = "90" / >
//<color name = "Jasmine" hex = "ffd07b" r = "255" g = "208" b = "123" / >
//<color name = "Xanthous" hex = "fdb833" r = "253" g = "184" b = "51" / >
//<color name = "Azul" hex = "296eb4" r = "41" g = "110" b = "180" / >
//<color name = "Bleu de France" hex = "1789fc" r = "23" g = "137" b = "252" / >
//< / palette>

#define BITTERSWEET = Color{237, 106, 90, 255};
#define JASMINE = Color{255, 208, 123, 255};
#define XANTHOUS = Color{253, 184, 51, 255};
#define AZUL = Color{41, 110, 180, 255};
#define BLEU = Color{23, 137, 252, 255};
#define MY_GRAY RAYWHITE

//----------------------------------------------------------------------------------
// Global Variables Declaration (shared by several modules)
//----------------------------------------------------------------------------------
extern GameScreen g_currentScreen;
extern Font g_font_small;
extern Font g_font_large;

enum GameMode {
    MODE_NONE = -1,
    MODE_TIMEATTACK,
    MODE_MOVEATTACK,
    MODE_COUNT,
};

struct Game {
    GameMode mode;
    int word_count = 0;
    int trash_count = 0;
    int move_count = 0;
    int refresh_count = 0;
    float elapsed_time = 0;
};

extern Game g_game;

#ifdef __cplusplus
extern "C" {            // Prevents name mangling of functions
#endif

//----------------------------------------------------------------------------------
// Logo Screen Functions Declaration
//----------------------------------------------------------------------------------
void init_logo_screen(void);
void update_logo_screen(void);
void draw_logo_screen(void);
void unload_logo_screen(void);
int finish_logo_screen(void);

//----------------------------------------------------------------------------------
// Title Screen Functions Declaration
//----------------------------------------------------------------------------------
void init_title_screen(void);
void update_title_screen(void);
void draw_title_screen(void);
void unload_title_screen(void);
int finish_title_screen(void);

//----------------------------------------------------------------------------------
// Options Screen Functions Declaration
//----------------------------------------------------------------------------------
void init_options_screen(void);
void update_options_screen(void);
void draw_options_screen(void);
void unload_options_screen(void);
int finish_options_screen(void);

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Declaration
//----------------------------------------------------------------------------------
void init_game_screen(void);
void update_game_screen(void);
void draw_game_screen(void);
void unload_game_screen(void);
int finish_game_screen(void);

//----------------------------------------------------------------------------------
// Ending Screen Functions Declaration
//----------------------------------------------------------------------------------
void init_ending_screen(void);
void update_ending_screen(void);
void draw_ending_screen(void);
void unload_ending_screen(void);
int finish_ending_screen(void);

#ifdef __cplusplus
}
#endif



#endif // SCREENS_H