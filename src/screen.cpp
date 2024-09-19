#include "screen.hpp"
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include "ImGuiFileDialog.h"


Screen::Screen(Chip8* chip8_pointer)
	: _chip8_ptr(chip8_pointer)
{
}

Screen::~Screen()
{
}

void Screen::draw_pixels()
{
	// Get the draw list for the current ImGui window
    ImDrawList* draw_list = ImGui::GetForegroundDrawList();

	// Get the position of the current window (top-left corner)
	ImVec2 screen_pos = ImGui::GetMainViewport()->Pos;

	// Get the height of the menu bar
	float menu_bar_height = ImGui::GetFrameHeight();

	// Define the drawing position below the menu bar
    ImVec2 drawArea_start = ImVec2(screen_pos.x, screen_pos.y + menu_bar_height);  // Offset by menu bar height

	// Get the bottom-right position of the main viewport
    ImVec2 drawArea_end = ImGui::GetMainViewport()->Pos;
    drawArea_end.x += ImGui::GetMainViewport()->Size.x;
    drawArea_end.y += ImGui::GetMainViewport()->Size.y;

    // Define color (RGBA)
    ImU32 color = IM_COL32(0, 255, 0, 255);  // Red color with full opacity

	float pixel_height = (drawArea_end.y - drawArea_start.y) / static_cast<float>(_chip8_ptr->native_height);
	float pixel_width = (drawArea_end.x - drawArea_start.x) / static_cast<float>(_chip8_ptr->native_width);

    // Draw each pixel
	for (uint8_t i = 0; i < _chip8_ptr->native_height; i++) {
		for (uint8_t j = 0; j < _chip8_ptr->native_width; j++) {
			if (_chip8_ptr->px_states[(i * _chip8_ptr->native_width) + j] == 1) {
				ImVec2 x_y = ImVec2(drawArea_start.x + (j * pixel_width), drawArea_start.y + (i * pixel_height));
				ImVec2 px_x_y = ImVec2(x_y.x + pixel_width, x_y.y + pixel_height);

				draw_list->AddRectFilled(x_y, px_x_y, color);
			}
		}
	}
}