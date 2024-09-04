#pragma once

#include <gtkmm.h>
// #include <vector>
// #include "window.hpp"
#include "chip8.hpp"

class MenuBar : public Gtk::PopoverMenuBar {

public:
	inline static int MAX_SAVE_LOAD_STATES = 4;
	inline static int MAX_RESOLUTION_MAGNIFACTION = 4;

	void generate();
	int get_height() { return _height; };
	void set_chip8_pointer(Chip8* chip8_pointer) { _chip8_ptr; };

private:
	void on_menu_file_load();
	void on_menu_file_quit();

	void on_menu_state_pause();
	void on_menu_state_save(int i);
	void on_menu_state_load(int i);
	void on_menu_update_resolution(int i);
	void on_file_dialog_finish(const Glib::RefPtr<Gio::AsyncResult>& result, const Glib::RefPtr<Gtk::FileDialog>& dialog);

	Glib::RefPtr<Gio::SimpleActionGroup> m_refActionGroup;

	int _height = 25;

	std::string _default_title = "test";
	Gtk::Window* _parent_window;
	Chip8* _chip8_ptr;

	// std::vector<Glib::RefPtr<Gio::MenuItem>> save_menu_items;
	// std::vector<Glib::RefPtr<Gio::MenuItem>> load_menu_items;
};
