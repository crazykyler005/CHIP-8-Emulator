#ifndef MENUBAR_HPP
#define MENUBAR_HPP

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <SDL2/SDL.h>

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
	uint32_t color1;
	uint32_t color2;

	// Constructor to initialize the values
    ColorScheme(const std::string& name, uint32_t color1, uint32_t color2)
        : name(name), color1(color1), color2(color2) {}
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

	static inline std::array<ColorScheme, 4> COLOR_SCHEME_ARRAY = {
		ColorScheme("Orginial", get_rgba32_value(255, 255, 255), get_rgba32_value(0, 0, 0) ),
		ColorScheme( "Gameboy", get_rgba32_value(113, 129, 40), get_rgba32_value(209, 209, 88) ),
		ColorScheme(    "Mint", get_rgba32_value(149, 199, 152), get_rgba32_value(133, 109, 82) ),
		ColorScheme(    "Gold", get_rgba32_value(207, 171, 81), get_rgba32_value(64, 51, 47) )
	};

	static inline uint8_t seletected_color_scheme = 0;
	static inline bool is_run_one_instruction = false;

private:
	inline static const int MIN_WINDOW_WIDTH = 250;
	inline static const int MIN_WINDOW_HEIGHT = 125;
	inline static const int _native_menubar_height = 25;

	std::shared_ptr<Chip8Interpreter> _chip8_ptr = std::make_shared<SuperChip>(Chip8Type::SUPER_1p1);

	bool running = true;

protected:
	std::unique_ptr<MenuBar> m_menubar;
	Screen screen;
};

#endif