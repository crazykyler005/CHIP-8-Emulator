#include "menubar.hpp"
#include "helper_functions.hpp"
#include "window.hpp"

#include <cstdio>
#include <iostream>

static bool display_ips_config = false;

MenuBar::MenuBar(Chip8* chip8_pointer, Window& parent_window)
	: _chip8_ptr(chip8_pointer), _parent_window(parent_window)
{
}

void MenuBar::generate() 
{
	if (ImGui::BeginMainMenuBar())
	{
		add_file_menu();
		add_states_menu();
		add_settings_menu();

		ImGui::EndMainMenuBar();
	}

	display_ips_configure_window();
	display_file_load_window();
}

void MenuBar::add_file_menu()
{
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Load", "Ctrl+O")) {
			if (!fileDialog.IsOpened()) {
				fileDialog.OpenDialog("ChooseFileDlgKey", "Choose File", ".ch8,.txt,.*");

				if (_chip8_ptr->is_running) {
					// wait for game loop thread to finish
					std::this_thread::sleep_for(std::chrono::microseconds((get_microseconds_in_second() / (_chip8_ptr->opcodes_per_second)) * 2));

					_chip8_ptr->reset();
				}
			}
		}

		if (ImGui::MenuItem("Reset", NULL, false, _chip8_ptr->is_running)) {
			on_menu_file_reset();
		}

		ImGui::Separator();

		if (ImGui::MenuItem("Quit", "Alt+F4")) {
			_parent_window.close();
		}

		ImGui::EndMenu();
	}
}

