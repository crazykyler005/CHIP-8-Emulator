# allows build cmd to run if build or clean folder exists
.PHONY: build clean

clean:
	@rm -rf build/
	@echo "All build artifacts removed"

build:
	@if ! test -d ./build; then \
		mkdir build; \
		cd build && cmake -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=../build ..; \
	fi

	@cd build && make

	@if test -f ./sound.wav; then cp ./sound.wav ./build; fi

run: build
	./build/chip8

zip: clean build
	cd build && zip -r Chip8.zip chip8 sound.wav