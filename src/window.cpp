#include "window.hpp"
// #include "menubar.hpp"
#include "helper_functions.hpp"
#include <thread>
#define FILE_PATH "../sound.wav"

static void my_audio_callback(void *userdata, Uint8 *stream, int len);
static uint32_t audio_len;
static uint8_t *audio_pos;

Window::Window()
 	: m_menubar(std::make_unique<MenuBar>(&chip8, *this)),
	screen(&chip8)
{

}

int Window::init() {

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		return -1;
	}

	printf("test\n");

	window_ptr = SDL_CreateWindow(
		"window",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		300,
		300,
		SDL_WINDOW_SHOWN
	);

	if (!window_ptr) {
		SDL_Quit();
		return -1;
	}

	printf("test\n");

	renderer_ptr = SDL_CreateRenderer(window_ptr, -1, 0);

	if (!renderer_ptr) {
		SDL_DestroyWindow(window_ptr);
		SDL_Quit();
		return -1;
	}

	printf("test\n");

	// init imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui_ImplSDL2_InitForSDLRenderer(
		window_ptr,
		renderer_ptr
	);

	printf("test\n");

	ImGui_ImplSDLRenderer2_Init(renderer_ptr);

	printf("test\n");

	return 0;
}

void Window::main_loop() 
{
	while (running) {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				running = false;
			} else if (e.type == SDL_KEYDOWN) {
				on_key_press_event(static_cast<SDL_KeyCode>(e.key.keysym.sym), static_cast<SDL_Keymod>(e.key.keysym.mod));
			}

			ImGui_ImplSDL2_ProcessEvent(&e);
		}

		ImGui_ImplSDLRenderer2_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		screen.render();
		
		m_menubar->generate();

		ImGui::Render();

		SDL_SetRenderDrawColor(renderer_ptr, 120, 180, 255, 255);
		SDL_RenderClear(renderer_ptr);
		ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer_ptr);

		SDL_RenderPresent(renderer_ptr);

		// running = false;
	}

	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyRenderer(renderer_ptr);
	SDL_DestroyWindow(window_ptr);
	SDL_Quit();
}

void Window::game_loop()
{
	// uint16_t i = 0;

	while (chip8.is_running) {
		if (!chip8.is_paused) {
			// printf("%d ", i);
			chip8.run();

			if (chip8.is_paused) {
				return;
			}

			// if (chip8.draw_flag) {
			// 	// screen.queue_draw();
			// 	chip8.draw_flag = false;
			// }

			if (chip8.play_sfx) {
				// TODO: play sound effect
				// printf("BEEP!\n");
				// play_a_sound();
				chip8.play_sfx = false;
			}

			sleep_thread_microseconds(Chip8::MICRO_SECONDS_PER_FRAME);
		} else {
			sleep_thread_microseconds(500000);
		}
	}
}

void Window::start_game_loop()
{
	std::thread worker(&Window::game_loop, this);
	worker.detach();
}

int Window::get_minimum_width() {
	// int calculated_min_width;

	// while(calculated_min_width < MIN_WINDOW_WIDTH) {
	// 	calculated_min_width += chip8.native_width * 4;
	// }

	return chip8.native_width * 4;
}
int Window::get_minimum_height() {

	// int calculated_min_height = _native_menubar_height;

	// // TODO: get the actual size of the titlebar 
	int titlebar_height = 7;
	// calculated_min_height += titlebar_height;

	// while(calculated_min_height < MIN_WINDOW_HEIGHT) {
	// 	calculated_min_height += chip8.native_height;
	// }

	return (chip8.native_height * 4) + _native_menubar_height + titlebar_height;
}

bool Window::on_key_press_event(SDL_KeyCode key_code, SDL_Keymod modifier)
{
	for (uint8_t i = 0; i < key_map.size(); i++) {

		if ((key_map[i] == key_code) &&
			// if ctrl or alt modifiers are used, mark key as un-pressed
			((modifier & (KMOD_CTRL | KMOD_ALT)) == (modifier & KMOD_NONE))
		) {
			chip8.keys_pressed[i] = true;

			printf("Valid key press\n");
			return true;
		}
	}

	if (((modifier & ~(KMOD_CTRL | KMOD_ALT)) == KMOD_NONE) &&
		((modifier & (KMOD_CTRL)) == KMOD_CTRL)) {
		if (key_code == SDLK_s) {

		} else if (key_code == SDLK_l) {

		} else if (key_code == SDLK_p) {

		}

		printf("Valid ctrl press\n");
		return true;
	}

	printf("char: %c, state: %d\n", key_code, modifier);

  	return false;
}

void Window::play_a_sound()
{
	std::thread worker(&Window::play_sound, this);
	worker.detach();
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
    // You can add cases for other formats like AUDIO_S32LSB or AUDIO_F32 here if needed.
}

void Window::play_sound()
{
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		printf("failed to initialize sound\n");
		return;
	}

	SDL_AudioSpec wav_spec;
	uint8_t* wav_buffer;
	uint32_t wav_length;

	if( SDL_LoadWAV(FILE_PATH, &wav_spec, &wav_buffer, &wav_length) == NULL ){
		printf("failed to find sound file\n");
		return;
	}

	SDL_AudioDeviceID device_id = SDL_OpenAudioDevice(NULL, 0, &wav_spec, NULL, 0);
	if (device_id == 0) {
        fprintf(stderr, "Failed to open audio device: %s\n", SDL_GetError());
        SDL_FreeWAV(wav_buffer);
        return;
    }

	adjust_volume(wav_buffer, wav_length, &wav_spec, 0.5f);

	// if ( SDL_OpenAudio(&wav_spec, NULL) < 0 ){
	// 	fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
	// 	return;
	// }

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

	printf("played sound\n");
}