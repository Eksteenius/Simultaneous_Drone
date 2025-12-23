
//#define PLATFORM_WEB
#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#include "Game.h"

/// GLOBAL VARIABLES
//const int screenWidth = rl::GetMonitorWidth(rl::GetCurrentMonitor());
//const int screenHeight = rl::GetMonitorHeight(rl::GetCurrentMonitor());

void UpdateDrawFrame(Game& _game, std::shared_ptr<Camera2D> _game_camera, std::shared_ptr<Camera2D> _ui_camera); /// Update and Draw one frame

int main(void)
{
    SetTraceLogLevel(LOG_ALL);

    /// INITIALIZATION
    //InitWindow(Game::screenWidth, Game::screenHeight, "raylib [core] example - basic window");
    InitWindow(GetMonitorWidth(GetCurrentMonitor()), GetMonitorHeight(GetCurrentMonitor()), "");
    SetTargetFPS(60);   // Set our game to run at 60 frames-per-second

    //ToggleBorderlessWindowed();

    std::shared_ptr<Camera2D> game_camera = std::make_shared<Camera2D>();
    game_camera->target = { 0,0 };
    game_camera->rotation = 0.0f;
    game_camera->zoom = ((float)GetMonitorHeight(GetCurrentMonitor()) / (float)Game::screenHeight);
    game_camera->offset = { 0,0 };

    std::shared_ptr<Camera2D> ui_camera = std::make_shared<Camera2D>();
    ui_camera->target = { 0,0 };
    ui_camera->rotation = 0.0f;
    ui_camera->zoom = ((float)GetMonitorHeight(GetCurrentMonitor()) / (float)Game::screenHeight);
    ui_camera->offset = { 0,0 };

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else

    Game game(game_camera, ui_camera);

    if (!game.init(false)) /// Run the init function of the game class and check it all initialises ok
    {
        return 0;
    }

    /// MAIN GAME LOOP
    while (!WindowShouldClose()) /// Detect window close button or ESC key
    {
        UpdateDrawFrame(game, game_camera, ui_camera);
    }
#endif

    /// DE-INITIALIZATION
    CloseWindow(); /// Close window and OpenGL context

    return 0;
}

/// MODULE FUNCTIONS DEFINITION
void UpdateDrawFrame(Game& _game, std::shared_ptr<Camera2D> _game_camera, std::shared_ptr<Camera2D> _ui_camera)
{
    /// UPDATE
    _game.update();

    /// DRAW
    BeginDrawing();
    ClearBackground(BLACK);

    BeginMode2D(*_game_camera);
    _game.render();
    EndMode2D();

    BeginMode2D(*_ui_camera);
    _game.render_ui();
    EndMode2D();

    EndDrawing();
}