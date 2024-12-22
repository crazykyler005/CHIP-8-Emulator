#include "menubar.hpp"
#include "helper_functions.hpp"
#include "window.hpp"

#include <cstdio>
#include <iostream>

static bool display_ips_config = false;

MenuBar::MenuBar(std::shared_ptr<Chip8Interpreter> chip8_pointer, Window& parent_window)
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
		add_intrepreter_menu();

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
		ImGui::MenuItem("Pause", "Ctrl-p", &_chip8_ptr->is_paused);
		ImGui::MenuItem("Display wait quirk", "", &_chip8_ptr->wait_for_display_update);
		ImGui::MenuItem("Run single instruction", "Ctrl-RIGHT", 
			&_parent_window.is_run_one_instruction, 
			(_chip8_ptr->is_paused && _chip8_ptr->is_running)
		);

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

		if (ImGui::MenuItem("Set instructions per second", NULL, false, true))
        {
			display_ips_config = true;
        }

		if (ImGui::BeginMenu("Colors Schemes"))
		{
		    float sz = ImGui::GetTextLineHeight();
		    for (int i = 0; i < Window::COLOR_SCHEME_ARRAY.size(); i++)
		    {
		        ImVec2 fg_c_pos = ImGui::GetCursorScreenPos();
				ImVec2 unselect_c_pos = {fg_c_pos.x + sz + 3, fg_c_pos.y};

		        ImGui::GetWindowDrawList()->AddRectFilled(fg_c_pos, ImVec2(fg_c_pos.x + sz, fg_c_pos.y + sz), Window::COLOR_SCHEME_ARRAY[i].foreground_color);

				if (_chip8_ptr->get_type() == Chip8Type::XO) {
					ImVec2 intersect_c_pos = { fg_c_pos.x + sz + 3, fg_c_pos.y };
					ImVec2 bg_c_pos = { intersect_c_pos.x + sz + 3, intersect_c_pos.y };
					unselect_c_pos = { bg_c_pos.x + sz + 3, bg_c_pos.y };

					ImGui::GetWindowDrawList()->AddRectFilled(intersect_c_pos, ImVec2(intersect_c_pos.x + sz, intersect_c_pos.y + sz), Window::COLOR_SCHEME_ARRAY[i].intersection_color);
					ImGui::GetWindowDrawList()->AddRectFilled(bg_c_pos, ImVec2(bg_c_pos.x + sz, bg_c_pos.y + sz), Window::COLOR_SCHEME_ARRAY[i].background_color);
					ImGui::GetWindowDrawList()->AddRectFilled(unselect_c_pos, ImVec2(unselect_c_pos.x + sz, unselect_c_pos.y + sz), Window::COLOR_SCHEME_ARRAY[i].unselected_plane_color);

					ImGui::Dummy(ImVec2((sz * 4) + 6, sz));
				} else {

					ImGui::GetWindowDrawList()->AddRectFilled(unselect_c_pos, ImVec2(unselect_c_pos.x + sz, unselect_c_pos.y + sz), Window::COLOR_SCHEME_ARRAY[i].unselected_plane_color);
					ImGui::Dummy(ImVec2(sz * 2, sz));
				}

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

		ImGui::EndMenu();
	}
}

void MenuBar::add_intrepreter_menu()
{
	if (ImGui::BeginMenu("Intrepreters"))
	{
		static auto selected_type = Chip8Type::END;
		auto& type = _chip8_ptr->get_type();

		if (ImGui::MenuItem("Chip 8", "", false, false)) {}

		if (ImGui::MenuItem("COSMAC VIP", "Original", type == Chip8Type::ORIGINAL)) {
			selected_type = Chip8Type::ORIGINAL;
		}

		if (ImGui::MenuItem("COMMODORE AMIGA", "", type == Chip8Type::AMIGA_CHIP8)) {
			selected_type = Chip8Type::AMIGA_CHIP8;
		}

		ImGui::Separator();

		if (ImGui::MenuItem("Super Chip", "", false, false)) {}

		if (ImGui::MenuItem("VERSION 1.0", "", type == Chip8Type::SUPER_1p0)) {
			selected_type = Chip8Type::SUPER_1p0;
		}
		if (ImGui::MenuItem("VERSION 1.1", "Legacy", type == Chip8Type::SUPER_1p1)) {
			selected_type = Chip8Type::SUPER_1p1;
		}
		if (ImGui::MenuItem("MODERN", "", type == Chip8Type::SUPER_MODERN)) {
			selected_type = Chip8Type::SUPER_MODERN;
		}

		ImGui::Separator();

		if (ImGui::MenuItem("XO Chip", "", type == Chip8Type::XO)) {
			selected_type = Chip8Type::XO;
		}

		if (selected_type != Chip8Type::END) {
			if (!_chip8_ptr->switch_type(selected_type)) {
				printf("Invalid type conversion. Switching interpreter\n");
				_parent_window.switch_interpreter(selected_type);
			}

			selected_type = Chip8Type::END;
		}

		ImGui::EndMenu();
	}
}

void MenuBar::on_menu_file_reset()
{
	_parent_window.stop_game_loop();
	_chip8_ptr->reset();
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
	_chip8_ptr->save_program_state(_program_name, i, utc_timestamp);
}

void MenuBar::on_menu_state_load(int i)
{
	std::cout << "States -> Load state " + std::to_string(i) << std::endl;
	_chip8_ptr->load_program_state(_program_name + std::to_string(i));
}

void MenuBar::on_menu_update_resolution(int i)
{
	float menu_bar_height = ImGui::GetFrameHeight();
	SDL_SetWindowSize(_parent_window.window_ptr, _chip8_ptr->native_width * i, menu_bar_height + (_chip8_ptr->native_height * i));

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

			if (_chip8_ptr->load_program(filePathName)) {
				_program_name = fileDialog.GetCurrentFileName();
				SDL_SetWindowTitle(_parent_window.window_ptr, (_chip8_ptr->INTERPRETER_NAME + "Emulator - " + fileDialog.GetCurrentFileName()).c_str());

				if (_chip8_ptr->is_running) {
					_parent_window.stop_game_loop();
					_chip8_ptr->reset();
				}

				_parent_window.start_game_loop();
			}
        }

        // Close the dialog after processing
        fileDialog.Close();
    }
}