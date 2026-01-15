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
	SetConfigFlags(FLAG_WINDOW_ALWAYS_RUN);

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

	/// BUTTONS
	btn_droning.bounds = Rectangle{ (float)screen_width - 300, 0 * (50 + 10), 300, 50 };
	btn_droning.text = "Activate Drones";
	btn_droning.text_size = 30;

	btn_destination.bounds = Rectangle{ (float)screen_width - 300, 1 * (50 + 10), 300, 50 };
	btn_destination.text = "Place Destination";
	btn_destination.text_size = 30;

	btn_obstacles.bounds = Rectangle{ (float)screen_width - 300, 2 * (50 + 10), 300, 50 };
	btn_obstacles.text = "Place Obstacles";
	btn_obstacles.text_size = 30;

	/// CELLS - setup for pathfinding
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
		updateKeyToggles();

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

		game_zoom *= 1 + GetMouseWheelMove() * scroll_speed;
		handleZoom(game_camera, game_zoom);

		//ui_zoom *= 1 + GetMouseWheelMove() * scrollSpeed;
		//handleZoom(ui_camera, ui_zoom);

		/// CURSOR
		//game_mouse_position = { (GetMousePosition().x - game_camera->offset.x) / game_camera->zoom,
		//						(GetMousePosition().y - game_camera->offset.y) / game_camera->zoom };

		ui_mouse_position = { (GetMousePosition().x - ui_camera->offset.x) / ui_camera->zoom,
							  (GetMousePosition().y - ui_camera->offset.y) / ui_camera->zoom };

		world_mouse_position = GetScreenToWorld2D(GetMousePosition(), *game_camera);

		/// BUTTONS
		btn_droning.update(ui_camera);
		btn_destination.update(ui_camera);
		btn_obstacles.update(ui_camera);

		/// Prevent clicking grid when clicking buttons
		if (btn_droning.clicked);
		else if (btn_destination.clicked) btn_obstacles.active = false;
		else if (btn_obstacles.clicked) btn_destination.active = false;

		/// GRID CLICK
		else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
		{
			if (utils::coordsWithinGrid(hovered_cell, grid_root_size))
			{
				int index = utils::coordsToIndex(hovered_cell, grid_root_size);
				if(clicked_cell == nullptr)
				{
					clicked_cell = std::make_shared<Cell>(cells[index]);
				}
				
				if (btn_obstacles.active && clicked_cell != nullptr) /// Place barriers
				{
					cells[index].barrier = !clicked_cell->barrier;
				}
			}
		}
		else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
		{
			if (utils::coordsWithinGrid(hovered_cell, grid_root_size) && 
				btn_destination.active) /// Place destination
			{
				destination_coords = hovered_cell;
				pathfinder->path_set = false;
			}

			if (clicked_cell != nullptr)
			{
				clicked_cell = nullptr;
				pathfinder->path_set = false;
			}
		}

		/// RAYCASTING

		/// PATHFINDING
		if (!pathfinder->path_set)
		{
			//Vector2 drone_pathing_coords = drone.getCurrentPathing();
			//if (drone.path_valid)
			//{
			//	pathfinder->setStartEndIndex(
			//		utils::coordsToIndex(drone_pathing_coords, grid_root_size),
			//		utils::coordsToIndex(destination_coords, grid_root_size));
			//}
			//else
			{
				pathfinder->setStartEndIndex(
					utils::coordsToIndex(utils::globalToCoords(drone.center(), grid_rect_size), grid_root_size),
					utils::coordsToIndex(destination_coords, grid_root_size));
			}


		}
		/// PATHFIND SOLVING
		{
			//int i = 0;
			while (!pathfinder->pathing_complete) //&& (pathfinder->search_iterations < 0 || i < pathfinder->search_iterations / 10))
			{
				pathfinder->AStar();
				//i++;
			}
		}

		/// DRONE MOVEMENT
		if (btn_droning.active)
		{
			drone.moveOnPath(pathfinder, grid_rect_size, dt);
			//drone.moveToPoint(utils::coordsToGlobal(destination_coords, grid_rect_size), dt);
		}
	}

	if(IsWindowMinimized())
	{
		paused = true;
	}
	else if (utils::isKeyVectorReleased(key_pause))
	{
		paused = !paused;
	}
	else if (GetKeyPressed() != 0 || utils::isAnyMouseButtonReleased() || GetMouseWheelMove() != 0.0f)
	{
		if (utils::isKeyVectorDown(key_pause) != true)
		{
			paused = false;
		}
	}
}

