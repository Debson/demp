#pragma once
#ifndef MUSIC_PLAYER_SETTINGS_H
#define MUSIC_PLAYER_SETTINGS_H

#include <vector>
#include <glm.hpp>

#include "SDL_ttf.h"

#include "md_types.h"


/* Max size of song that can be loaded into ram IN BYTES */
#define _MAX_SIZE_RAM_LOADED 50000000
#define MAX_EXTENSION_LENGTH 4
#define MAX_FILEPATH_BUFFER_SIZE 65536
#define MAX_FOLDERPATH_BUFFER_SIZE 512

namespace mdEngine
{
namespace MP
{
	namespace Data
	{
		extern u16 _PLAYLIST_CHOOSE_ITEM_DELAY;

		extern TTF_Font* _MUSIC_PLAYER_FONT;

		extern glm::vec2 _MIN_PLAYER_SIZE;

		extern glm::vec2 _DEFAULT_PLAYER_POS;
		extern glm::vec2 _DEFAULT_PLAYER_SIZE;

		extern glm::vec2 _DEFAULT_WINDOW_POS;
		extern glm::vec2 _DEFAULT_WINDOW_SIZE;

		extern glm::vec2 _MAIN_BACKGROUND_POS;
		extern glm::vec2 _MAIN_BACKGROUND_SIZE;

		extern glm::vec2 _MAIN_FOREGROUND_POS;
		extern glm::vec2 _MAIN_FOREGROUND_SIZE;

		extern glm::vec2 _PLAYLIST_FOREGROUND_POS;
		extern glm::vec2 _PLAYLIST_FOREGROUND_SIZE;

		extern glm::vec2 _VOLUME_BAR_POS;
		extern glm::vec2 _VOLUME_BAR_SIZE;

		extern glm::vec2 _MUSIC_PROGRESS_BAR_POS;
		extern glm::vec2 _MUSIC_PROGRESS_BAR_SIZE;

		extern glm::vec2 _VOLUME_SPEAKER_POS;
		extern glm::vec2 _VOLUME_SPEAKER_SIZE;

		extern glm::vec2 _MUSIC_PROGRESS_BAR_DOT_POS;
		extern glm::vec2 _VOLUME_BAR_DOT_POS;
		extern glm::vec2 _SLIDER_DOT_SIZE;

		extern glm::vec2 _UI_WINDOW_BAR_POS;
		extern glm::vec2 _UI_WINDOW_BAR_SIZE;

		extern glm::vec2 _EXIT_BUTTON_POS;
		extern glm::vec2 _EXIT_BUTTON_SIZE;

		extern glm::vec2 _MINIMIZE_BUTTON_POS;
		extern glm::vec2 _MINIMIZE_BUTTON_SIZE;

		extern glm::vec2 _STAY_ON_TOP_BUTTON_POS;
		extern glm::vec2 _STAY_ON_TOP_BUTTON_SIZE;

		extern glm::vec2 _PLAY_BUTTON_POS;
		extern glm::vec2 _PLAY_BUTTON_SIZE;

		extern glm::vec2 _NEXT_BUTTON_POS;
		extern glm::vec2 _NEXT_BUTTON_SIZE;

		extern glm::vec2 _PREVIOUS_BUTTON_POS;
		extern glm::vec2 _PREVIOUS_BUTTON_SIZE;

		extern glm::vec2 _SHUFFLE_BUTTON_POS;
		extern glm::vec2 _SHUFFLE_BUTTON_SIZE;

		extern glm::vec2 _REPEAT_BUTTON_POS;
		extern glm::vec2 _REPEAT_BUTTON_SIZE;

		extern glm::vec2 _DOT_BUTTON_STATE_SIZE;

		extern glm::vec2 _PLAYLIST_BUTTON_POS;
		extern glm::vec2 _PLAYLIST_BUTTON_SIZE;

		extern glm::vec2 _PLAYLIST_ITEMS_SURFACE_POS;
		extern glm::vec2 _PLAYLIST_ITEMS_SURFACE_SIZE;

		extern glm::vec2 _PLAYLIST_ITEM_SIZE;

		extern glm::vec2 _PLAYLIST_SCROLL_BAR_POS;
		extern glm::vec2 _PLAYLIST_SCROLL_BAR_SIZE;

		extern glm::vec2 _PLAYLIST_ADD_BUTTON_POS;
		extern glm::vec2 _PLAYLIST_ADD_BUTTON_SIZE;

		extern glm::vec2 _PLAYLIST_ADD_BUTTON_TEXTBOX_POS;
		extern glm::vec2 _PLAYLIST_ADD_BUTTON_TEXTBOX_SIZE;

		extern s16 _TEXT_BOX_ITEM_HEIGHT;

		extern f32 VolumeKeyMultiplier;
		extern s32 VolumeScrollStep;
		extern s32 VolumeFadeTime;
		extern s32 PlaylistRollMultiplier;

		extern f32 PlaylistScrollStep;

#ifdef _WIN32_
		const std::vector<std::wstring> SupportedFormats =
		{
			L".mp3",
			L".wav",
			L".wma"
		};
#else
		const std::vector<const char*> SupportedFormats =
		{
			".mp3",
			".wav"
		};
#endif


		void InitializeData();

		void UpdateData();
	}



}
}


#endif // !MUSIC_PLAYER_SETTINGS_H
