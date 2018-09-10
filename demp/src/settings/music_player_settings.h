#pragma once
#ifndef MUSIC_PLAYER_SETTINGS_H
#define MUSIC_PLAYER_SETTINGS_H

#include <vector>
#include <glm.hpp>

#include "SDL_ttf.h"

#include "../utility/md_types.h"

/* Max size of song that can be loaded into ram IN BYTES */
//#define _MAX_SIZE_RAM_LOADED 50000000
#define MAX_EXTENSION_LENGTH 4
#define MAX_FILEPATH_BUFFER_SIZE 65536
#define MAX_FOLDERPATH_BUFFER_SIZE 512


#define WAIT_TIME_BEFORE_NEXT_CALL 50
#define MAX_PATH_WAIT_TIME 50
#define LAST_EVENT_TIME 50

namespace mdEngine
{
namespace MP
{
	namespace Data
	{
		static f32 mdDefaultWidth = 500.f;
		static f32 mdDefaultHeight = 350.f;


		extern u16 _PLAYLIST_CHOOSE_ITEM_DELAY;

		extern TTF_Font* _MUSIC_PLAYER_FONT;
		extern TTF_Font* _MUSIC_PLAYER_NUMBER_FONT;

		extern glm::vec2 _MIN_PLAYER_SIZE;

		extern glm::vec2 _DEFAULT_PLAYER_POS;
		extern glm::vec2 _DEFAULT_PLAYER_SIZE;

		extern glm::vec2 _DEFAULT_WINDOW_POS;
		extern glm::vec2 _DEFAULT_WINDOW_SIZE;

		const glm::vec2 _OPTIONS_WINDOW_SIZE = glm::vec2(300, 400);

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

		const glm::vec2 _UI_BUTTONS_BACKGROUND_RIGHT_POS = glm::vec2(430.f, 5.f);
		const glm::vec2 _UI_BUTTONS_BACKGROUND_RIGHT_SIZE = glm::vec2(40.f, 15.0f);

		const glm::vec2 _UI_BUTTONS_BACKGROUND_LEFT_POS = glm::vec2(30.f, 5.f);
		const glm::vec2 _UI_BUTTONS_BACKGROUND_LEFT_SIZE = glm::vec2(40.f, 15.f);

		extern glm::vec2 _UI_WINDOW_BAR_POS;
		extern glm::vec2 _UI_WINDOW_BAR_SIZE;

		const glm::vec2 _EXIT_BUTTON_POS = glm::vec2(mdDefaultWidth - 45.f, 5.f);
		const glm::vec2 _EXIT_BUTTON_SIZE = glm::vec2(15.f, 15.f);

		const glm::vec2 _MINIMIZE_BUTTON_POS = glm::vec2(mdDefaultWidth - 60.f, 5.f);
		const glm::vec2 _MINIMIZE_BUTTON_SIZE = glm::vec2(15.f, 15.f);

		const glm::vec2 _SETTINGS_BUTTON_POS = glm::vec2(30.f, 5.f);
		const glm::vec2 _SETTINGS_BUTTON_SIZE = glm::vec2(15.f, 15.f);

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

		extern glm::vec2 _PLAYLIST_SEPARATOR_POS_OFFSET;
		extern glm::vec2 _PLAYLIST_SEPARATOR_SIZE;

		extern glm::vec2 _PLAYLIST_SCROLL_BAR_POS;
		extern glm::vec2 _PLAYLIST_SCROLL_BAR_SIZE;

		extern glm::vec2 _PLAYLIST_ADD_BUTTON_POS;
		extern glm::vec2 _PLAYLIST_ADD_BUTTON_SIZE;

		extern glm::vec2 _PLAYLIST_ADD_BUTTON_TEXTBOX_POS;
		extern glm::vec2 _PLAYLIST_ADD_BUTTON_TEXTBOX_SIZE;

		extern s16 _TEXT_BOX_ITEM_HEIGHT;

		extern glm::vec2 _TEXT_ITEMS_DURATION_POS;

		extern glm::vec2 _TEXT_ITEMS_SIZE_POS;

		extern glm::vec2 _TEXT_ITEMS_COUNT_POS;

		extern glm::vec2 _TEXT_LOADED_ITEMS_COUNT_POS;

		extern s32 _MAX_SIZE_RAM_LOADED;

		const s32 StartMusicFadeTime = 1000;

		extern f32 VolumeLevel;
		extern f32 VolumeKeyMultiplier;
		extern s32 VolumeScrollStep;
		extern  s32 PauseFadeTime;
		extern s32 PlaylistRollMultiplier;
		extern s32 PlaylistBarMovableZoneXOffset;

		extern f32 PlaylistScrollStep;

		const s32 _MAX_SIZE_RAM_LOADED_MAX = 300;
		const s32 VolumeScrollStepMAX = 100.f;
		const s32 PauseFadeTimeMAX = 10000;
		const s32 VolumeFadeTimeMAX = 10000;
		const f32 PlaylistScrollStepMAX = 150.f;


		namespace Default
		{
			const s32 _MAX_SIZE_RAM_LOADED		= 50;
			const f32 VolumeKeyMultiplier		= 0.8f;
			const s32 VolumeScrollStep			= 2.f;
			const s32 PauseFadeTime				= 500;
			const s32 VolumeFadeTime			= 500;
			const s32 PlaylistRollMultiplier	= 500;
			const f32 PlaylistScrollStep		= 30.f;


		}

#ifdef _WIN32_
		const std::vector<std::wstring> SupportedFormatsW =
		{
			L".mp3",
			L".wav",
			L".wma"
		};

		const std::vector<std::string> SupportedFormats =
		{
			".mp3",
			".wav",
			".wma"
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

		void CloseData();
	}



}
}


#endif // !MUSIC_PLAYER_SETTINGS_H
