#include "window.hpp"
#include <cstdio>
#include <stdint.h>

// g++ `pkg-config --cflags gtkmm-4.0` -o test main.cpp window.cpp screen.cpp menubar.cpp `pkg-config --libs gtkmm-4.0`

int main(int argc, char *argv[])
{
	auto app = Gtk::Application::create("org.gtkmm.example");
	return app->make_window_and_run<Window>(argc, argv);
}
