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
	game_camera->target = { ((float)GetMonitorWidth(GetCurrentMonitor()) / game_camera->zoom) / 2.0f, 
							((float)GetMonitorHeight(GetCurrentMonitor()) / game_camera->zoom) / 2.0f };

	/// CONTROLS
	key_camera_up = {KEY_UP, KEY_W};
	key_camera_down = { KEY_DOWN, KEY_S };
	key_camera_left = { KEY_LEFT, KEY_A };
	key_camera_right = { KEY_RIGHT, KEY_D };

	/// UI
	//HideCursor();

	/// makes grid fit in screen height
	//grid_rect_size = (float)screenHeight / (float)grid_root_size;

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
	if (utils::isKeyVectorDown(key_camera_up)) 
	{
		//game_offset = { game_offset.x, game_offset.y + camera_move_speed * dt }; 
		game_camera->target = { game_camera->target.x, game_camera->target.y - camera_move_speed / game_zoom * dt };
	} 
	if (utils::isKeyVectorDown(key_camera_down))
	{ 
		//game_offset = { game_offset.x, game_offset.y - camera_move_speed * dt }; 
		game_camera->target = { game_camera->target.x, game_camera->target.y + camera_move_speed / game_zoom * dt };
	} 
	if (utils::isKeyVectorDown(key_camera_left)) 
	{
		//game_offset = { game_offset.x + camera_move_speed * dt, game_offset.y }; 
		game_camera->target = { game_camera->target.x - camera_move_speed / game_zoom * dt, game_camera->target.y };
	} 
	if (utils::isKeyVectorDown(key_camera_right))
	{ 
		//game_offset = { game_offset.x - camera_move_speed * dt , game_offset.y }; 
		game_camera->target = { game_camera->target.x + camera_move_speed / game_zoom * dt , game_camera->target.y };
	} 
	
	game_zoom *= 1 + GetMouseWheelMove() * scrollSpeed; 
	handleZoom(game_camera, game_zoom);

	//ui_zoom *= 1 + GetMouseWheelMove() * scrollSpeed;
	//handleZoom(ui_camera, ui_zoom);

	game_mouse_position = { (GetMousePosition().x - game_camera->offset.x) / game_camera->zoom,
							(GetMousePosition().y - game_camera->offset.y) / game_camera->zoom };

	ui_mouse_position = { (GetMousePosition().x - ui_camera->offset.x) / ui_camera->zoom,
						  (GetMousePosition().y - ui_camera->offset.y) / ui_camera->zoom };

	world_mouse_position = GetScreenToWorld2D(GetMousePosition(), *game_camera);


	//if(IsKeyReleased(KEY_W))
	//{
	//	if (position_coords.y > 0) position_coords.y--;
	//	pathfinder->path_set = false;
	//}
	//else if (IsKeyReleased(KEY_S))
	//{
	//	if (position_coords.y < grid_root_size - 1) position_coords.y++;
	//	pathfinder->path_set = false;
	//}
	//else if (IsKeyReleased(KEY_A))
	//{
	//	if (position_coords.x > 0) position_coords.x--;
	//	pathfinder->path_set = false;
	//}
	//else if (IsKeyReleased(KEY_D))
	//{
	//	if (position_coords.x < grid_root_size - 1) position_coords.x++;
	//	pathfinder->path_set = false;
	//}

	if(!pathfinder->path_set)
	{
		pathfinder->setStartEndIndex(
			utils::coordsToIndex(position_coords, grid_root_size),
			utils::coordsToIndex(destination_coords, grid_root_size));
	}
	
	while (!pathfinder->pathing_complete)
	{
		pathfinder->AStar();
	}
}

void Game::render()
{
	DrawRectangle(0, 0, screenWidth, screenHeight, GRAY);

	DrawRectangle(0, 0, grid_root_size * grid_rect_size, grid_root_size * grid_rect_size, DARKGRAY);

	for(Cell& cell : cells)
	{
		DrawRectangleLines(
			(cell.i * grid_rect_size), (cell.j * grid_rect_size), grid_rect_size, grid_rect_size, WHITE);
	}

	for(std::reference_wrapper<Cell> cell : pathfinder->getLastSolvedPath())
	{
		DrawRectangle(cell.get().i * grid_rect_size + (grid_rect_size * 0.1f / 2), cell.get().j * grid_rect_size + (grid_rect_size * 0.1f / 2), 
			grid_rect_size * 0.9f, grid_rect_size * 0.9f, BLUE);
	}

	Vector2 hovered_cell = utils::globalToCoords(world_mouse_position, grid_rect_size);
	DrawRectangle(hovered_cell.x * grid_rect_size, hovered_cell.y * grid_rect_size, grid_rect_size, grid_rect_size, YELLOW);

	/// Custom Cursor
	//DrawCircle(game_mouse_position.x, game_mouse_position.y, 4, GREEN);
}

void Game::render_ui()
{
	DrawCircle(screenWidth / 2.f / ui_zoom, screenHeight / 2.f / ui_zoom, 2, WHITE);

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