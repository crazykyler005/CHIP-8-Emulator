#include <chip8.hpp>


chip8 myChip8;

int main(int argc, char **argv) {

	myChip8.initialize();
	myChip8.load_program("game");
}