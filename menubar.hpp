#include <gtkmm.h>
#include <vector>


class MenuBar : public Gtk::PopoverMenuBar {

public:
	inline static uint8_t MAX_SAVE_LOAD_STATES = 5;
	inline static uint8_t MAX_RESOLUTION_MAGNIFACTION = 4;

	void generate();

private:
	void on_menu_file_load();
	void on_menu_file_quit();

	void on_menu_state_pause();
	void on_menu_state_save(int i);
	void on_menu_state_load(int i);
	void on_menu_update_resolution(int i);
	void on_file_dialog_finish(const Glib::RefPtr<Gio::AsyncResult>& result, const Glib::RefPtr<Gtk::FileDialog>& dialog);

	Glib::RefPtr<Gio::SimpleActionGroup> m_refActionGroup;

	int _native_height = 400;
	int _native_width = 400;
	std::string _default_title = "test";
	Gtk::Window* _parent_window;
};
