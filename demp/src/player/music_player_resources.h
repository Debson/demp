#ifndef MUSIC_PLAYER_RESOURCES_H
#define MUSIC_PLAYER_RESOURCES_H

#include <GL/gl3w.h>

namespace mdEngine
{
	namespace Resources
	{
		extern GLuint main_background, main_foreground;
		extern GLuint ui_buttons_background, ui_buttons_background_left,
			exit_icon, minimize_icon, stay_on_top_icon, settings_icon,
			exit_background, minimize_background, stay_on_top_background, settings_background,
			exit_background_glow, minimize_background_glow, stay_on_top_background_glow, settings_background_glow;

		extern GLuint volume_bar, volume_speaker, volume_speaker_muted, volume_speaker_low, volume_speaker_medium;
		extern GLuint play_button, stop_button, next_button, previous_button, shuffle_button, repeat_button, repeat_dot, dot_icon, playlist_button,
			playlist_add_file;
		extern GLuint music_progress_bar;
		extern GLuint playlist_add_file_icon, playlist_add_folder_icon, playlist_add_textbox_background, playlist_add_textbox_select;
		extern GLuint playlist_textbox_texture;
		extern GLuint interface_button_slider;

		void Init();

		void Free();
	}
}
#endif // !MUSIC_PLAYER_RESOURCES_H
