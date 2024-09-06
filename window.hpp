#ifndef MENUBAR_HPP
#define MENUBAR_HPP

#pragma once

#include <gtkmm.h>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "screen.hpp"
#include "chip8.hpp"

enum class ColorScheme : uint8_t { // TODO: use a struct for storing RGB values
	ORIGINAL=0,
	GAMEBOY,
	END
};

// Forward declare MenuBar class
class MenuBar;

class Window : public Gtk::Window
{
public:
	Window();
	void main_loop();
	int get_minimum_width();
	int get_minimum_height();
	bool on_key_pressed(guint keyval, guint keycode, Gdk::ModifierType state);

protected:
	Gtk::Box m_box;

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
	static inline std::array<guint, 16> key_map = {
		GDK_KEY_1,
		GDK_KEY_2,
		GDK_KEY_3,
		GDK_KEY_4,
		GDK_KEY_Q,
		GDK_KEY_W,
		GDK_KEY_E,
		GDK_KEY_R,
		GDK_KEY_A,
		GDK_KEY_S,
		GDK_KEY_D,
		GDK_KEY_F,
		GDK_KEY_Z,
		GDK_KEY_X,
		GDK_KEY_C,
		GDK_KEY_V
	};

	static inline Gdk::ModifierType NO_KEY_MODIFIER = static_cast<Gdk::ModifierType>(0);
};

#endif