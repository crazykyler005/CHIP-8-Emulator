// #include "chip8.hpp"
#include "window.hpp"
//chip8 myChip8;
#include <cstdio>
#include <stdint.h>

// g++ `pkg-config --cflags gtkmm4` -o start main.cpp window.cpp `pkg-config --libs gtk4`
// g++ `pkg-config --cflags gtkmm-4.0` -o test main.cpp test2.cpp screen.cpp menubar.cpp `pkg-config --libs gtkmm-4.0`

int main(int argc, char *argv[])
{
	auto app = Gtk::Application::create("org.gtkmm.example");
	return app->make_window_and_run<Window>(argc, argv);
}
