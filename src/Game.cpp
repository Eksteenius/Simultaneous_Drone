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
	SetWindowState(FLAG_WINDOW_MAXIMIZED);

	monitor_width = (float)GetMonitorWidth(GetCurrentMonitor());
	monitor_height = (float)GetMonitorHeight(GetCurrentMonitor());

	game_camera->offset = { monitor_width / 2.0f, monitor_height / 2.0f };
	game_camera->target = { (monitor_width / game_camera->zoom) / 2.0f, 
							(monitor_height / game_camera->zoom) / 2.0f };

	//game_camera->offset = { (float)GetScreenWidth() / 2.0f, (float)GetScreenHeight() / 2.0f };
	//game_camera->target = { ((float)GetScreenWidth() / game_camera->zoom) / 2.0f,
	//						((float)GetScreenHeight() / game_camera->zoom) / 2.0f };

	SetWindowState(FLAG_WINDOW_MINIMIZED);
	SetConfigFlags(FLAG_WINDOW_ALWAYS_RUN);

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

	for (int j = 0; j < grid_root_size; j++)
	{
		for (int i = 0; i < grid_root_size; i++)
		{
			obstacles.emplace_back(0);
		}
	}

	pathfinder = std::make_shared<Pathfinder>(Pathfinder(cells));

	/// Drone initialization
	drones.emplace_back();
	drones.emplace_back();
	for (Drone& drone : drones)
	{
		drone.size = (grid_rect_size / 2.f);
	}
	drones[1].position = Vector2(100, 800);
	

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

		/// DRONE INPUTS
		if (IsKeyReleased(KEY_EQUAL))
		{
			for (Drone& drone : drones)
			{
				drone.range *= 1.1f;
			}
		}
		else if (IsKeyReleased(KEY_MINUS))
		{
			for (Drone& drone : drones)
			{
				drone.range /= 1.1f;
			}
		}

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

				if (clicked_obstacle == nullptr)
				{
					clicked_obstacle = std::make_shared<float>(obstacles[index]);
				}

				if (btn_obstacles.active && clicked_obstacle != nullptr) /// Place barriers
				{
					if (*clicked_obstacle == 0)
					{
						obstacles[index] = 1;
					}
					else
					{
						obstacles[index] = 0;
					}
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

			if (clicked_obstacle != nullptr)
			{
				clicked_obstacle = nullptr;
			}
		}

		/// RAYCASTING
		for (Drone& drone : drones)
		{
			float fov_ray_count = drone.rayCount(grid_rect_size);
			if (drone.raycasts.size() < fov_ray_count)
			{
				while (drone.raycasts.size() < fov_ray_count)
				{
					drone.raycasts.push_back(Raycast(drone.center(), world_mouse_position));
				}
			}
			for (int i = 0; i < fov_ray_count; i++)
			{
				drone.raycasts[i].start = drone.center();
				drone.raycasts[i].end = Vector2Add(drone.center(),
					Vector2Scale(utils::unitVectorFromAngle(drone.rotation + (drone.fov / 2.f) - (drone.fov / (fov_ray_count - 1)) * i), drone.range));

				raycastGridCollision(drone.raycasts[i]);

				if (drone.raycasts[i].collided && cells[drone.raycasts[i].collider_index].state != Cell::BLOCKED)
				{
					cells[drone.raycasts[i].collider_index].state = Cell::BLOCKED;
					pathfinder->path_set = false;
				}
			}
		}

		/// PATHFINDING
		for (Drone& drone : drones)
		{
			if (!pathfinder->path_set)
			{
				pathfinder->setStartEndIndex(
					utils::coordsToIndex(utils::globalToCoords(drone.center(), grid_rect_size), grid_root_size),
					utils::coordsToIndex(destination_coords, grid_root_size));
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
	/// Renders a gray rect to show how large the screen would be without any zoom other than resolution scaling
	DrawRectangle(0, 0, screen_width, screen_height, GRAY); 

	/// CELLS
	DrawRectangle(0, 0, grid_root_size * grid_rect_size, grid_root_size * grid_rect_size, DARKGRAY);
	for (const Cell& cell : cells)
	{
		if (utils::boxOverlapBox(
			std::pair(game_camera->target.x - (screen_width / game_zoom) / 2.f,
				game_camera->target.y - (screen_height / game_zoom) / 2.f),
			monitor_width / game_camera->zoom, monitor_height / game_camera->zoom,
			std::pair((cell.i * grid_rect_size), (cell.j * grid_rect_size)),
			grid_rect_size, grid_rect_size))
		{

			/// GRID
			DrawRectangleLines((cell.i * grid_rect_size), (cell.j * grid_rect_size), grid_rect_size, grid_rect_size, WHITE);
			//DrawRectangleLinesEx({ (cell.i * grid_rect_size), (cell.j * grid_rect_size), grid_rect_size, grid_rect_size }, 1, WHITE);

			/// BARRIER
			if (cell.state == Cell::BLOCKED)
			{
				Vector2 cell_center = utils::center({ cell.i * grid_rect_size, cell.j * grid_rect_size }, grid_rect_size);
				drawGridNode(cell_center, grid_rect_size / 2, BLACK, ColorAlpha(BLACK, 0.25f));
			}
		}
	}

	/// OBSTACLES
	for (int i = 0; i < obstacles.size(); i++)
	{
		if (obstacles[i] > 0)
		{
			Vector2 coords = utils::indexToCoords(i, grid_root_size);

			drawGridSquare(coords, grid_rect_size, BLACK, ColorAlpha(BLACK, 0.25f));
		}
	}

	/// SEARCHED CELLS
	if (pathfinder->pathing_complete)
	{
		for (const Cell& cell : pathfinder->getClosedSet())
		{
			if (utils::boxOverlapBox(
				std::pair(game_camera->target.x - (screen_width / game_zoom) / 2.f,
					game_camera->target.y - (screen_height / game_zoom) / 2.f),
				monitor_width / game_camera->zoom, monitor_height / game_camera->zoom,
				std::pair((cell.i * grid_rect_size), (cell.j * grid_rect_size)),
				grid_rect_size, grid_rect_size))
			{

				if ((pathfinder->pathing_solved &&
					std::find(pathfinder->getPath().begin(), pathfinder->getPath().end(), cell) == pathfinder->getPath().end()) ||
					((pathfinder->getLastSolvedPath().size() > 0) &&
						std::find(pathfinder->getLastSolvedPath().begin(), pathfinder->getLastSolvedPath().end(), cell) == pathfinder->getLastSolvedPath().end()))
				{
					Vector2 cell_center = utils::center({ cell.i * grid_rect_size, cell.j * grid_rect_size }, grid_rect_size);
					drawGridNode(cell_center, grid_rect_size / 2, DARKBLUE, ColorAlpha(DARKBLUE, 0.25f));
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

			if (utils::boxOverlapBox(
				std::pair(game_camera->target.x - (screen_width / game_zoom) / 2.f,
					game_camera->target.y - (screen_height / game_zoom) / 2.f),
				monitor_width / game_camera->zoom, monitor_height / game_camera->zoom,
				std::pair((cell.i * grid_rect_size), (cell.j * grid_rect_size)),
				grid_rect_size, grid_rect_size))
			{

				Vector2 cell_center = utils::center({ cell.i * grid_rect_size, cell.j * grid_rect_size }, grid_rect_size);
				drawGridNode(cell_center, grid_rect_size / 2, Color(100, 255, 255, 255), ColorAlpha(Color(50, 255, 255, 255), 0.25f));
			}
		}
	}
	/// UNSOLVED PATH
	else if (pathfinder->getLastSolvedPath().size() > 0)
	{
		for (const Cell& cell : pathfinder->getLastSolvedPath())
		{
			if (utils::boxOverlapBox(
				std::pair(game_camera->target.x - (screen_width / game_zoom) / 2.f,
					game_camera->target.y - (screen_height / game_zoom) / 2.f),
				monitor_width / game_camera->zoom, monitor_height / game_camera->zoom,
				std::pair((cell.i * grid_rect_size), (cell.j * grid_rect_size)),
				grid_rect_size, grid_rect_size))
			{
				Vector2 cell_center = utils::center({ cell.i * grid_rect_size, cell.j * grid_rect_size }, grid_rect_size);
				drawGridNode(cell_center, grid_rect_size / 2, Color(255, 0, 0, 255), ColorAlpha(RED, 0.25f));
			}
		}
	}

	/// DESTINATION
	{
		Vector2 cell_center = utils::center(utils::coordsToGlobal(destination_coords, grid_rect_size), grid_rect_size);
		drawGridNode(cell_center, grid_rect_size / 2, Color(0, 255, 0, 255), ColorAlpha(GREEN, 0.25f));
	}

	/// PROXIMITY PATHING - Renders a small green outline to show the proximity distance the drone needs to be to the destination cell
	for (Drone& drone : drones)
	{
		Vector2 proximity = utils::coordsToGlobal(drone.getCurrentPathing(), grid_rect_size);
		if (drone.proximity_distance >= 1)
		{
			DrawCircleLines(proximity.x + grid_rect_size / 2, proximity.y + grid_rect_size / 2,
				drone.proximity_distance, ColorAlpha(GREEN, 1));
		}
	}
	
	/// SELECTED CELL
	hovered_cell = utils::globalToCoords(world_mouse_position, grid_rect_size);
	if (utils::coordsWithinGrid(hovered_cell, grid_root_size))
	{
		drawGridSquare(hovered_cell, grid_rect_size, YELLOW, ColorAlpha(YELLOW, 0.5f));
	}

	/// RAYCASTING
	for (Drone& drone : drones)
	{
		float fov_ray_count = drone.rayCount(grid_rect_size);

		DrawCircleSector(drone.center(), drone.range, drone.rotation - drone.fov / 2, drone.rotation + drone.fov / 2, fov_ray_count - 1, ColorAlpha(RED, 0.5f));

		if (fov_ray_count > 0 && drone.raycasts.size() > 0 && drone.raycasts.size() >= fov_ray_count)
		{
			//for (const Raycast& ray : drone.raycasts)
			for (int i = 0; i < fov_ray_count; i++)
			{
				if (drone.raycasts[i].collided) /// Render the fov rays that collided an obstacle
				{
					DrawLine(drone.raycasts[i].start.x, drone.raycasts[i].start.y, drone.raycasts[i].end.x, drone.raycasts[i].end.y, RED);

					DrawCircle(drone.raycasts[i].collision.x, drone.raycasts[i].collision.y, 2, RED); /// red outline around obstacle node
					DrawCircleLines(drone.raycasts[i].collision.x, drone.raycasts[i].collision.y, 10, YELLOW); /// yellow circle at point of collision

					Cell& cell_collided = cells[drone.raycasts[i].collider_index];
					Vector2 cell_center = utils::center(utils::coordsToGlobal(cell_collided.i, cell_collided.j, grid_rect_size), grid_rect_size);
					DrawCircleLines(cell_center.x, cell_center.y, grid_rect_size / 2 - 2, RED);
				}
				else /// Render the fov rays that did not collide with an obstacle
				{
					DrawLine(drone.raycasts[i].start.x, drone.raycasts[i].start.y, drone.raycasts[i].end.x, drone.raycasts[i].end.y, WHITE);
				}
			}
		}
	}

	/// DRONE
	for (Drone& drone : drones)
	{
		DrawRing(drone.center(), drone.size - 4, drone.size, 0, 360, 1, WHITE);
		/// Center dot
		DrawCircle(drone.center().x, drone.center().y, 4, WHITE);
		/// Direction arrow
		DrawPolyLinesEx(drone.center(), 3, drone.size, drone.rotation, 8, WHITE);
		DrawPolyLinesEx({ drone.center().x + (drone.size - drone.size / 2) * cosf(drone.rotation * PI / 180.0),
						  drone.center().y + (drone.size - drone.size / 2) * sinf(drone.rotation * PI / 180.0) }, 3, drone.size / 2, drone.rotation, 8, WHITE);
	}

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

	/// SCREEN CENTER
	DrawCircle(screen_width / 2.f / ui_zoom, screen_height / 2.f / ui_zoom, 2, WHITE);

	/// CUSTOM CURSOR
	DrawCircle(ui_mouse_position.x, ui_mouse_position.y, 4, RED);

	DrawFPS(0, 0);
}

void Game::drawGridNode(Vector2 center, float radius, Color outlineColor, Color fillColor)
{
	DrawCircle(center.x, center.y, radius - 2, fillColor);
	DrawRing(center, radius - 8, radius - 2, 0, 360, 1, outlineColor);
}

void Game::drawGridSquare(Vector2 coords, float size, Color outlineColor, Color fillColor)
{
	DrawRectangle(coords.x * size + 1, coords.y * size + 1,
		size - 2, size - 2, fillColor);
	DrawRectangleLinesEx({ coords.x * size + 2, coords.y * size + 2,
		size - 4, size - 4 }, 4, outlineColor);
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
	camera->zoom = (monitor_height / (float)screen_height) * zoom; 
	//camera->zoom = ((float)GetScreenHeight() / (float)screen_height) * zoom;
}

void Game::raycastGridCollision(Raycast& ray)
{
	Vector2 ray_start = { ray.start.x / grid_rect_size, ray.start.y / grid_rect_size };
	Vector2 ray_direction = Vector2Normalize(Vector2Subtract(ray.end, ray.start));

	Vector2 unit_step_size = { abs(1.0f / ray_direction.x), abs(1.0f / ray_direction.y) };
	Vector2 grid_coords_check = { floorf(ray_start.x), floorf(ray_start.y) };
	int grid_check_index;

	Vector2 unit_distance;
	Vector2 step;

	if (ray_direction.x < 0)
	{
		step.x = -1;
		unit_distance.x = (ray_start.x - grid_coords_check.x) * unit_step_size.x;
	}
	else
	{
		step.x = 1;
		unit_distance.x = ((grid_coords_check.x + 1) - ray_start.x) * unit_step_size.x;
	}

	if (ray_direction.y < 0)
	{
		step.y = -1;
		unit_distance.y = (ray_start.y - grid_coords_check.y) * unit_step_size.y;
	}
	else
	{
		step.y = 1;
		unit_distance.y = ((grid_coords_check.y + 1) - ray_start.y) * unit_step_size.y;
	}

	bool obstacle_hit = false;
	float distance = 0;

	float ray_length = utils::magnitude(utils::directionToPoint(ray.start, ray.end));
	bool past_length = (distance * grid_rect_size > ray_length);

	while (!obstacle_hit && !past_length)
	{
		/// Walk to next in grid
		if (unit_distance.x < unit_distance.y)
		{
			grid_coords_check.x += step.x;
			distance = unit_distance.x;
			unit_distance.x += unit_step_size.x;
		}
		else
		{
			grid_coords_check.y += step.y;
			distance = unit_distance.y;
			unit_distance.y += unit_step_size.y;
		}

		grid_check_index = utils::coordsToIndex(grid_coords_check, grid_root_size);

		float max_range = 50000;

		if (distance * grid_rect_size > ray_length || distance * grid_rect_size > max_range) /// prevent collision past raycast length and hard limit
		{
			past_length = true;
		}
		else if (utils::coordsWithinGrid(grid_coords_check, grid_root_size) && obstacles[grid_check_index] > 0)
		{
			obstacle_hit = true;
		}
		
		Vector2 intersection;
		if (obstacle_hit)
		{
			intersection = Vector2Add(ray_start, Vector2Scale(ray_direction, distance));
			ray.collision = utils::coordsToGlobal(intersection, grid_rect_size);
			ray.collider_index = grid_check_index;
			ray.distance = distance;
			ray.collided = true;
		}
		else
		{
			ray.collided = false;
		}
	}
}
