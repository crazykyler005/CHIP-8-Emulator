#ifndef MENUBAR_HPP
#define MENUBAR_HPP

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <SDL.h>

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include "ImGuiFileDialog.h"

#include "screen.hpp"
#include "chip8.hpp"
#include "menubar.hpp"

// enum class ColorScheme : uint8_t { // TODO: use a struct for storing RGB values
// 	ORIGINAL=0,
// 	VIRTUAL_BOY,
// 	GAMEBOY,
// 	END
// };

struct ColorScheme {
	std::string name;
	uint32_t foreground_color;
	uint32_t intersection_color;
	uint32_t background_color;
	uint32_t unselected_plane_color;

	// Constructor to initialize the values
    ColorScheme(const std::string& name, uint32_t foreground_color, uint32_t intersection_color, uint32_t background_color, uint32_t unselected_plane_color)
        : name(name), foreground_color(foreground_color), intersection_color(intersection_color), background_color(background_color), unselected_plane_color(unselected_plane_color) {}
};

// Forward declare MenuBar class
class MenuBar;
class Screen;

class Window
{
public:
	Window();
	~Window();
	int init();

	void switch_interpreter(Chip8Type type);

	void main_loop();
	void start_game_loop();
	void stop_game_loop();
	void game_loop();
	void run_single_instruction();
	void on_key_event(const SDL_Keysym& key_info, bool is_press_event);
	void play_sound();

	SDL_Window* window_ptr;
	SDL_Renderer* renderer_ptr;

	void close() { running = false; }

	// TODO: implement this better
	inline static uint32_t get_rgba32_value(uint8_t r, uint8_t g, uint8_t b) {
		// AABBGGRR
		static uint8_t a = 255;
		return (a << 24) | (b << 16) | (g << 8) | r;
	}

	static inline std::array<ColorScheme, 5> COLOR_SCHEME_ARRAY = {
		ColorScheme("Orginial", get_rgba32_value(255, 255, 255), get_rgba32_value(171, 171, 171), get_rgba32_value(85, 85, 85), get_rgba32_value(0, 0, 0) ),
		ColorScheme( "Gameboy", get_rgba32_value(144, 140, 12), get_rgba32_value(104, 129, 57), get_rgba32_value(68, 99, 61), get_rgba32_value(43, 76, 45) ),
		ColorScheme(  "Sunset", get_rgba32_value(249, 205, 166), get_rgba32_value(227, 155, 127), get_rgba32_value(171, 100, 90), get_rgba32_value(82, 50, 55) ),
		ColorScheme(   "Chill", get_rgba32_value(196, 210, 132), get_rgba32_value(174, 164, 110), get_rgba32_value(139, 110, 89), get_rgba32_value(98, 62, 62) ),
		ColorScheme( "Vibrant", get_rgba32_value(126, 31, 35), get_rgba32_value(196, 24, 31), get_rgba32_value(18, 10, 25), get_rgba32_value(94, 64, 105) )
	};

	static inline uint8_t selected_color_scheme = 0;

private:
	inline static const int MIN_WINDOW_WIDTH = 250;
	inline static const int MIN_WINDOW_HEIGHT = 125;
	inline static const int _native_menubar_height = 25;

	std::shared_ptr<Chip8Interpreter> _chip8_ptr = std::make_shared<XOChip>();

	bool running = true;

protected:
	std::unique_ptr<MenuBar> m_menubar;
	Screen screen;
};

#endif