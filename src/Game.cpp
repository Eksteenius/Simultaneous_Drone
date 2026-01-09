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
	key_pause = { KEY_TAB, KEY_ESCAPE };

	key_move_up = {KEY_UP, KEY_W};
	key_move_down = { KEY_DOWN, KEY_S };
	key_move_left = { KEY_LEFT, KEY_A };
	key_move_right = { KEY_RIGHT, KEY_D };

	/// UI
	//HideCursor();

	/// Makes grid fit in screen height
	//grid_rect_size = (float)screenHeight / (float)grid_root_size;

	/// Cell setup for pathfinding
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

	/// Drone initialization
	drone.size = (grid_rect_size / 2.f);

	return true;
}

void Game::update()
{
	if (!paused)
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
		if (utils::isKeyVectorDown(key_move_up))
		{
			//game_offset = { game_offset.x, game_offset.y + camera_move_speed * dt }; 
			game_camera->target = { game_camera->target.x, game_camera->target.y - camera_move_speed / game_zoom * dt };
		}
		if (utils::isKeyVectorDown(key_move_down))
		{
			//game_offset = { game_offset.x, game_offset.y - camera_move_speed * dt }; 
			game_camera->target = { game_camera->target.x, game_camera->target.y + camera_move_speed / game_zoom * dt };
		}
		if (utils::isKeyVectorDown(key_move_left))
		{
			//game_offset = { game_offset.x + camera_move_speed * dt, game_offset.y }; 
			game_camera->target = { game_camera->target.x - camera_move_speed / game_zoom * dt, game_camera->target.y };
		}
		if (utils::isKeyVectorDown(key_move_right))
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

		/// CLICK
		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
		{
			int index = utils::coordsToIndex(hovered_cell, grid_root_size);
			if (index >= 0 && index < cells.size())
			{
				cells[index].barrier = !cells[index].barrier;
				pathfinder->path_set = false;
			}
		}

		/// PATHFINDING

		if (!pathfinder->path_set)
		{
			Vector2* drone_tile = drone.getTilePathing();
			if (drone_tile != nullptr)
			{
				pathfinder->setStartEndIndex(
					utils::coordsToIndex(*drone_tile, grid_root_size),
					utils::coordsToIndex(destination_coords, grid_root_size));
			}
			else
			{
				pathfinder->setStartEndIndex(
					utils::coordsToIndex(utils::globalToCoords(drone.position, grid_rect_size), grid_root_size),
					utils::coordsToIndex(destination_coords, grid_root_size));
			}


		}

		while (!pathfinder->pathing_complete)
		{
			pathfinder->AStar();
		}

		/// DRONE MOVEMENT
		drone.moveOnPath(pathfinder, grid_rect_size, dt);
		//drone.moveToPoint(utils::coordsToGlobal(destination_coords, grid_rect_size), dt);
	}

	if(IsWindowState(FLAG_WINDOW_MINIMIZED))
	{
		paused = true;
	}
	if (utils::isKeyVectorReleased(key_pause))
	{
		paused = !paused;
	}
	//else if (GetKeyPressed() != 0 || utils::isAnyMouseButtonReleased() || GetMouseWheelMove() != 0.0f)
	//{
	//	if (utils::isKeyVectorDown(key_pause) != true)
	//	{
	//		paused = false;
	//	}
	//}
}

void Game::render()
{
	DrawRectangle(0, 0, screenWidth, screenHeight, GRAY);

	/// CELLS
	DrawRectangle(0, 0, grid_root_size * grid_rect_size, grid_root_size * grid_rect_size, DARKGRAY);
	for (Cell& cell : cells)
	{
		DrawRectangleLines((cell.i * grid_rect_size), (cell.j * grid_rect_size), grid_rect_size, grid_rect_size, WHITE);

		if (cell.barrier == true)
		{
			DrawRectangle((cell.i * grid_rect_size) + 1, (cell.j * grid_rect_size) + 1,
				grid_rect_size - 2, grid_rect_size - 2, BLACK);
		}
	}
	if (pathfinder->pathing_solved)
	{
		for (Cell& cell : pathfinder->getLastSolvedPath())
		{
			DrawRectangle(cell.i * grid_rect_size + 1, cell.j * grid_rect_size + 1,
				grid_rect_size - 2, grid_rect_size - 2, ColorAlpha(BLUE, 0.25f));
		}
	}
	else if (pathfinder->getLastSolvedPath().size() > 0)
	{
		for (Cell& cell : pathfinder->getLastSolvedPath())
		{
			DrawRectangle(cell.i * grid_rect_size + 1, cell.j * grid_rect_size + 1,
				grid_rect_size - 2, grid_rect_size - 2, ColorAlpha(RED, 0.25f));
		}
	}
	
	/// UI
	hovered_cell = utils::globalToCoords(world_mouse_position, grid_rect_size);
	DrawRectangle(hovered_cell.x * grid_rect_size + 1, hovered_cell.y * grid_rect_size + 1,
		grid_rect_size - 2, grid_rect_size - 2, YELLOW);

	/// DRONE
	DrawRing(drone.center(), drone.size - 4, drone.size, 0, 360, 1, WHITE);

	/// CUSTOM CURSOR
	//DrawCircle(game_mouse_position.x, game_mouse_position.y, 4, GREEN);
}

void Game::render_ui()
{
	DrawCircle(screenWidth / 2.f / ui_zoom, screenHeight / 2.f / ui_zoom, 2, WHITE);

	/// CUSTOM CURSOR
	DrawCircle(ui_mouse_position.x, ui_mouse_position.y, 4, RED);

	if (paused)
	{
		DrawRectangle(0, 0, screenWidth, 50, ColorAlpha(BLACK, 0.5f));
		DrawText("PAUSED: Press any key to continue...", 0, 0, 50, WHITE);
	}
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


/// QUANTUM CAMERAS

//void Game::handleZoom(std::shared_ptr<Camera2D> camera, Vector2 offset, float zoom)
//{
//	float previous_zoom = camera->zoom;
//	Vector2 previous_offset = camera->offset;
//	camera->zoom = ((float)GetMonitorHeight(GetCurrentMonitor()) / (float)screenHeight) * zoom;
//	camera->offset.x = offset.x - previous_offset.x + (previous_zoom * screenWidth - camera->zoom * screenWidth + offset.x * (previous_zoom - camera->zoom)) / 2;
//	camera->offset.y = offset.y - previous_offset.y + (previous_zoom * screenHeight - camera->zoom * screenHeight + offset.y * (previous_zoom - camera->zoom)) / 2;
//
//}