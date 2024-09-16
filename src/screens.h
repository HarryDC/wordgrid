/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Screens Functions Declarations (Init, Update, Draw, Unload)
*
*   Copyright (c) 2014-2022 Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

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
#define GRAY RAYWHITE

//----------------------------------------------------------------------------------
// Global Variables Declaration (shared by several modules)
//----------------------------------------------------------------------------------
extern GameScreen g_currentScreen;
extern Font g_font_small;

enum GameMode {
    MODE_TIMEATTACK,
    MODE_COUNT,
};

struct GameSettings {
    GameMode mode;
};

extern GameSettings g_game_settings;

struct Game {
    int word_count = 0;
    int trash_count = 0;
};


#ifdef __cplusplus
extern "C" {            // Prevents name mangling of functions
#endif

//----------------------------------------------------------------------------------
// Logo Screen Functions Declaration
//----------------------------------------------------------------------------------
void InitLogoScreen(void);
void UpdateLogoScreen(void);
void DrawLogoScreen(void);
void UnloadLogoScreen(void);
int FinishLogoScreen(void);

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
void InitOptionsScreen(void);
void UpdateOptionsScreen(void);
void DrawOptionsScreen(void);
void UnloadOptionsScreen(void);
int FinishOptionsScreen(void);

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
void InitEndingScreen(void);
void UpdateEndingScreen(void);
void DrawEndingScreen(void);
void UnloadEndingScreen(void);
int FinishEndingScreen(void);

#ifdef __cplusplus
}
#endif



#endif // SCREENS_H