void Game::render()
{
	DrawRectangle(0, 0, screen_width, screen_height, GRAY);

	/// CELLS
	DrawRectangle(0, 0, grid_root_size * grid_rect_size, grid_root_size * grid_rect_size, DARKGRAY);
	for (Cell& cell : cells)
	{
		/// GRID
		DrawRectangleLines((cell.i * grid_rect_size), (cell.j * grid_rect_size), grid_rect_size, grid_rect_size, WHITE);
		//DrawRectangleLinesEx({ (cell.i * grid_rect_size), (cell.j * grid_rect_size), grid_rect_size, grid_rect_size }, 1, WHITE);

		/// BARRIER
		if (cell.barrier == true)
		{
			if (use_circular_nodes)
			{
				Vector2 cell_center = utils::center({ cell.i * grid_rect_size, cell.j * grid_rect_size }, grid_rect_size);
				DrawCircle(cell_center.x, cell_center.y, grid_rect_size / 2 - 2, ColorAlpha(BLACK, 0.25f));
				DrawRing(cell_center, grid_rect_size / 2 - 8, grid_rect_size / 2 - 2, 0, 360, 1, BLACK);
			}
			else
			{
				DrawRectangle((cell.i * grid_rect_size) + 1, (cell.j * grid_rect_size) + 1,grid_rect_size - 2, grid_rect_size - 2, ColorAlpha(BLACK, 0.25f));
				DrawRectangleLinesEx({ (cell.i * grid_rect_size) + 2, (cell.j * grid_rect_size) + 2, grid_rect_size - 4, grid_rect_size - 4 }, 8, BLACK);
			}
		}
	}

	/// SEARCHED CELLS
	if (pathfinder->pathing_complete)
	{
		for (Cell& cell : pathfinder->getClosedSet())
		{
			if ((pathfinder->pathing_solved && 
				std::find(pathfinder->getPath().begin(), pathfinder->getPath().end(), cell) == pathfinder->getPath().end()) ||
				((pathfinder->getLastSolvedPath().size() > 0) && 
					std::find(pathfinder->getLastSolvedPath().begin(), pathfinder->getLastSolvedPath().end(), cell) == pathfinder->getLastSolvedPath().end()))
			{
				if(use_circular_nodes)
				{
					Vector2 cell_center = utils::center({ cell.i * grid_rect_size, cell.j * grid_rect_size }, grid_rect_size);
					DrawCircle(cell_center.x, cell_center.y, grid_rect_size / 2 - 2, ColorAlpha(DARKBLUE, 0.25f));
					DrawRing(cell_center, grid_rect_size / 2 - 8, grid_rect_size / 2 - 2, 0, 360, 1, DARKBLUE);
				}
				else
				{
					DrawRectangle(cell.i * grid_rect_size + 1, cell.j * grid_rect_size + 1,
						grid_rect_size - 2, grid_rect_size - 2, ColorAlpha(DARKBLUE, 0.25f));
					DrawRectangleLinesEx({ cell.i * grid_rect_size + 2, cell.j * grid_rect_size + 2,
						grid_rect_size - 4, grid_rect_size - 4 }, 4, DARKBLUE);
				}
			}
		}
	}


	/// SOLVED PATH
	if (pathfinder->pathing_solved)
	{
		for (int i = 1; i < pathfinder->getPath().size(); i++)
		{
			const Cell& cell = pathfinder->getPath()[i];
			if (use_circular_nodes)
			{
				Vector2 cell_center = utils::center({ cell.i * grid_rect_size, cell.j * grid_rect_size }, grid_rect_size);
				DrawCircle(cell_center.x, cell_center.y, grid_rect_size / 2 - 2, ColorAlpha(Color(50, 255, 255, 255), 0.25f));
				DrawRing(cell_center, grid_rect_size / 2 - 8, grid_rect_size / 2 - 2, 0, 360, 1, Color(100, 255, 255, 255));
			}
			else
			{
				DrawRectangle(cell.i * grid_rect_size + 1, cell.j * grid_rect_size + 1,
					grid_rect_size - 2, grid_rect_size - 2, ColorAlpha(Color(50, 255, 255, 255), 0.25f));
				DrawRectangleLinesEx({ cell.i * grid_rect_size + 2, cell.j * grid_rect_size + 2,
					grid_rect_size - 4, grid_rect_size - 4 }, 4, Color(100, 255, 255, 255));
			}
		}
	}
	/// UNSOLVED PATH
	else if (pathfinder->getLastSolvedPath().size() > 0)
	{
		for (Cell& cell : pathfinder->getLastSolvedPath())
		{
			if (use_circular_nodes)
			{
				Vector2 cell_center = utils::center({ cell.i * grid_rect_size, cell.j * grid_rect_size }, grid_rect_size);
				DrawCircle(cell_center.x, cell_center.y, grid_rect_size / 2 - 2, ColorAlpha(RED, 0.25f));
				DrawRing(cell_center, grid_rect_size / 2 - 8, grid_rect_size / 2 - 2, 0, 360, 1, Color(255, 0, 0, 255));
			}
			else
			{
				DrawRectangle(cell.i * grid_rect_size + 1, cell.j * grid_rect_size + 1,
					grid_rect_size - 2, grid_rect_size - 2, ColorAlpha(RED, 0.25f));
				DrawRectangleLinesEx({ cell.i * grid_rect_size + 2, cell.j * grid_rect_size + 2,
					grid_rect_size - 4, grid_rect_size - 4 }, 4, Color(255, 0, 0, 255));
			}
		}
	}

	/// DESTINATION
	{
		if (use_circular_nodes)
		{
			Vector2 cell_center = utils::center(utils::coordsToGlobal(destination_coords, grid_rect_size), grid_rect_size);
			DrawCircle(cell_center.x, cell_center.y, grid_rect_size / 2 - 2, ColorAlpha(GREEN, 0.25f));
			DrawRing(cell_center, grid_rect_size / 2 - 8, grid_rect_size / 2 - 2, 0, 360, 1, Color(0, 255, 0, 255));
		}
		else
		{
			DrawRectangle(destination_coords.x * grid_rect_size + 1, destination_coords.y * grid_rect_size + 1,
				grid_rect_size - 2, grid_rect_size - 2, ColorAlpha(GREEN, 0.25f));
			DrawRectangleLinesEx({ destination_coords.x * grid_rect_size + 2, destination_coords.y * grid_rect_size + 2,
				grid_rect_size - 4, grid_rect_size - 4 }, 4, Color(0, 255, 0, 255));
		}
	}

	/// PROXIMITY PATHING
	Vector2 drone_pathing = utils::coordsToGlobal(drone.getCurrentPathing(), grid_rect_size);
	if(drone.proximity_distance >= 1)
	{
		DrawCircleLines(drone_pathing.x + grid_rect_size / 2, drone_pathing.y + grid_rect_size / 2,
			drone.proximity_distance, ColorAlpha(GREEN, 1));
	}
	
	/// SELECTED CELL
	hovered_cell = utils::globalToCoords(world_mouse_position, grid_rect_size);
	if (utils::coordsWithinGrid(hovered_cell, grid_root_size))
	{
		DrawRectangle(hovered_cell.x * grid_rect_size + 1, hovered_cell.y * grid_rect_size + 1,
			grid_rect_size - 2, grid_rect_size - 2, ColorAlpha(YELLOW, 0.5f));
		DrawRectangleLinesEx({ hovered_cell.x * grid_rect_size + 2, hovered_cell.y * grid_rect_size + 2,
			grid_rect_size - 4, grid_rect_size - 4 }, 4, YELLOW);
	}

	/// RAYCASTING
	std::unique_ptr<Vector2> collision = raycastCellCollision(Raycast(drone.center(), world_mouse_position, 50000));
	if(collision != nullptr)
	{
		DrawLine(drone.center().x, drone.center().y,
			world_mouse_position.x,
			world_mouse_position.y, RED);

		DrawCircle(collision->x, collision->y, 2, RED);
		DrawCircleLines(collision->x, collision->y, 10, YELLOW);
	}
	else
	{
		DrawLine(drone.center().x, drone.center().y,
			world_mouse_position.x,
			world_mouse_position.y, WHITE);
	}

	/// DRONE
	DrawRing(drone.center(), drone.size - 4, drone.size, 0, 360, 1, WHITE);
	DrawCircle(drone.center().x, drone.center().y, 4, WHITE);
	DrawPolyLinesEx(drone.center(), 3, drone.size, drone.rotation , 8, WHITE);
	DrawPolyLinesEx({ drone.center().x + (drone.size - drone.size / 2) * cosf(drone.rotation * PI / 180.0),
					  drone.center().y + (drone.size - drone.size / 2) * sinf(drone.rotation * PI / 180.0) }, 3, drone.size / 2, drone.rotation, 8, WHITE);

	/// CUSTOM CURSOR
	//DrawCircle(game_mouse_position.x, game_mouse_position.y, 4, GREEN);
}

