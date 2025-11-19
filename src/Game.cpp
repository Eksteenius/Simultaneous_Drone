#include "Game.h"

Game::Game(std::shared_ptr<Camera2D> _game_camera, std::shared_ptr<Camera2D> _ui_camera)
{
	game_camera = _game_camera;
	ui_camera = _ui_camera;
}

Game::~Game()
{

}

bool Game::init(bool reset)
{
	SetWindowState(FLAG_WINDOW_MINIMIZED);

	/// UI
	//HideCursor();

	return true;
}

void Game::update()
{
	/// PHYSICS
	float dt = GetFrameTime();

	/// INPUT
	UpdateKeyToggles();

	/// WINDOW
	if (IsKeyPressed(KEY_ESCAPE))
	{
		//MinimizeWindow();
		CloseWindow();
	}

	/// CAMERA
	game_zoom *= 1 + GetMouseWheelMove() * scrollSpeed;
	handleZoom();
	float camera_move_speed = 500;
	if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
	{
		game_camera->offset = { game_camera->offset.x, game_camera->offset.y + camera_move_speed * dt };
	}
	if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
	{
		game_camera->offset = { game_camera->offset.x, game_camera->offset.y - camera_move_speed * dt };
	}
	if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
	{
		game_camera->offset = { game_camera->offset.x + camera_move_speed * dt, game_camera->offset.y };
	}
	if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
	{
		game_camera->offset = { game_camera->offset.x - camera_move_speed * dt , game_camera->offset.y };
	}

	game_mouse_position = { (GetMousePosition().x - game_camera->offset.x) / game_camera->zoom,
							(GetMousePosition().y - game_camera->offset.y) / game_camera->zoom };

	ui_mouse_position = { (GetMousePosition().x - ui_camera->offset.x) / ui_camera->zoom,
						  (GetMousePosition().y - ui_camera->offset.y) / ui_camera->zoom };

	/// UPDATE UI
}

void Game::render()
{
	DrawRectangle(0, 0, 1920, 1080, DARKGRAY);
}

void Game::render_ui()
{

	/// Custom Cursor
	DrawCircle(ui_mouse_position.x, ui_mouse_position.y, 4, RED);
}

void Game::UpdateKeyToggles()
{
	for (auto &pair : keyToggledMap)
	{
		if (IsKeyReleased(pair.first))
		{
			keyToggledMap[pair.first] = !pair.second;
		}
	}
}

std::unordered_map<int, bool> Game::keyToggledMap;

void Game::handleZoom()
{
	game_camera->zoom = ((float)GetMonitorHeight(GetCurrentMonitor()) / (float)screenHeight) * game_zoom;
}

float Game::randomFromRange(float min, float max)
{
	std::random_device rd; // obtain a random number from hardware
	std::mt19937 gen(rd()); // seed the generator
	std::uniform_real_distribution<> distr(min, max); // define the range

	return distr(gen);
}