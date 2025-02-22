#include <cstdio>
#include <stdint.h>
#include <string>
#include <vector>

#include "window.hpp"

int main(int argc, char *argv[])
{
	Window win;

	if (win.init() < 0) {
		return -1;
	}

	win.main_loop();

	return 0;
}
