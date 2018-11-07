#include "music_player_resources.h"

#include <bit7zlibrary.hpp>
#include <bitextractor.hpp>
#include <bitformat.hpp>
#include <boost/filesystem.hpp>

#include "../app/realtime_system_application.h"
#include "../settings/music_player_string.h"
#include "../utility/md_load_texture.h"
#include "../utility/utf8_to_utf16.h"


namespace fs = boost::filesystem;

namespace mdEngine
{
	namespace Resources
	{
		GLuint main_background, main_foreground;
		GLuint ui_buttons_background, ui_buttons_background_left,
			exit_icon, minimize_icon, stay_on_top_icon, settings_icon,
			exit_background, minimize_background, stay_on_top_background, settings_background,
			exit_background_glow, minimize_background_glow, stay_on_top_background_glow, settings_background_glow;

		GLuint volume_bar, volume_speaker, volume_speaker_muted, volume_speaker_low, volume_speaker_medium;
		GLuint play_button, stop_button, next_button, previous_button, shuffle_button, repeat_button, dot_icon, playlist_button,
			playlist_add_file;
		GLuint music_progress_bar;
		GLuint playlist_add_file_icon, playlist_add_folder_icon, playlist_add_textbox_background, playlist_add_textbox_select;
		GLuint playlist_textbox_texture;
		GLuint interface_button_slider;

		b8 CheckTextureError();
	}


