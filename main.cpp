// #include "chip8.hpp"
#include "window.hpp"
//chip8 myChip8;
#include <cstdio>
#include <stdint.h>

// g++ `pkg-config --cflags gtk4` -o start main.cpp window.cpp `pkg-config --libs gtk4`

int main(int argc, char **argv) {

	GtkApplication *app;
	int status;

	app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(app, "activate", G_CALLBACK(+[] (GtkApplication* app, gpointer user_data) {
        Window *win = new Window(app, 400, 400);
        gtk_window_present(GTK_WINDOW(win->_window));
    }), NULL);

	status = g_application_run (G_APPLICATION (app), argc, argv);
	g_object_unref (app);

	return status;
}