void Game::renderUI()
{
	/// BUTTONS
	btn_droning.render();
	btn_destination.render();
	btn_obstacles.render();

	/// PAUSE
	if (paused)
	{
		DrawRectangle(0, 0, screen_width, 50, ColorAlpha(BLACK, 0.5f));
		DrawText("PAUSED: Press any key to continue...", 0, 0, 50, WHITE);
	}

	/// CENTER
	DrawCircle(screen_width / 2.f / ui_zoom, screen_height / 2.f / ui_zoom, 2, WHITE);

	/// CUSTOM CURSOR
	DrawCircle(ui_mouse_position.x, ui_mouse_position.y, 4, RED);

	DrawFPS(0, 0);
}

void Game::updateKeyToggles()
{
	for (auto &pair : key_toggled_map)
	{
		if (IsKeyReleased(pair.first))
		{
			key_toggled_map[pair.first] = !pair.second;
		}
	}
}
std::unordered_map<int, bool> Game::key_toggled_map;

void Game::handleZoom(std::shared_ptr<Camera2D> camera, float zoom) 
{ 
	camera->zoom = ((float)GetMonitorHeight(GetCurrentMonitor()) / (float)screen_height) * zoom; 
}

std::unique_ptr<Vector2> Game::raycastCellCollision(Raycast ray)
{
	Vector2 ray_start = { ray.start.x / grid_rect_size, ray.start.y / grid_rect_size };
	Vector2 ray_direction = Vector2Normalize(Vector2Subtract(ray.end, ray.start));

	Vector2 unit_step_size = { abs(1.0f / ray_direction.x), abs(1.0f / ray_direction.y) };
	Vector2 cell_coords_check = { floorf(ray_start.x), floorf(ray_start.y) };

	Vector2 unit_distance;
	Vector2 step;

	if (ray_direction.x < 0)
	{
		step.x = -1;
		unit_distance.x = (ray_start.x - cell_coords_check.x) * unit_step_size.x;
	}
	else
	{
		step.x = 1;
		unit_distance.x = ((cell_coords_check.x + 1) - ray_start.x) * unit_step_size.x;
	}

	if (ray_direction.y < 0)
	{
		step.y = -1;
		unit_distance.y = (ray_start.y - cell_coords_check.y) * unit_step_size.y;
	}
	else
	{
		step.y = 1;
		unit_distance.y = ((cell_coords_check.y + 1) - ray_start.y) * unit_step_size.y;
	}

	bool cell_found = false;
	float distance = 0;

	while (!cell_found && distance * grid_rect_size <= ray.length)
	{
		/// Walk
		if (unit_distance.x < unit_distance.y)
		{
			cell_coords_check.x += step.x;
			distance = unit_distance.x;
			unit_distance.x += unit_step_size.x;
		}
		else
		{
			cell_coords_check.y += step.y;
			distance = unit_distance.y;
			unit_distance.y += unit_step_size.y;
		}

		if (utils::coordsWithinGrid(cell_coords_check, grid_root_size) &&
			cells[utils::coordsToIndex(cell_coords_check, grid_root_size)].barrier)
		{
			cell_found = true;
		}
	}

	Vector2 intersection;
	if (cell_found)
	{
		intersection = Vector2Add(ray_start, Vector2Scale(ray_direction, distance));
		return std::make_unique<Vector2>(utils::coordsToGlobal(intersection, grid_rect_size));
	}
	return nullptr;
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


/// DRONE DIRECTION INDICATOR

//DrawLineEx({ drone.center().x + drone.size, drone.center().y },
//	{ drone.position.x + (drone.size / 2 - 6 * std::sqrtf(3)), drone.position.y + (drone.size /2 - 6 * std::sqrtf(3)) }, 4, WHITE);
//DrawLineEx({ drone.center().x + drone.size, drone.center().y },
//	{ drone.position.x + (drone.size / 2 - 6 * std::sqrtf(3)), drone.position.y + drone.size * 2 - (drone.size / 2 - 6 * std::sqrtf(3)) }, 4, WHITE);
//DrawPolyLinesEx(drone.center(), 3, drone.size, 0 , 8, WHITE);
//DrawLineEx({ drone.center().x + drone.size, drone.center().y }, { drone.center().x - drone.size, drone.center().y}, 4, WHITE);
//DrawLineEx({ drone.center().x, drone.center().y + drone.size }, { drone.center().x, drone.center().y - drone.size }, 4, WHITE);
//DrawLineEx({ drone.center().x + drone.size, drone.center().y }, { drone.center().x - (drone.size / 2 * 3) + drone.size, drone.center().y}, 4, WHITE);
//DrawLineEx({ drone.center().x, drone.center().y + (drone.size / 2 * 3) - drone.size },
//		   { drone.center().x, drone.center().y - (drone.size / 2 * 3) + drone.size }, 4, WHITE);
//DrawLineEx({ drone.center().x + ((drone.size / 2 * 3) - drone.size) / 2,
//	         drone.center().y + (drone.size / 2 * 3) - drone.size - ((drone.size / 2 * 3) - drone.size) / 4 },
//		   { drone.center().x + ((drone.size / 2 * 3) - drone.size) / 2,
//	         drone.center().y - (drone.size / 2 * 3) + drone.size + ((drone.size / 2 * 3) - drone.size) / 4 }, 4, WHITE);