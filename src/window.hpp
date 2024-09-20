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

enum class ColorScheme : uint8_t { // TODO: use a struct for storing RGB values
	ORIGINAL=0,
	GAMEBOY,
	END
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
	void main_loop();
	int get_minimum_width();
	int get_minimum_height();
	void start_game_loop();
	void game_loop();
	void on_key_event(const SDL_Keysym& key_info, bool is_press_event);
	void play_sound();

	SDL_Window* window_ptr;
	SDL_Renderer* renderer_ptr;

	void close() { running = false; }

protected:
	std::unique_ptr<MenuBar> m_menubar;
	Screen screen;

private:
	inline static const int MIN_WINDOW_WIDTH = 250;
	inline static const int MIN_WINDOW_HEIGHT = 125;
	inline static const int _native_menubar_height = 25;

	Chip8 chip8;

	// OG key mapping    Recommend key mapping
	//   |1|2|3|C|            |1|2|3|4|
	//   |4|5|6|D|     =>     |Q|W|E|R|
	//   |7|8|9|E|            |A|S|D|F|
	//   |A|0|B|F|            |Z|X|C|V|

	// we create a SDL Scancodes instead of KeyCodes in the case that a non-QWERTY keyboard layout is used
	// since we want each key press to be mapped to a specific physical position on a keyboard
	static inline std::array<SDL_Scancode, 16> key_map = {
		SDL_SCANCODE_1,
		SDL_SCANCODE_2,
		SDL_SCANCODE_3,
		SDL_SCANCODE_4,
		SDL_SCANCODE_Q,
		SDL_SCANCODE_W,
		SDL_SCANCODE_E,
		SDL_SCANCODE_R,
		SDL_SCANCODE_A,
		SDL_SCANCODE_S,
		SDL_SCANCODE_D,
		SDL_SCANCODE_F,
		SDL_SCANCODE_Z,
		SDL_SCANCODE_X,
		SDL_SCANCODE_C,
		SDL_SCANCODE_V
	};

	bool running = true;
};

#endif