	void Resources::Init()
	{
		bit7z::Bit7zLibrary lib(L"7zxa.dll");
		bit7z::BitExtractor extractor(lib, bit7z::BitFormat::SevenZip);

		std::string dir = Strings::_CURRENT_DIRECTORY_PATH + "assets";
		std::string assetsDir = dir + "\\textures";
		std::string assetsFileDir = dir + "\\assets.jkdm";
		extractor.extract(utf8_to_utf16(assetsFileDir), utf8_to_utf16(dir));

		main_background = mdLoadTexture(Strings::_CURRENT_DIRECTORY_PATH + "assets\\textures\\main.style");
		main_foreground = mdLoadTexture(Strings::_CURRENT_DIRECTORY_PATH + "assets\\textures\\main.style");

		ui_buttons_background = mdLoadTexture(Strings::_CURRENT_DIRECTORY_PATH + "assets\\textures\\ui_buttons_background.style");
		ui_buttons_background_left = mdLoadTexture(Strings::_CURRENT_DIRECTORY_PATH + "assets\\textures\\ui_buttons_background_left.style");
		exit_background = mdLoadTexture(Strings::_CURRENT_DIRECTORY_PATH + "assets\\textures\\exit_background.style");
		stay_on_top_background = mdLoadTexture(Strings::_CURRENT_DIRECTORY_PATH + "assets\\textures\\stay_on_top_background.style");
		minimize_background = mdLoadTexture(Strings::_CURRENT_DIRECTORY_PATH + "assets\\textures\\minimize_background.style");
		settings_background = mdLoadTexture(Strings::_CURRENT_DIRECTORY_PATH + "assets\\textures\\settings_background.style");
		exit_icon = mdLoadTexture(Strings::_CURRENT_DIRECTORY_PATH + "assets\\textures\\exit_icon.style");
		stay_on_top_icon = mdLoadTexture(Strings::_CURRENT_DIRECTORY_PATH + "assets\\textures\\stay_on_top_icon.style");
		minimize_icon = mdLoadTexture(Strings::_CURRENT_DIRECTORY_PATH + "assets\\textures\\minimize_icon.style");
		settings_icon = mdLoadTexture(Strings::_CURRENT_DIRECTORY_PATH + "assets\\textures\\settings_icon.style");
		exit_background_glow = mdLoadTexture(Strings::_CURRENT_DIRECTORY_PATH + "assets\\textures\\exit_icon_glow.style");
		minimize_background_glow = mdLoadTexture(Strings::_CURRENT_DIRECTORY_PATH + "assets\\textures\\minimize_icon_glow.style");
		stay_on_top_background_glow = mdLoadTexture(Strings::_CURRENT_DIRECTORY_PATH + "assets\\textures\\stay_on_top_icon_glow.style");
		settings_background_glow = mdLoadTexture(Strings::_CURRENT_DIRECTORY_PATH + "assets\\textures\\settings_icon_glow.style");


		volume_bar = mdLoadTexture(Strings::_CURRENT_DIRECTORY_PATH + "assets\\textures\\volume_bar.style");
		volume_speaker = mdLoadTexture(Strings::_CURRENT_DIRECTORY_PATH + "assets\\textures\\volume_speaker.style");
		volume_speaker_muted = mdLoadTexture(Strings::_CURRENT_DIRECTORY_PATH + "assets\\textures\\volume_speaker_muted.style");
		volume_speaker_low = mdLoadTexture(Strings::_CURRENT_DIRECTORY_PATH + "assets\\textures\\volume_speaker_low.style");
		volume_speaker_medium = mdLoadTexture(Strings::_CURRENT_DIRECTORY_PATH + "assets\\textures\\volume_speaker_medium.style");;


		play_button = mdLoadTexture(Strings::_CURRENT_DIRECTORY_PATH + "assets\\textures\\play_button.style");
		stop_button = mdLoadTexture(Strings::_CURRENT_DIRECTORY_PATH + "assets\\textures\\stop_button.style");
		next_button = mdLoadTexture(Strings::_CURRENT_DIRECTORY_PATH + "assets\\textures\\next_button.style");
		previous_button = mdLoadTexture(Strings::_CURRENT_DIRECTORY_PATH + "assets\\textures\\previous_button.style");
		shuffle_button = mdLoadTexture(Strings::_CURRENT_DIRECTORY_PATH + "assets\\textures\\shuffle_button.style");
		repeat_button = mdLoadTexture(Strings::_CURRENT_DIRECTORY_PATH + "assets\\textures\\repeat_button.style");
		dot_icon = mdLoadTexture(Strings::_CURRENT_DIRECTORY_PATH + "assets\\textures\\dot_button_state.style");
		playlist_button = mdLoadTexture(Strings::_CURRENT_DIRECTORY_PATH + "assets\\textures\\playlist_button.style");
		playlist_add_file = mdLoadTexture(Strings::_CURRENT_DIRECTORY_PATH + "assets\\textures\\playlist_add.style");

		music_progress_bar = mdLoadTexture(Strings::_CURRENT_DIRECTORY_PATH + "assets\\textures\\music_progress_bar.style");


		playlist_add_file_icon = mdLoadTexture(Strings::_CURRENT_DIRECTORY_PATH + "assets\\textures\\playlist_add_file_icon.style");;
		playlist_add_folder_icon = mdLoadTexture(Strings::_CURRENT_DIRECTORY_PATH + "assets\\textures\\playlist_add_folder_icon.style");
		playlist_add_textbox_background = mdLoadTexture(Strings::_CURRENT_DIRECTORY_PATH + "assets\\textures\\playlist_add_files_textbox.style");
		playlist_add_textbox_select = mdLoadTexture(Strings::_CURRENT_DIRECTORY_PATH + "assets\\textures\\playlist_add_select_background.style");

		interface_button_slider = mdLoadTexture(Strings::_CURRENT_DIRECTORY_PATH + "assets\\textures\\switch.style");

		if (fs::exists(assetsDir))
			fs::remove_all(assetsDir);

		if (CheckTextureError() == true)
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
				"Missing file",
				"File is missing. Please reinstall the program.",
				NULL);
			mdEngine::AppExit();
			return;
		}
	}

	void Resources::Free()
	{
		glDeleteTextures(1, &main_background);
		glDeleteTextures(1, &main_foreground);
		glDeleteTextures(1, &ui_buttons_background);
		glDeleteTextures(1, &ui_buttons_background_left);
		glDeleteTextures(1, &exit_background);
		glDeleteTextures(1, &stay_on_top_background);
		glDeleteTextures(1, &minimize_background);
		glDeleteTextures(1, &settings_background);
		glDeleteTextures(1, &exit_icon);
		glDeleteTextures(1, &stay_on_top_icon);
		glDeleteTextures(1, &minimize_icon);
		glDeleteTextures(1, &settings_icon);
		glDeleteTextures(1, &exit_background_glow);
		glDeleteTextures(1, &minimize_background_glow);
		glDeleteTextures(1, &stay_on_top_background_glow);
		glDeleteTextures(1, &settings_background_glow);
		glDeleteTextures(1, &volume_bar);
		glDeleteTextures(1, &volume_speaker);
		glDeleteTextures(1, &volume_speaker_muted);
		glDeleteTextures(1, &volume_speaker_low);
		glDeleteTextures(1, &volume_speaker_medium);
		glDeleteTextures(1, &play_button);
		glDeleteTextures(1, &stop_button);
		glDeleteTextures(1, &next_button);
		glDeleteTextures(1, &previous_button);
		glDeleteTextures(1, &shuffle_button);
		glDeleteTextures(1, &repeat_button);
		glDeleteTextures(1, &dot_icon);
		glDeleteTextures(1, &playlist_button);
		glDeleteTextures(1, &playlist_add_file);
		glDeleteTextures(1, &music_progress_bar);
		glDeleteTextures(1, &playlist_add_file_icon);
		glDeleteTextures(1, &playlist_add_folder_icon);;
		glDeleteTextures(1, &playlist_add_textbox_background);
		glDeleteTextures(1, &playlist_add_textbox_select);
		glDeleteTextures(1, &interface_button_slider);
	}


	b8 Resources::CheckTextureError()
	{
		if (main_background && main_foreground && ui_buttons_background && ui_buttons_background_left  &&
			exit_icon && minimize_icon && stay_on_top_icon && settings_icon &&
			exit_background && minimize_background && stay_on_top_background && settings_background &&
			exit_background_glow && minimize_background_glow && stay_on_top_background_glow &&
			settings_background_glow && volume_bar  && volume_speaker  && volume_speaker_muted  &&
			volume_speaker_low && volume_speaker_medium && play_button && stop_button && next_button &&
			previous_button && shuffle_button && repeat_button && dot_icon && playlist_button &&
			playlist_add_file  && music_progress_bar  && playlist_add_file_icon && playlist_add_folder_icon &&
			playlist_add_textbox_background && playlist_add_textbox_select)
		{
			return false;
		}

		return true;
	}

}