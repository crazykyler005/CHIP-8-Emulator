#include "window.hpp"
#include "helper_functions.hpp"
#include <thread>
#define FILE_PATH "../sound.wav"

std::mutex mtx;
static bool update_texture = false;

Window::Window()
 	: m_menubar(std::make_unique<MenuBar>(_chip8_ptr, *this)),
	screen(_chip8_ptr, *this)
{
}

Window::~Window()
{
	if (!renderer_ptr && !window_ptr) {
		return;
	}

	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyRenderer(renderer_ptr);
	SDL_DestroyWindow(window_ptr);
	SDL_Quit();
}

int Window::init() {

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		return -1;
	}

	window_ptr = SDL_CreateWindow(
		"window",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		_chip8_ptr->native_width * m_menubar->selected_resolution_multiplier,
		_chip8_ptr->native_height * m_menubar->selected_resolution_multiplier,
		SDL_WINDOW_SHOWN
	);

	if (!window_ptr) {
		SDL_Quit();
		return -1;
	}

	renderer_ptr = SDL_CreateRenderer(window_ptr, -1, 0);

	if (!renderer_ptr) {
		SDL_DestroyWindow(window_ptr);
		SDL_Quit();
		return -1;
	}

	// init imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui_ImplSDL2_InitForSDLRenderer(
		window_ptr,
		renderer_ptr
	);

	ImGui_ImplSDLRenderer2_Init(renderer_ptr);

	screen.generate_texture();

	return 0;
}

void Window::switch_interpreter(Chip8Type type)
{
	bool was_running = _chip8_ptr->is_running;
	
	stop_game_loop();

	std::lock_guard<std::mutex> lock(mtx);

	// need to first nullify the shared pointer references so an objects destructor doesn't modify any changes made by the new objects constructor
	_chip8_ptr = nullptr;
	m_menubar->set_chip8_pointer(nullptr);
	screen.set_chip8_pointer(nullptr);

	switch (type) {
		case Chip8Type::ORIGINAL:
		case Chip8Type::CHIP48:
			_chip8_ptr = std::make_shared<Chip8>(type);
			break;
		case Chip8Type::SUPER_1p0:
		case Chip8Type::SUPER_1p1:
			_chip8_ptr = std::make_shared<SuperChipLegacy>(type);
			break;
		case Chip8Type::SUPER_MODERN:
			_chip8_ptr = std::make_shared<SuperChipModern>();
			break;
		case Chip8Type::XO:
			_chip8_ptr = std::make_shared<XOChip>();
			break;
		default:
			throw std::invalid_argument("Invalid Chip8Type");
	}

	m_menubar->set_chip8_pointer(_chip8_ptr);
	screen.set_chip8_pointer(_chip8_ptr);

	// updates texture based on new native dimensions
	screen.generate_texture();
	// load program into new Chip8Inerpreter instance
	_chip8_ptr->reset();

	if (was_running) {
		start_game_loop();
	}
}