void MenuBar::add_states_menu()
{
	if (ImGui::BeginMenu("States"))
	{
		if (ImGui::BeginMenu("Save State", _chip8_ptr->is_running))
		{
			for (uint8_t i = 0; i < stateTimestamps.size(); i++) {

				// if button is selected
				if (ImGui::MenuItem(("State " + std::to_string(i)).c_str(), stateTimestamps[i].c_str())) {
					stateTimestamps[i] = get_time_str(true, utc_time_in_seconds());
				}
			}
			
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Load State", _chip8_ptr->is_running))
		{
			for (uint8_t i = 0; i < stateTimestamps.size(); i++) {

				// if button was selected
				if ( ImGui::MenuItem(("State " + std::to_string(i)).c_str(), 
						stateTimestamps[i].c_str(), 
						false, 
						!stateTimestamps[i].empty())
				) {
					// load save state
				}
			}
			
			ImGui::EndMenu();
		}

		ImGui::EndMenu();
	}
}

void MenuBar::add_settings_menu()
{
	if (ImGui::BeginMenu("Settings"))
	{
		ImGui::MenuItem("Disable sound", NULL, &_chip8_ptr->sound_disabled);

		if (ImGui::MenuItem("Pause", "ctrl p", &_chip8_ptr->is_paused));

		if (ImGui::BeginMenu("Resolution"))
		{
			for (int i = 4; i <= 16; i+=4) {

				auto action_name = "x" + std::to_string(i);

				if (ImGui::MenuItem((action_name).c_str(), NULL, (i == selected_resolution_multiplier))) {
					on_menu_update_resolution(i);
				}
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Colors Schemes"))
		{
		    float sz = ImGui::GetTextLineHeight();
		    for (int i = 0; i < Window::COLOR_SCHEME_ARRAY.size(); i++)
		    {
		        ImVec2 c1_pos = ImGui::GetCursorScreenPos();
				ImVec2 c2_pos = {c1_pos.x + sz + 3, c1_pos.y};

		        ImGui::GetWindowDrawList()->AddRectFilled(c1_pos, ImVec2(c1_pos.x + sz, c1_pos.y + sz), Window::COLOR_SCHEME_ARRAY[i].color1);
				ImGui::GetWindowDrawList()->AddRectFilled(c2_pos, ImVec2(c2_pos.x + sz, c2_pos.y + sz), Window::COLOR_SCHEME_ARRAY[i].color2);
		        ImGui::Dummy(ImVec2(sz * 2, sz));
		        ImGui::SameLine();
		        
				if ( ImGui::MenuItem(Window::COLOR_SCHEME_ARRAY[i].name.c_str(), 
						(i == 0) ? "default" : "",
						(Window::seletected_color_scheme == i)
				)) {
					Window::seletected_color_scheme = i;
				}
		    }
		    ImGui::EndMenu();
		}

		if (ImGui::MenuItem("Set instructions per second", NULL, false, true))
        {
			display_ips_config = true;
        }

		ImGui::MenuItem("Enable 0xFX1E overflow", NULL, &_chip8_ptr->_0xFX1E_overflow_enabled);
		ImGui::EndMenu();
	}
}

void MenuBar::add_intrepreter_menu()
{
	if (ImGui::BeginMenu("Intrepreters"))
	{
		ImGui::MenuItem("Disable sound", NULL, &_chip8_ptr->sound_disabled);

		if (ImGui::MenuItem("Pause", "ctrl p", &_chip8_ptr->is_paused));

		if (ImGui::BeginMenu("Resolution"))
		{
			for (int i = 4; i <= 16; i+=4) {

				auto action_name = "x" + std::to_string(i);

				if (ImGui::MenuItem((action_name).c_str(), NULL, (i == selected_resolution_multiplier))) {
					on_menu_update_resolution(i);
				}
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Colors Schemes"))
		{
		    float sz = ImGui::GetTextLineHeight();
		    for (int i = 0; i < Window::COLOR_SCHEME_ARRAY.size(); i++)
		    {
		        ImVec2 c1_pos = ImGui::GetCursorScreenPos();
				ImVec2 c2_pos = {c1_pos.x + sz + 3, c1_pos.y};

		        ImGui::GetWindowDrawList()->AddRectFilled(c1_pos, ImVec2(c1_pos.x + sz, c1_pos.y + sz), Window::COLOR_SCHEME_ARRAY[i].color1);
				ImGui::GetWindowDrawList()->AddRectFilled(c2_pos, ImVec2(c2_pos.x + sz, c2_pos.y + sz), Window::COLOR_SCHEME_ARRAY[i].color2);
		        ImGui::Dummy(ImVec2(sz * 2, sz));
		        ImGui::SameLine();
		        
				if ( ImGui::MenuItem(Window::COLOR_SCHEME_ARRAY[i].name.c_str(), 
						(i == 0) ? "default" : "",
						(Window::seletected_color_scheme == i)
				)) {
					Window::seletected_color_scheme = i;
				}
		    }
		    ImGui::EndMenu();
		}

		if (ImGui::MenuItem("Set instructions per second", NULL, false, true))
        {
			display_ips_config = true;
        }

		ImGui::MenuItem("Enable 0xFX1E overflow", NULL, &_chip8_ptr->_0xFX1E_overflow_enabled);
		ImGui::EndMenu();
	}
}

void MenuBar::on_menu_file_reset()
{
	_chip8_ptr->is_running = false;

	// wait for game loop thread to finish
	std::this_thread::sleep_for(std::chrono::microseconds((get_microseconds_in_second() / (_chip8_ptr->opcodes_per_second)) * 2));

	_chip8_ptr->reset();
	_chip8_ptr->is_running = true;
	_parent_window.start_game_loop();
}

void MenuBar::on_menu_file_quit()
{
	_parent_window.close();
}

void MenuBar::on_menu_state_save(int i)
{
	// pause the program and create a save state
	_chip8_ptr->is_paused = true;
	std::cout << "States -> Creating Save state " + std::to_string(i) << std::endl;

	// need to pause the program here, generate a save state, then resume the application
	auto utc_timestamp = utc_time_in_seconds();
	_chip8_ptr->save_program_state(i, utc_timestamp);
}

void MenuBar::on_menu_state_load(int i)
{
	std::cout << "States -> Load state " + std::to_string(i) << std::endl;
	_chip8_ptr->load_program_state(_program_name + std::to_string(i));
}

void MenuBar::on_menu_update_resolution(int i)
{
	float menu_bar_height = ImGui::GetFrameHeight();
	SDL_SetWindowSize(_parent_window.window_ptr, Chip8::native_width * i, menu_bar_height + (Chip8::native_height * i));

	selected_resolution_multiplier = i;
}

void MenuBar::display_ips_configure_window()
{
	if (!display_ips_config) {
		return;
	}

	if (ImGui::Begin("Configure IPS", (bool*)nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse))
	{
		ImGui::Text("Type in a value between 1 and 2000");

		static char IPS_char_buff[5] = "700"; ImGui::InputText("IPS", IPS_char_buff, 32, ImGuiInputTextFlags_CharsDecimal);
		if (ImGui::Button("Submit", ImVec2(50,50))) {
			auto new_IPS = atoi(IPS_char_buff);

			if (0 < new_IPS <= 2000) {
				_chip8_ptr->opcodes_per_second = new_IPS;
				display_ips_config = false;
			}
		}

		ImGui::End();
	}
}

void MenuBar::display_file_load_window()
{
	// If the file dialog is open, display it
    if (fileDialog.Display("ChooseFileDlgKey")) {
        // If the user selects a file, get the result
        if (fileDialog.IsOk()) {
            std::string filePathName = fileDialog.GetFilePathName();
            std::string filePath = fileDialog.GetCurrentPath();

            // You can now use the selected file path here
            printf("Selected file: %s\n", filePathName.c_str());
			SDL_SetWindowTitle(_parent_window.window_ptr, (_chip8_ptr->DEFAULT_TITLE + " - " + fileDialog.GetCurrentFileName()).c_str());

			_chip8_ptr->is_running = _chip8_ptr->load_program(filePathName);

			if (_chip8_ptr->is_running) {
				_program_name = fileDialog.GetCurrentFileName();
			}

			_parent_window.start_game_loop();
        }

        // Close the dialog after processing
        fileDialog.Close();
    }
}