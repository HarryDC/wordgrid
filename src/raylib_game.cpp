/*******************************************************************************************
*
*   raylib game template
*
*   <Game title>
*   <Game description>
*
*   This game has been created using raylib (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2021 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"

// Lets us include raygui from other places without issues
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#undef RAYGUI_IMPLEMENTATION

#include "screens.h"    // NOTE: Declares global (extern) variables and screens functions
#include "raylib-extras.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Shared Variables Definition (global)
// NOTE: Those variables are shared between modules through screens.h
//----------------------------------------------------------------------------------
GameScreen g_currentScreen = LOGO;
Font g_font_small = { 0 };
Font g_font_large = { 0 };
Font g_default_font = { 0 };

Game g_game{};

//----------------------------------------------------------------------------------
// Local Variables Definition (local to this module)
//----------------------------------------------------------------------------------
static const int screenWidth = 852;
static const int screenHeight = 393;

// Required variables to manage screen transitions (fade-in, fade-out)
static float transAlpha = 0.0f;
static bool onTransition = false;
static bool transFadeOut = false;
static int transFromScreen = -1;
static GameScreen transToScreen = UNKNOWN;

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
static void ChangeToScreen(int screen);     // Change to screen, no transition effect

static void TransitionToScreen(int screen); // Request transition to next screen
static void UpdateTransition(void);         // Update transition effect
static void DrawTransition(void);           // Draw transition effect (full-screen rectangle)

static void UpdateDrawFrame(void);          // Update and draw one frame

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //---------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "Wordgrid");

    InitAudioDevice();      // Initialize audio device

    // Load global data (assets that must be available in all screens, i.e. font)
    g_font_small = LoadFontEx("resources/fredoka_medium.ttf",24, nullptr, 0);
    g_default_font = g_font_small;

    g_font_large = LoadFontEx("resources/fredoka_medium.ttf", 96, nullptr, 0);

    GuiSetFont(g_font_small);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 24);
    

    // Setup and init first screen
    g_currentScreen = LOGO;
    init_logo_screen();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);       // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        UpdateDrawFrame();
    }
#endif

    // De-Initialization
    //--------------------------------------------------------------------------------------
    // Unload current screen data before closing
    switch (g_currentScreen)
    {
        case LOGO: unload_logo_screen(); break;
        case TITLE: unload_title_screen(); break;
        case GAMEPLAY: unload_game_screen(); break;
        case ENDING: unload_ending_screen(); break;
        default: break;
    }

    // Unload global data loaded
    UnloadFont(g_font_small);

    CloseAudioDevice();     // Close audio context

    CloseWindow();          // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//----------------------------------------------------------------------------------
// Module specific Functions Definition
//----------------------------------------------------------------------------------
// Change to next screen, no transition
static void ChangeToScreen(GameScreen screen)
{
    // Unload current screen
    switch (g_currentScreen)
    {
        case LOGO: unload_logo_screen(); break;
        case TITLE: unload_title_screen(); break;
        case GAMEPLAY: unload_game_screen(); break;
        case ENDING: unload_ending_screen(); break;
        default: break;
    }

    // Init next screen
    switch (screen)
    {
        case LOGO: init_logo_screen(); break;
        case TITLE: init_title_screen(); break;
        case GAMEPLAY: init_game_screen(); break;
        case ENDING: init_ending_screen(); break;
        default: break;
    }

    g_currentScreen = screen;
}

// Request transition to next screen
static void TransitionToScreen(GameScreen screen)
{
    onTransition = true;
    transFadeOut = false;
    transFromScreen = g_currentScreen;
    transToScreen = screen;
    transAlpha = 0.0f;
}

// Update transition effect (fade-in, fade-out)
static void UpdateTransition(void)
{
    if (!transFadeOut)
    {
        transAlpha += 0.05f;

        // NOTE: Due to float internal representation, condition jumps on 1.0f instead of 1.05f
        // For that reason we compare against 1.01f, to avoid last frame loading stop
        if (transAlpha > 1.01f)
        {
            transAlpha = 1.0f;

            // Unload current screen
            switch (transFromScreen)
            {
                case LOGO: unload_logo_screen(); break;
                case TITLE: unload_title_screen(); break;
                case OPTIONS: unload_options_screen(); break;
                case GAMEPLAY: unload_game_screen(); break;
                case ENDING: unload_ending_screen(); break;
                default: break;
            }

            // Load next screen
            switch (transToScreen)
            {
                case LOGO: init_logo_screen(); break;
                case TITLE: init_title_screen(); break;
                case GAMEPLAY: init_game_screen(); break;
                case ENDING: init_ending_screen(); break;
                default: break;
            }

            g_currentScreen = transToScreen;

            // Activate fade out effect to next loaded screen
            transFadeOut = true;
        }
    }
    else  // Transition fade out logic
    {
        transAlpha -= 0.02f;

        if (transAlpha < -0.01f)
        {
            transAlpha = 0.0f;
            transFadeOut = false;
            onTransition = false;
            transFromScreen = -1;
            transToScreen = UNKNOWN;
        }
    }
}

// Draw transition effect (full-screen rectangle)
static void DrawTransition(void)
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, transAlpha));
}

// Update and draw game frame
static void UpdateDrawFrame(void)
{
    // Update
    //----------------------------------------------------------------------------------
    //UpdateMusicStream(music);       // NOTE: Music keeps playing between screens

    if (!onTransition)
    {
        switch(g_currentScreen)
        {
            case LOGO:
            {
                update_logo_screen();

                if (finish_logo_screen()) TransitionToScreen(TITLE);

            } break;
            case TITLE:
            {
                update_title_screen();

                if (finish_title_screen() == 1) TransitionToScreen(OPTIONS);
                else if (finish_title_screen() == 2) TransitionToScreen(GAMEPLAY);

            } break;
            case OPTIONS:
            {
                update_options_screen();

                if (finish_options_screen()) TransitionToScreen(TITLE);

            } break;
            case GAMEPLAY:
            {
                update_game_screen();

                if (finish_game_screen() == 1) TransitionToScreen(ENDING);
                else if (finish_game_screen() == 2) TransitionToScreen(TITLE);

            } break;
            case ENDING:
            {
                update_ending_screen();

                if (finish_ending_screen() == 1) TransitionToScreen(TITLE);

            } break;
            default: break;
        }
    }
    else UpdateTransition();    // Update transition (fade-in, fade-out)
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

        ClearBackground(RAYWHITE);

        switch(g_currentScreen)
        {
            case LOGO: draw_logo_screen(); break;
            case TITLE: draw_title_screen(); break;
            case OPTIONS: draw_options_screen(); break;
            case GAMEPLAY: draw_game_screen(); break;
            case ENDING: draw_ending_screen(); break;
            default: break;
        }

        // Draw full screen rectangle in front of everything
        if (onTransition) DrawTransition();

        //DrawFPS(10, 10);

    EndDrawing();
    //----------------------------------------------------------------------------------
}
