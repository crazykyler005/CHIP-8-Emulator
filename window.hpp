#include <gtkmm.h>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "screen.hpp"
#include "menubar.hpp"


// g++ `pkg-config --cflags gtkmm-4.0` -o test test2.cpp window.cpp `pkg-config --libs gtkmm-4.0`
// g++ `pkg-config --cflags gtkmm-4.0` -o test test2.cpp screen.cpp menubar.hpp `pkg-config --libs gtkmm-4.0`

enum class ColorScheme : uint8_t { // TODO: use a struct for storing RGB values
	ORIGINAL=0,
	GAMEBOY,
	END
};

class Window : public Gtk::Window
{
public:
	Window();
	int get_native_width();
	int get_native_height();
	std::string default_title = "Chip-8 Emulator";

protected:
	Gtk::Box m_box;
	MenuBar m_menubar;

	Screen screen;

private:
	int _native_width = 400;
	int _native_height = 300;
};
