#include "music_player_resources.h"

#include <map>

#include <bit7zlibrary.hpp>
#include <bitextractor.hpp>
#include <bitformat.hpp>
#include <bitarchiveinfo.hpp>
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


		const std::string textureEnding("assets\\textures.jkdm");

		b8 CheckTextureError();
		void ExtractAndLoadTex(GLuint* tex, u32 name, bit7z::BitExtractor *extractor);
	}


	void Resources::Init()
	{


		std::string dir = Strings::_CURRENT_DIRECTORY_PATH + textureEnding;

		bit7z::Bit7zLibrary lib(L"7z.dll");
		bit7z::BitArchiveInfo info(lib, utf8_to_utf16(dir), bit7z::BitFormat::Zip);

		std::map<std::string, u32> files;
		auto arc_items = info.items();
		for (auto& item : arc_items)
		{
			files.insert(std::pair<std::string, u32>(utf16_to_utf8(item.name()), item.index()));
			/*std::string path = "E:\\SDL Projects\\demp\\demp\\assets\\" + utf16_to_utf8(item.name());
			path = path.substr(0, path.length() - 5);
			path += "png";
			std::vector<bit7z::byte_t> buffer;
			extractor.extract(utf8_to_utf16(dir), buffer, item.index());
			std::ofstream file(utf8_to_utf16(path), std::ofstream::out | std::ofstream::binary);
			file.write((char*)buffer.data(), buffer.size());
			file.close();*/
		}

		bit7z::BitExtractor extractor(lib, bit7z::BitFormat::Zip);

		ExtractAndLoadTex(&main_background,					files["main.png"], &extractor);
		ExtractAndLoadTex(&main_foreground,					files["main.png"], &extractor);
		ExtractAndLoadTex(&ui_buttons_background,			files["ui_buttons_background.png"], &extractor);
		ExtractAndLoadTex(&ui_buttons_background_left,		files["ui_buttons_background_left.png"], &extractor);
		ExtractAndLoadTex(&exit_background,					files["exit_background.png"], &extractor);
		ExtractAndLoadTex(&stay_on_top_background,			files["stay_on_top_background.png"], &extractor);
		ExtractAndLoadTex(&minimize_background,				files["minimize_background.png"], &extractor);
		ExtractAndLoadTex(&settings_background,				files["settings_background.png"], &extractor);
		ExtractAndLoadTex(&exit_icon,						files["exit_icon.png"], &extractor);
		ExtractAndLoadTex(&stay_on_top_icon,				files["stay_on_top_icon.png"], &extractor);
		ExtractAndLoadTex(&minimize_icon,					files["minimize_icon.png"], &extractor);
		ExtractAndLoadTex(&settings_icon,					files["settings_icon.png"], &extractor);
		ExtractAndLoadTex(&exit_background_glow,			files["exit_icon_glow.png"], &extractor);
		ExtractAndLoadTex(&minimize_background_glow,		files["minimize_icon_glow.png"], &extractor);
		ExtractAndLoadTex(&stay_on_top_background_glow,		files["stay_on_top_icon_glow.png"], &extractor);
		ExtractAndLoadTex(&settings_background_glow,		files["settings_icon_glow.png"], &extractor);
		ExtractAndLoadTex(&volume_bar,						files["volume_bar.png"], &extractor);
		ExtractAndLoadTex(&volume_speaker,					files["volume_speaker.png"], &extractor);
		ExtractAndLoadTex(&volume_speaker_muted,			files["volume_speaker_muted.png"], &extractor);
		ExtractAndLoadTex(&volume_speaker_low,				files["volume_speaker_low.png"], &extractor);
		ExtractAndLoadTex(&volume_speaker_medium,			files["volume_speaker_medium.png"], &extractor);
		ExtractAndLoadTex(&play_button,						files["play_button.png"], &extractor);
		ExtractAndLoadTex(&stop_button,						files["stop_button.png"], &extractor);
		ExtractAndLoadTex(&next_button,						files["next_button.png"], &extractor);
		ExtractAndLoadTex(&previous_button,					files["previous_button.png"], &extractor);
		ExtractAndLoadTex(&shuffle_button,					files["shuffle_button.png"], &extractor);
		ExtractAndLoadTex(&repeat_button,					files["repeat_button.png"], &extractor);
		ExtractAndLoadTex(&dot_icon,						files["dot_button_state.png"], &extractor);
		ExtractAndLoadTex(&playlist_button,					files["playlist_button.png"], &extractor);
		ExtractAndLoadTex(&playlist_add_file,				files["playlist_add.png"], &extractor);
		ExtractAndLoadTex(&music_progress_bar,				files["music_progress_bar.png"], &extractor);
		ExtractAndLoadTex(&playlist_add_file_icon,			files["playlist_add_file_icon.png"], &extractor);
		ExtractAndLoadTex(&playlist_add_folder_icon,		files["playlist_add_folder_icon.png"], &extractor);
		ExtractAndLoadTex(&playlist_add_textbox_background, files["playlist_add_files_textbox.png"], &extractor);
		ExtractAndLoadTex(&playlist_add_textbox_select,		files["playlist_add_select_background.png"], &extractor);
		ExtractAndLoadTex(&interface_button_slider,			files["switch.png"], &extractor);



		/*main_background = mdLoadTexture(texDir + "textures\\main.style");
		main_foreground = mdLoadTexture(texDir + "textures\\main.style");

		ui_buttons_background = mdLoadTexture(texDir + "textures\\ui_buttons_background.style");
		ui_buttons_background_left = mdLoadTexture(texDir + "textures\\ui_buttons_background_left.style");
		exit_background = mdLoadTexture(texDir + "textures\\exit_background.style");
		stay_on_top_background = mdLoadTexture(texDir + "textures\\stay_on_top_background.style");
		minimize_background = mdLoadTexture(texDir + "textures\\minimize_background.style");
		settings_background = mdLoadTexture(texDir + "textures\\settings_background.style");
		exit_icon = mdLoadTexture(texDir + "textures\\exit_icon.style");
		stay_on_top_icon = mdLoadTexture(texDir + "textures\\stay_on_top_icon.style");
		minimize_icon = mdLoadTexture(texDir + "textures\\minimize_icon.style");
		settings_icon = mdLoadTexture(texDir + "textures\\settings_icon.style");
		exit_background_glow = mdLoadTexture(texDir + "textures\\exit_icon_glow.style");
		minimize_background_glow = mdLoadTexture(texDir + "textures\\minimize_icon_glow.style");
		stay_on_top_background_glow = mdLoadTexture(texDir + "textures\\stay_on_top_icon_glow.style");
		settings_background_glow = mdLoadTexture(texDir + "textures\\settings_icon_glow.style");


		volume_bar = mdLoadTexture(texDir + "textures\\volume_bar.style");
		volume_speaker = mdLoadTexture(texDir + "textures\\volume_speaker.style");
		volume_speaker_muted = mdLoadTexture(texDir + "textures\\volume_speaker_muted.style");
		volume_speaker_low = mdLoadTexture(texDir + "textures\\volume_speaker_low.style");
		volume_speaker_medium = mdLoadTexture(texDir + "textures\\volume_speaker_medium.style");;


		play_button = mdLoadTexture(texDir + "textures\\play_button.style");
		stop_button = mdLoadTexture(texDir + "textures\\stop_button.style");
		next_button = mdLoadTexture(texDir + "textures\\next_button.style");
		previous_button = mdLoadTexture(texDir + "textures\\previous_button.style");
		shuffle_button = mdLoadTexture(texDir + "textures\\shuffle_button.style");
		repeat_button = mdLoadTexture(texDir + "textures\\repeat_button.style");
		dot_icon = mdLoadTexture(texDir + "textures\\dot_button_state.style");
		playlist_button = mdLoadTexture(texDir + "textures\\playlist_button.style");
		playlist_add_file = mdLoadTexture(texDir + "textures\\playlist_add.style");

		music_progress_bar = mdLoadTexture(texDir + "textures\\music_progress_bar.style");


		playlist_add_file_icon = mdLoadTexture(texDir + "textures\\playlist_add_file_icon.style");;
		playlist_add_folder_icon = mdLoadTexture(texDir + "textures\\playlist_add_folder_icon.style");
		playlist_add_textbox_background = mdLoadTexture(texDir + "textures\\playlist_add_files_textbox.style");
		playlist_add_textbox_select = mdLoadTexture(texDir + "textures\\playlist_add_select_background.style");

		interface_button_slider = mdLoadTexture(texDir + "textures\\switch.style");*/

		/*if (fs::exists(texturesPath))
			fs::remove_all(texturesPath);*/

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
			playlist_add_textbox_background && playlist_add_textbox_select && interface_button_slider)
		{
			return false;
		}

		return true;
	}

	void Resources::ExtractAndLoadTex(GLuint* tex, u32 index, bit7z::BitExtractor *extractor)
	{
		std::string dir = Strings::_CURRENT_DIRECTORY_PATH + textureEnding;
		std::vector<unsigned char> buffer;
		extractor->extract(utf8_to_utf16(dir), buffer, index);

		*tex = mdLoadTexture(&buffer[0], buffer.size());
	}

}