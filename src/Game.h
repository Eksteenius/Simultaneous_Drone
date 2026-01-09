#pragma once
#include <raylib.h>

#include <iostream>
#include <cmath>
#include <vector>
#include <unordered_map>
#include <string>

#include "utils.h"

#include "Button.h"
#include "Pathfinder.h"
#include "Drone.h"

class Game
{
public:
	Game(std::shared_ptr<Camera2D> _game_camera, std::shared_ptr<Camera2D> _ui_camera);
	~Game();

	static const int screenWidth = 1920;
	static const int screenHeight = 1080;

	std::shared_ptr<Camera2D> game_camera;
	std::shared_ptr<Camera2D> ui_camera;

	float game_zoom = 1;
	float ui_zoom = 1;

	Vector2 game_offset = Vector2(0, 0);
	Vector2 ui_offset = Vector2(0, 0);

	bool init(bool reset);
	void update();
	void render();
	void render_ui();

	bool paused = true;

	/// INPUTS
	void UpdateKeyToggles();
	std::vector<KeyboardKey> keyPressedMap;
	static std::unordered_map<int, bool> keyToggledMap;
	float scrollSpeed = 0.05f;
	Vector2 game_mouse_position;
	Vector2 ui_mouse_position;
	Vector2 world_mouse_position;

	void handleZoom(std::shared_ptr<Camera2D> camera, float zoom);

private:
	/// UI

	Vector2 hovered_cell = { 0, 0 };

	/// Cells
	int grid_root_size = 16;
	float grid_rect_size = 100;
	std::vector<Cell> cells;

	/// Drones

	Drone drone;

	std::shared_ptr<Pathfinder> pathfinder;

	Vector2 destination_coords = { (float)(grid_root_size - 1), (float)(grid_root_size - 1)};

	
	/// Inputs
	std::vector<KeyboardKey> key_pause;

	std::vector<KeyboardKey> key_move_up;
	std::vector<KeyboardKey> key_move_down;
	std::vector<KeyboardKey> key_move_left;
	std::vector<KeyboardKey> key_move_right;
};