void Window::main_loop() 
{
	// prevents last selected menubar dropdowns from staying on the screen
	screen.update_texture();

	while (running) {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				running = false;
			} else if (e.type == SDL_KEYDOWN) {
				on_key_event(e.key.keysym, true);
			} else if (e.type == SDL_KEYUP) {
				on_key_event(e.key.keysym, false);
			}

			ImGui_ImplSDL2_ProcessEvent(&e);
		}

		ImGui_ImplSDLRenderer2_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		if (update_texture) {
            // std::lock_guard<std::mutex> lock(mtx);
            screen.update_texture();  // Update the texture on the main thread
            update_texture = false;
        }
		
		auto dstRect = screen.get_texture_dimensions();
		SDL_RenderCopyF(renderer_ptr, screen.get_texture(), NULL, &dstRect);

		m_menubar->generate();

		ImGui::Render();
		ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer_ptr);

		SDL_RenderPresent(renderer_ptr);

		if (!_chip8_ptr->is_running || _chip8_ptr->is_paused) {
			// wait 1/60th of a second
			std::this_thread::sleep_for(std::chrono::milliseconds(16));
			continue;
		}

		uint32_t instructions_ran = 0;
		auto instruction_start_time = std::chrono::steady_clock::now();

		while (instructions_ran++ < _chip8_ptr->opcodes_per_frame) {
			_chip8_ptr->run_instruction();
			continue;
		}

		auto fps = std::chrono::microseconds(get_microseconds_in_second() / _chip8_ptr->HZ_PER_SECOND);
		auto time_passed = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - instruction_start_time);

		if (std::chrono::duration_cast<std::chrono::microseconds>(time_passed) < fps) {
			auto sleep_duration = std::chrono::duration_cast<std::chrono::microseconds>(fps - time_passed).count();
			sleep_thread_microseconds(sleep_duration);
		}

		_chip8_ptr->countdown_timers();

		if (_chip8_ptr->play_sfx) {
			_chip8_ptr->play_sfx = false;

			std::thread sound_worker(&Window::play_sound, this);
			sound_worker.detach();
		}

		if (_chip8_ptr->draw_flag) {
			_chip8_ptr->draw_flag = false;
			update_texture = true;
		}

	}
}

// void Window::game_loop()
// {
// 	auto fps = std::chrono::microseconds(get_microseconds_in_second() / _chip8_ptr->HZ_PER_SECOND);
// 	auto start_time = std::chrono::steady_clock::now();
// 	uint32_t instructions_ran = 0;

// 	while (_chip8_ptr->is_running) {
// 		if (_chip8_ptr->is_paused) {
// 			std::this_thread::sleep_for(std::chrono::milliseconds(500));
// 		}

// 		_chip8_ptr->run_instruction();

// 		if (++instructions_ran < _chip8_ptr->opcodes_per_frame) {
// 			continue;
// 		}

// 		auto time_passed = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start_time);

// 		if (std::chrono::duration_cast<std::chrono::microseconds>(time_passed) < fps) {
// 			auto sleep_duration = std::chrono::duration_cast<std::chrono::microseconds>(fps - time_passed).count();
// 			sleep_thread_microseconds(sleep_duration);
// 		}

// 		std::lock_guard<std::mutex> lock(mtx);

// 		instructions_ran = 0;
// 		start_time = std::chrono::steady_clock::now();
// 		_chip8_ptr->countdown_timers();

// 		if (_chip8_ptr->play_sfx) {
// 			_chip8_ptr->play_sfx = false;

// 			std::thread sound_worker(&Window::play_sound, this);
// 			sound_worker.detach();
// 		}

// 		if (_chip8_ptr->draw_flag) {
// 			_chip8_ptr->draw_flag = false;
// 			update_texture = true;
// 		}
// 	}
// }

void Window::run_single_instruction() {
	static uint16_t ran_instructions = 0;

	// _chip8_ptr->print_current_opcode();
	_chip8_ptr->run_instruction();

	if (++ran_instructions >= _chip8_ptr->opcodes_per_frame) {
		_chip8_ptr->countdown_timers();
		ran_instructions = 0;
	}

	if (_chip8_ptr->play_sfx) {
		_chip8_ptr->play_sfx = false;

		std::thread sound_worker(&Window::play_sound, this);
		sound_worker.detach();
	}

	if (_chip8_ptr->draw_flag) {
		_chip8_ptr->draw_flag = false;
		update_texture = true;
	}
}

void Window::start_game_loop()
{
	_chip8_ptr->is_running = true;
	//std::thread worker(&Window::game_loop, this);
	//worker.detach();
}

