#include <cstdio>
#include <stdint.h>
// #include <GL/glew.h>
#include <gtk/gtk.h>

enum class ColorScheme : uint8_t { // TODO: use a struct for storing RGB values
	ORIGINAL=0,
	GAMEBOY,
	END
};

class Window {

public:
	Window(GtkApplication* _app, size_t width, size_t height);
	static void present(GtkApplication* app, gpointer user_data);

	inline static GtkWidget* _window;

private:
	inline static uint8_t MAX_SAVE_LOAD_STATES = 4;
	inline static uint8_t MAX_RESOLUTION_MAGNIFACTION = 4;

	size_t _width = 0;
	size_t _height = 0;

	void add_menubar(GtkApplication* app);
	void generate_menubar(GtkApplication* app);
	// void update_color_scheme(ColorScheme scheme);
	void resize_window(size_t width, size_t height);
};