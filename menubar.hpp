#ifndef WINDOW_HPP
#define WINDOW_HPP

#pragma once

#include <gtkmm.h>
// #include <vector>

#include "chip8.hpp"

class Window;

class MenuBar : public Gtk::PopoverMenuBar {

public:
	MenuBar(Chip8* chip8_pointer, Window& _parent_window);

	void generate();
	int get_height() { return _height; };

	inline static const int MAX_SAVE_LOAD_STATES = 4;
	inline static const int MAX_RESOLUTION_MAGNIFACTION = 4;

private:
	void on_menu_file_load();
	void on_menu_file_quit();

	void on_menu_state_pause();
	void on_menu_state_save(int i);
	void on_menu_state_load(int i);
	void on_menu_update_resolution(int i);
	void on_file_dialog_finish(const Glib::RefPtr<Gio::AsyncResult>& result, const Glib::RefPtr<Gtk::FileDialog>& dialog);

	Glib::RefPtr<Gio::SimpleActionGroup> m_refActionGroup;

	inline static const int _height = 25;

	Window& _parent_window;
	Chip8* _chip8_ptr;

	// std::vector<Glib::RefPtr<Gio::MenuItem>> save_menu_items;
	// std::vector<Glib::RefPtr<Gio::MenuItem>> load_menu_items;
};

#endif