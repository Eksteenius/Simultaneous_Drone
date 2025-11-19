#include "Button.h"

Button::Button()
{
}

Button::~Button()
{
}

void Button::Click()
{
    active = !active;

    clicked = true;
}

void Button::update(std::shared_ptr<Camera2D> camera)
{
    clicked = false;

    Vector2 camera_mouse_position = { (GetMousePosition().x - camera->offset.x) / camera->zoom,
                                      (GetMousePosition().y - camera->offset.y) / camera->zoom };

    if (utils::pointOverlapBox(camera_mouse_position, { bounds.x, bounds.y }, bounds.width, bounds.height))
    {
        hovered = true;

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            click_started = true;
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            if(click_started = true)
            {
                Click();
            }
            click_started = false;
        }
    }
    else
    {
        hovered = false;
    }
}

void Button::render() const
{
    if (use_highlight_colors && hovered && !active)
    {
        if (highlight_fill.a != 0)
        {
            DrawRectangleRec(bounds, highlight_fill);
        }
        DrawRectangleLinesEx(bounds, 2, highlight_outline);

        if (show_text) 
        {
            DrawText(TextFormat(text.c_str()), bounds.x + text_offset.x, bounds.y + text_offset.y, text_size, text_highlight_color);
        }
    }
    //else if (use_highlight_colors && use_active_colors && hovered && active) 
    //{}
    else if (use_active_colors && active)
    {
        if (active_fill.a != 0)
        {
            DrawRectangleRec(bounds, active_fill);
        }
        DrawRectangleLinesEx(bounds, 2, active_outline);

        if (show_text) 
        {
            DrawText(TextFormat(text.c_str()), bounds.x + text_offset.x, bounds.y + text_offset.x, text_size, text_active_color);
        }
    }
    else
    {
        if (fill.a != 0)
        {
            DrawRectangleRec(bounds, fill);
        }
        DrawRectangleLinesEx(bounds, 2, outline);

        if (show_text) 
        {
            DrawText(TextFormat(text.c_str()), bounds.x + text_offset.x, bounds.y + text_offset.y, text_size, text_color);
        }
    }
}