void Window::on_key_event(const SDL_Keysym& key_info, bool is_press_event)
{
	auto& char_pressed = key_info.sym;
	auto& modifier = key_info.mod;

	// if ctrl or alt modifiers are used, when a key within 
	// the key_map is pressed, it's considered invalid
	if (((modifier & (KMOD_CTRL | KMOD_ALT)) == KMOD_NONE)) {
		for (uint8_t i = 0; i < _chip8_ptr->keys.size(); i++) {
			auto& key = _chip8_ptr->keys[i];

			// if the physical position of the pressed key matches an accepted postion
			if (key.map == SDL_GetScancodeFromKey(char_pressed)) {
				_chip8_ptr->process_key_event(i, is_press_event);
				return;
			}
		}

	} else if (is_press_event && ((modifier & (KMOD_CTRL | KMOD_ALT)) != KMOD_NONE) &&
		(modifier & KMOD_CTRL) && _chip8_ptr->is_running) {

		if (char_pressed == SDLK_s) {

		} else if (char_pressed == SDLK_l) {
			
		} else if (char_pressed == SDLK_p) {
			if (_chip8_ptr->is_paused) {
				_chip8_ptr->is_paused = false;
				return;
			}

			_chip8_ptr->is_paused = true;
		} else if (char_pressed == SDLK_r) {
			m_menubar->on_menu_file_reset();

		// run one instruction at a time
		} else if (char_pressed == SDLK_RIGHT) {
			if (_chip8_ptr->is_paused && _chip8_ptr->is_running) {
				run_single_instruction();
			}
		}

		// // countdown timers 
		// } else if (char_pressed == SDLK_DOWN) {
		// 	_chip8_ptr->countdown_timers()
		// 	}
		// }

		// printf("Valid ctrl press\n");
		return;
	}

	// printf("char: %c, state: %d\n", char_pressed, modifier);

  	return;
}

void Window::stop_game_loop() {
	if (!_chip8_ptr->is_running) {
		return;
	}

	_chip8_ptr->is_running = false;

	// wait for game loop thread to finish
	std::this_thread::sleep_for(std::chrono::microseconds((get_microseconds_in_second() / (_chip8_ptr->opcodes_per_frame))));
}

void adjust_volume(uint8_t* wav_buffer, uint32_t wav_length, SDL_AudioSpec* wav_spec, float volume) {
    // Depending on the audio format, the sample size can vary.
    // For simplicity, this example assumes 16-bit signed audio (AUDIO_S16LSB).
    
    // Calculate the number of samples (wav_length is in bytes)
    if (wav_spec->format == AUDIO_U8) {
        // 8-bit unsigned audio
        for (uint32_t i = 0; i < wav_length; ++i) {
            wav_buffer[i] = (uint8_t)(wav_buffer[i] * volume);
        }
    } else if (wav_spec->format == AUDIO_S16LSB) {
        // 16-bit signed audio
        int16_t* samples = (int16_t*)wav_buffer;
        uint32_t sample_count = wav_length / 2; // 2 bytes per sample

        for (uint32_t i = 0; i < sample_count; ++i) {
            samples[i] = (int16_t)(samples[i] * volume);
        }
    }
}

void Window::play_sound()
{
	SDL_AudioSpec wav_spec;
	uint8_t* wav_buffer;
	uint32_t wav_length;

	if( SDL_LoadWAV(FILE_PATH, &wav_spec, &wav_buffer, &wav_length) == NULL ){
		fprintf(stderr, "Failed to find sound file\n");
		return;
	}

	SDL_AudioDeviceID device_id = SDL_OpenAudioDevice(NULL, 0, &wav_spec, NULL, 0);
	if (device_id == 0) {
        fprintf(stderr, "Failed to open audio device: %s\n", SDL_GetError());
        SDL_FreeWAV(wav_buffer);
        return;
    }

	adjust_volume(wav_buffer, wav_length, &wav_spec, 0.5f);

	if (SDL_QueueAudio(device_id, wav_buffer, wav_length) < 0) {
		fprintf(stderr, "Failed to queue audio: %s\n", SDL_GetError());
		SDL_CloseAudioDevice(device_id);
		SDL_FreeWAV(wav_buffer);
		return;
	}
	
	/* Start playing */
	SDL_PauseAudioDevice(device_id, 0);

	// wait until we're done playing
	while ( SDL_GetQueuedAudioSize(device_id) > 0 ) {
		SDL_Delay(100); 
	}
	
	// shut everything down
	SDL_CloseAudioDevice(device_id);
	SDL_FreeWAV(wav_buffer);
}