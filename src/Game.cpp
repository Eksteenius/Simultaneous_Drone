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

	game_camera->offset = { (float)GetMonitorWidth(GetCurrentMonitor()) / 2.0f, (float)GetMonitorHeight(GetCurrentMonitor()) / 2.0f };

	/// UI
	//HideCursor();

	grid_box_size = screenHeight / grid_root_size;

	cells.reserve(grid_root_size * grid_root_size);
	for (int j = 0; j < grid_root_size; j++)
	{
		for (int i = 0; i < grid_root_size; i++)
		{
			cells.emplace_back(i, j);
		}
	}
	for (Cell& cell : cells)
	{
		cell.addNeighbors(cells, grid_root_size, grid_root_size);
	}


	pathfinder = std::make_shared<Pathfinder>(Pathfinder(cells));


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
	float camera_move_speed = 500; 
	if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) 
	{
		//game_offset = { game_offset.x, game_offset.y + camera_move_speed * dt }; 
		game_camera->target = { game_camera->target.x, game_camera->target.y - camera_move_speed * dt };
	} 
	if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) 
	{ 
		//game_offset = { game_offset.x, game_offset.y - camera_move_speed * dt }; 
		game_camera->target = { game_camera->target.x, game_camera->target.y + camera_move_speed * dt };
	} 
	if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) { 
		//game_offset = { game_offset.x + camera_move_speed * dt, game_offset.y }; 
		game_camera->target = { game_camera->target.x - camera_move_speed * dt, game_camera->target.y };
	} 
	if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) 
	{ 
		//game_offset = { game_offset.x - camera_move_speed * dt , game_offset.y }; 
		game_camera->target = { game_camera->target.x + camera_move_speed * dt , game_camera->target.y };
	} 
	
	game_zoom *= 1 + GetMouseWheelMove() * scrollSpeed; 
	handleZoom(game_camera, game_zoom);

	game_mouse_position = { (GetMousePosition().x - game_camera->offset.x) / game_camera->zoom,
							(GetMousePosition().y - game_camera->offset.y) / game_camera->zoom };

	ui_mouse_position = { (GetMousePosition().x - ui_camera->offset.x) / ui_camera->zoom,
						  (GetMousePosition().y - ui_camera->offset.y) / ui_camera->zoom };

	if(!pathfinder->path_set)
	{
		pathfinder->setStartEndIndex(0, cells.size() - 1);
	}
	
	while (!pathfinder->pathing_complete)
	{
		pathfinder->AStar();
	}
}

void Game::render()
{
	DrawRectangle(0, 0, 1920, 1080, DARKGRAY);

	for(Cell& cell : cells)
	{
		DrawRectangleLines(cell.i * grid_box_size, cell.j * grid_box_size, grid_box_size, grid_box_size, WHITE);
	}

	for(std::reference_wrapper<Cell> cell : pathfinder->getLastSolvedPath())
	{
		DrawRectangle(cell.get().i * grid_box_size + (grid_box_size * 0.1f / 2), cell.get().j * grid_box_size + (grid_box_size * 0.1f / 2), 
			grid_box_size * 0.9f, grid_box_size * 0.9f, BLUE);
	}
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

void Game::handleZoom(std::shared_ptr<Camera2D> camera, float zoom) 
{ 
	camera->zoom = ((float)GetMonitorHeight(GetCurrentMonitor()) / (float)screenHeight) * zoom; 
}

float Game::randomFromRange(float min, float max)
{
	std::random_device rd; // obtain a random number from hardware
	std::mt19937 gen(rd()); // seed the generator
	std::uniform_real_distribution<> distr(min, max); // define the range

	return distr(gen);
}

/// Quantum Cameras

//void Game::handleZoom(std::shared_ptr<Camera2D> camera, Vector2 offset, float zoom)
//{
//	float previous_zoom = camera->zoom;
//	Vector2 previous_offset = camera->offset;
//	camera->zoom = ((float)GetMonitorHeight(GetCurrentMonitor()) / (float)screenHeight) * zoom;
//	camera->offset.x = offset.x - previous_offset.x + (previous_zoom * screenWidth - camera->zoom * screenWidth + offset.x * (previous_zoom - camera->zoom)) / 2;
//	camera->offset.y = offset.y - previous_offset.y + (previous_zoom * screenHeight - camera->zoom * screenHeight + offset.y * (previous_zoom - camera->zoom)) / 2;
//
//}