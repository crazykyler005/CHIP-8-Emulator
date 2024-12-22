#ifndef WINDOW_HPP
#define WINDOW_HPP

#pragma once

// #include <vector>

#include <SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <memory>
#include "ImGuiFileDialog.h"

#include "chip8.hpp"
#include "superChipLegacy.hpp"
#include "superChipModern.hpp"
#include "xoChip.hpp"

class Window;

class MenuBar {

public:
	MenuBar(std::shared_ptr<Chip8Interpreter> chip8_pointer, Window& _parent_window);

	void generate();
	int get_height() { return _height; };

	inline static const int MAX_SAVE_LOAD_STATES = 4;
	inline static const int MAX_RESOLUTION_MAGNIFACTION = 4;

	uint8_t selected_resolution_multiplier = 8;

	void set_chip8_pointer(std::shared_ptr<Chip8Interpreter> chip8_pointer) { _chip8_ptr = chip8_pointer; };

private:

	void add_file_menu();
	void add_states_menu();
	void add_settings_menu();
	void add_intrepreter_menu();

	void on_menu_file_reset();
	void on_menu_file_quit();

	void on_menu_state_save(int i);
	void on_menu_state_load(int i);
	void on_menu_update_resolution(int i);

	void display_ips_configure_window();
	void display_file_load_window();
	
	ImGuiFileDialog fileDialog;
	std::string _program_name = "";

	inline static const int _height = 25;

	Window& _parent_window;
	std::shared_ptr<Chip8Interpreter> _chip8_ptr;
	
	std::vector<std::string> stateTimestamps = { 5, "" };
};

#endif