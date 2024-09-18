#ifndef WINDOW_HPP
#define WINDOW_HPP

#pragma once

// #include <vector>

#include <SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include "ImGuiFileDialog.h"

#include "chip8.hpp"

class Window;

class MenuBar {

public:
	MenuBar(Chip8* chip8_pointer, Window& _parent_window);

	void generate();
	int get_height() { return _height; };

	inline static const int MAX_SAVE_LOAD_STATES = 4;
	inline static const int MAX_RESOLUTION_MAGNIFACTION = 4;

private:

	void add_file_menu();
	void add_states_menu();
	void add_settings_menu();
	void on_menu_file_load();
	void on_menu_file_quit();

	void on_menu_state_pause();
	void on_menu_state_save(int i);
	void on_menu_state_load(int i);
	void on_menu_update_resolution(int i);
	
	ImGuiFileDialog fileDialog;

	inline static const int _height = 25;

	Window& _parent_window;
	Chip8* _chip8_ptr;

	int8_t _selected_color_scheme = -1;

	std::vector<std::string> stateTimestamps = { 5, "" };
};

#endif