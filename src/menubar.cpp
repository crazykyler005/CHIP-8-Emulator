#include "menubar.hpp"
#include "helper_functions.hpp"
#include "window.hpp"

#include <cstdio>
#include <iostream>

static bool seletected_color_schemes[5] = { false };

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

	on_menu_file_load();
}

void MenuBar::add_file_menu()
{
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Load", "Ctrl+O")) {
			fileDialog.OpenDialog("ChooseFileDlgKey", "Choose File", ".ch8,.txt,.*");
		}

		if (ImGui::BeginMenu("Load Recent"))
		{
			ImGui::MenuItem("fish_hat.c");
			ImGui::MenuItem("fish_hat.inl");
			ImGui::MenuItem("fish_hat.h");
			ImGui::EndMenu();
		}

		if (ImGui::MenuItem("Reset", NULL, false, false)) {}

		ImGui::Separator();
		// if (ImGui::BeginMenu("Options"))
		// {
		//     static bool enabled = true;
		//     ImGui::MenuItem("Enabled", "", &enabled);
		//     ImGui::BeginChild("child", ImVec2(0, 60), ImGuiChildFlags_Borders);
		//     for (int i = 0; i < 10; i++)
		//         ImGui::Text("Scrolling Text %d", i);
		//     ImGui::EndChild();
		//     static float f = 0.5f;
		//     static int n = 0;
		//     ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
		//     ImGui::InputFloat("Input", &f, 0.1f);
		//     ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
		//     ImGui::EndMenu();
		// }

		// if (ImGui::BeginMenu("Colors"))
		// {
		//     float sz = ImGui::GetTextLineHeight();
		//     for (int i = 0; i < ImGuiCol_COUNT; i++)
		//     {
		//         const char* name = ImGui::GetStyleColorName((ImGuiCol)i);
		//         ImVec2 p = ImGui::GetCursorScreenPos();
		//         ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + sz, p.y + sz), ImGui::GetColorU32((ImGuiCol)i));
		//         ImGui::Dummy(ImVec2(sz, sz));
		//         ImGui::SameLine();
		//         ImGui::MenuItem(name);
		//     }
		//     ImGui::EndMenu();
		// }

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
		if (ImGui::BeginMenu("Save State"))
		{
			for (uint8_t i = 0; i < stateTimestamps.size(); i++) {

				// if button is selected
				if (ImGui::MenuItem(("State " + std::to_string(i)).c_str(), stateTimestamps[i].c_str())) {
					stateTimestamps[i] = get_time_str(true, utc_time_in_seconds());
				}
			}
			
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Load State"))
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
		if (ImGui::MenuItem("Play sound", NULL)) {
			_parent_window.play_a_sound();
		}
		if (ImGui::MenuItem("Pause", NULL, &_chip8_ptr->is_paused)) {
			on_menu_state_pause();
		}

		if (ImGui::BeginMenu("Resolution"))
		{
			for (int i = 4; i <= 16; i+=4) {

				auto action_name = "x" + std::to_string(i);

				if (ImGui::MenuItem((action_name).c_str(), NULL)) {
					on_menu_update_resolution(i);
				}
			}

			ImGui::EndMenu();
		}

		// generating color scheme submenu to settings menu
		if (ImGui::BeginMenu("Color Scheme"))
		{
			for (uint8_t i = 0; i < sizeof(seletected_color_schemes); i++) {

				if ( ImGui::MenuItem(("State " + std::to_string(i)).c_str(), 
						(i == 0) ? "default" : "",
						&seletected_color_schemes[i]
				)) 
				{
					seletected_color_schemes[i] = true;

					for (uint8_t k = 0; k < sizeof(seletected_color_schemes); k++) {
						if (i == k) {
							continue;
						}
						seletected_color_schemes[k] = false;
					}
				}
			}
			ImGui::EndMenu();
		}

		ImGui::MenuItem("Enable 0xFX1E overflow", NULL, &_chip8_ptr->_0xFX1E_overflow_enabled);
		ImGui::EndMenu();
	}
}

void MenuBar::on_menu_file_load()
{
	// If the file dialog is open, display it
    if (fileDialog.Display("ChooseFileDlgKey")) {
        // If the user selects a file, get the result
        if (fileDialog.IsOk()) {
            std::string filePathName = fileDialog.GetFilePathName();
            std::string filePath = fileDialog.GetCurrentPath();

            // You can now use the selected file path here
            printf("Selected file: %s\n", filePathName.c_str());
			SDL_SetWindowTitle(_parent_window.window_ptr, ("Chip-8 - " + fileDialog.GetCurrentFileName()).c_str());

			_chip8_ptr->is_running = _chip8_ptr->load_program(filePathName);
			_parent_window.start_game_loop();
        }

        // Close the dialog after processing
        fileDialog.Close();
    }
}

void MenuBar::on_menu_file_quit()
{
	_parent_window.close();
}

void MenuBar::on_menu_state_save(int i)
{
	std::cout << "States -> Save state " + std::to_string(i) << std::endl;

	// need to pause the program here, generate a save state, then resume the application
	auto utc_timestamp = utc_time_in_seconds();
	_chip8_ptr->save_program_state(i, utc_timestamp);

	// save_menu_items[i]->set_label("test");

	// TODO: figure out how to update the label of the item after it's been added to the menu
	// menu_item->set_label("Save State " + std::to_string(state_idx) + " - " + get_time_str(true, utc)).c_str());
}

void MenuBar::on_menu_state_load(int i)
{
	// _chip8_ptr->load_program_state(load_menu_items[i]->get_label());
	std::cout << "States -> Load state " + std::to_string(i) << std::endl;
}

void MenuBar::on_menu_state_pause()
{
	// if (_chip8_ptr->is_paused) {
	// 	_parent_window.run_game_loop();

	// 	return;
	// }

	std::cout << "States -> Pause Program\n";
}

void MenuBar::on_menu_update_resolution(int i)
{

	// SDL_DisplayMode dm;
	// SDL_GetCurrentDisplayMode(0, &DM);
	// auto Width = DM.w;
	// auto Height = DM.h;

	// ImGui::GetWindowSize()

	// auto current_screen_height = _parent_window.get_child()->get_height() - _height;
	// auto current_titlebar_height = _parent_window.get_height() - (current_screen_height + _height);
	// auto updated_screen_height = Chip8::native_height * i;

	// SDL_SetWindowSize(_parent_window.window_ptr, Chip8::native_width * i, updated_screen_height + _height + current_titlebar_height);
	SDL_SetWindowSize(_parent_window.window_ptr, Chip8::native_width * i, Chip8::native_width * i);
}
