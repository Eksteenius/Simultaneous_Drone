#pragma once
#include <raylib.h>

#include <iostream>

#include "utils.h"

class Button
{
public:
	Button();
	~Button();

	void Click();

	void update(std::shared_ptr<Camera2D> camera);
	void render() const;

	Rectangle bounds = { 0,0,100,100 };

	bool show_text = true;
	std::string text = "";
	Vector2 text_offset = { 10, 10 };
	float text_size = 10;

	/// Default Colors
	Color text_color = WHITE;
	Color outline = WHITE;
	Color fill = { 0,0,0,0 };

	bool use_highlight_colors = true;
	Color text_highlight_color = WHITE;
	Color highlight_outline = SKYBLUE;
	Color highlight_fill = { 0,0,0,0 };

	bool use_active_colors = true;
	Color text_active_color = WHITE;
	Color active_outline = WHITE;
	Color active_fill = BLUE;

	bool clickable = true;

	/// Button states
	bool active = false;
	bool hovered = false;

	bool clicked = false;

private:
	bool click_started = false;
};
