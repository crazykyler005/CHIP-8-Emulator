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

class Window
{
public:
	Window();
	int init();
	void main_loop();
	int get_minimum_width();
	int get_minimum_height();
	void start_game_loop();
	void game_loop();
	bool on_key_press_event(SDL_KeyCode key_code, SDL_Keymod modifier);
	void play_a_sound();
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

	static inline std::array<SDL_KeyCode, 16> key_map = {
		SDLK_1,
		SDLK_2,
		SDLK_3,
		SDLK_4,
		SDLK_q,
		SDLK_w,
		SDLK_e,
		SDLK_r,
		SDLK_a,
		SDLK_s,
		SDLK_d,
		SDLK_f,
		SDLK_z,
		SDLK_x,
		SDLK_c,
		SDLK_v
	};

	bool running = true;
};

#endif