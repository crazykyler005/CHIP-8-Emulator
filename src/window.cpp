#include "window.hpp"
#include "helper_functions.hpp"
#include <thread>
#define FILE_PATH "../sound.wav"

std::mutex mtx;
static bool update_texture = false;

Window::Window()
 	: m_menubar(std::make_unique<MenuBar>(&chip8, *this)),
	screen(&chip8, *this)
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
		chip8.native_width * m_menubar->selected_resolution_multiplier,
		chip8.native_height * m_menubar->selected_resolution_multiplier,
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
            std::lock_guard<std::mutex> lock(mtx);
            screen.update_texture();  // Update the texture on the main thread
            update_texture = false;
        }
		
		auto dstRect = screen.get_texture_dimensions();
		SDL_RenderCopyF(renderer_ptr, screen.get_texture(), NULL, &dstRect);

		m_menubar->generate();

		ImGui::Render();
		ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer_ptr);

		SDL_RenderPresent(renderer_ptr);
	}
}

void Window::game_loop()
{
	auto fps = std::chrono::microseconds(get_microseconds_in_second() / chip8.HZ_PER_SECOND);
	auto start_time = std::chrono::steady_clock::now();

	while (chip8.is_running) {
		if (chip8.is_paused) {
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			continue;
		}

		chip8.run_instruction();

		if (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start_time) >= fps) {

			std::lock_guard<std::mutex> lock(mtx);

			start_time = std::chrono::steady_clock::now();
			chip8.countdown_timers();

			if (chip8.play_sfx) {
				chip8.play_sfx = false;

				std::thread sound_worker(&Window::play_sound, this);
				sound_worker.detach();
			}

			if (chip8.draw_flag) {
				chip8.draw_flag = false;
				update_texture = true;
			}
		}

		sleep_thread_microseconds(get_microseconds_in_second() / chip8.opcodes_per_second);
	}
}

void Window::start_game_loop()
{
	std::thread worker(&Window::game_loop, this);
	worker.detach();
}

void Window::on_key_event(const SDL_Keysym& key_info, bool is_press_event)
{
	auto& char_pressed = key_info.sym;
	auto& modifier = key_info.mod;

	// if ctrl or alt modifiers are used, when a key within 
	// the key_map is pressed, it's considered invalid
	if (((modifier & (KMOD_CTRL | KMOD_ALT)) == KMOD_NONE)) {
		for (uint8_t i = 0; i < chip8.keys.size(); i++) {
			auto& key = chip8.keys[i];

			// if the physical position of the pressed key matches an accepted postion
			if (key.map == SDL_GetScancodeFromKey(char_pressed)) {
				chip8.process_key_event(i, is_press_event);
				return;
			}
		}

	} else if (is_press_event && ((modifier & ~(KMOD_CTRL | KMOD_ALT)) == KMOD_NONE) &&
		((modifier & KMOD_CTRL) & KMOD_CTRL) && chip8.is_running) {
		if (char_pressed == SDLK_s) {

		} else if (char_pressed == SDLK_l) {
			
		} else if (char_pressed == SDLK_p) {
			if (chip8.is_paused) {
				chip8.is_paused = false;
				return;
			}

			chip8.is_paused = true;

		// run one instruction at a time
		} else if (char_pressed == SDLK_RIGHT) {
			if (chip8.is_paused) {
				chip8.run_instruction();

				if (chip8.draw_flag) {
					chip8.draw_flag = false;
					update_texture = true;
				}
			}

		// countdown timers 
		} else if (char_pressed == SDLK_DOWN) {
			if (chip8.is_paused) {
				chip8.countdown_timers();
			}
		}

		// printf("Valid ctrl press\n");
		return;
	}

	// printf("char: %c, state: %d\n", char_pressed, modifier);

  	return;
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