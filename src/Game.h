#pragma once
#include <raylib.h>

#include <iostream>
#include <random>
#include <cmath>
#include <vector>
#include <unordered_map>
#include <string>

#include "utils.h"

#include "Button.h"

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

	bool init(bool reset);
	void update();
	void render();
	void render_ui();

	/// INPUTS
	void UpdateKeyToggles();
	std::vector<KeyboardKey> keyPressedMap;
	static std::unordered_map<int, bool> keyToggledMap;
	float scrollSpeed = 0.05f;
	Vector2 game_mouse_position;
	Vector2 ui_mouse_position;

	void handleZoom();

	float randomFromRange(float min, float max);

private:
	/// UI
};