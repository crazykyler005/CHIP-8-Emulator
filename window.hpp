#include <cstdio>
#include <stdint.h>

enum class SatelliteMessageType : uint8_t {
	SHELL = 0,
	ERROR
};

enum class ColorScheme : uint8_t { // TODO: use a struct for storing RGB values
	ORIGINAL=0,
	GAMEBOY=1,
	
};

class Window {

public:
	void set_screen_size(size_t width, size_t height);
	void update_color_scheme(ColorScheme scheme);

private:
	size_t _width = 0;
	size_t _height = 0;
};