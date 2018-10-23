#pragma once
#ifndef MUSIC_PLAYER_SETTINGS_H
#define MUSIC_PLAYER_SETTINGS_H

#include <vector>
#include <thread>

#include <glm.hpp>

#include "SDL_ttf.h"

#include "../utility/md_types.h"

/* Max size of song that can be loaded into ram IN BYTES */
//#define _MAX_SIZE_RAM_LOADED 50000000
#define MAX_EXTENSION_LENGTH 4
#define MAX_FILEPATH_BUFFER_SIZE 65536
#define MAX_FOLDERPATH_BUFFER_SIZE 512

#define MAX_ALBUM_IMAGE_SIZE 800000

#define LAST_EVENT_TIME 50

const std::thread::id MAIN_THREAD_ID = std::this_thread::get_id();

namespace mdEngine
{
namespace MP
{
	namespace Data
	{
		static f32 mdDefaultWidth = 500.f;
		static f32 mdDefaultHeight = 320.f;

		
		const s32 _WINDOW_TEXTBOXES_OFFSET = 150;

		const glm::vec2 _MUSIC_INFO_WINDOW_SIZE = glm::vec2(370, 147);

		const s32 _SCREEN_FPS = 30;
		extern s32 _SCREEN_TICK_PER_FRAME;

		const s32 _PLAYLIST_CHOOSE_ITEM_DELAY = 300;

		const s32 _PLAYLIST_ARROWS_SCROLL_DELAY = 500;
		const s32 _PLAYLIST_ARROWS_SCROLL_INTERVAL = 15;

		const glm::vec2 _MIN_PLAYER_SIZE = glm::vec2(500.f, 500.f);

		const glm::vec2 _DEFAULT_PLAYER_POS = glm::vec2(0.f, 0.f);
		const glm::vec2 _DEFAULT_PLAYER_SIZE = glm::vec2(500.f, mdDefaultHeight);

		const glm::vec2 _DEFAULT_WINDOW_POS = glm::vec2(0.f, 0.f);;
		const glm::vec2 _DEFAULT_WINDOW_SIZE = glm::vec2(mdDefaultWidth, 700.f);

		const glm::vec2 _OPTIONS_WINDOW_SIZE = glm::vec2(400, 350);

		const glm::vec2 _MAIN_BACKGROUND_POS = glm::vec2(0.f, 0.f);;
		extern glm::vec2 _MAIN_BACKGROUND_SIZE;

		const glm::vec2 _MAIN_FOREGROUND_POS = glm::vec2(20.0f, 30.0f);
		const glm::vec2 _MAIN_FOREGROUND_SIZE = glm::vec2(mdDefaultWidth - 40.f, mdDefaultHeight - 40.f);

		const glm::vec2 _PLAYLIST_FOREGROUND_POS = glm::vec2(20.f, mdDefaultHeight + 20.f);;
		extern glm::vec2 _PLAYLIST_FOREGROUND_SIZE;

		static s16 musicUIOffsetX = -60;
		static s16 musicUIOffsetY = 10;
		static s16 musicProgressBarOffsetY = 8;

		const glm::vec2 _VOLUME_BAR_POS = glm::vec2(mdDefaultWidth / 2.f - 170.f, mdDefaultHeight - 60.f - musicUIOffsetY);;
		const glm::vec2 _VOLUME_BAR_SIZE = glm::vec2(90.f, 4.f);;

		const glm::vec2 _MUSIC_PROGRESS_BAR_POS = glm::vec2(mdDefaultWidth / 2.f - 175.f, mdDefaultHeight - musicUIOffsetY - musicProgressBarOffsetY);
		const glm::vec2 _MUSIC_PROGRESS_BAR_SIZE = glm::vec2(mdDefaultWidth / 2.f + 100.f, 5.f);

		const glm::vec2 _VOLUME_SPEAKER_POS = glm::vec2(mdDefaultWidth / 2.f - 200.f, mdDefaultHeight - 66.f - musicUIOffsetY);
		const glm::vec2 _VOLUME_SPEAKER_SIZE = glm::vec2(15.f, 15.f);;

		const glm::vec2 _MUSIC_PROGRESS_BAR_DOT_POS = glm::vec2(_MUSIC_PROGRESS_BAR_POS.x, _MUSIC_PROGRESS_BAR_POS.y - 2.f);;
		const glm::vec2 _VOLUME_BAR_DOT_POS = glm::vec2(mdDefaultWidth / 2.f - 130.f, mdDefaultHeight - 63.f - musicUIOffsetY);;
		const glm::vec2 _SLIDER_DOT_SIZE = glm::vec2(9.f, 9.f);

		static f32 ui_buttons_scale = 0.9f;
		static f32 offsetY = 20.f * ui_buttons_scale;
		const glm::vec2 _UI_BUTTONS_BACKGROUND_RIGHT_SIZE = glm::vec2(73.2f, 19.8f) * ui_buttons_scale;
		const glm::vec2 _UI_BUTTONS_BACKGROUND_RIGHT_POS = glm::vec2(mdDefaultWidth - _UI_BUTTONS_BACKGROUND_RIGHT_SIZE.x - offsetY, 5.f);

		const glm::vec2 _UI_BUTTONS_BACKGROUND_LEFT_POS = glm::vec2(offsetY, 5.f);
		const glm::vec2 _UI_BUTTONS_BACKGROUND_LEFT_SIZE = glm::vec2(28.2f, 19.8f) * ui_buttons_scale;
;

		extern glm::vec2 _UI_WINDOW_BAR_POS;
		extern glm::vec2 _UI_WINDOW_BAR_SIZE;

		static glm::vec2 backgroundSize = glm::vec2(21.f, 15.6f) * ui_buttons_scale;
		static f32 offsetBackgroundX = 3.f * ui_buttons_scale;
		const glm::vec2 _EXIT_BUTTON_BACKGROUND_SIZE = backgroundSize;
		const glm::vec2 _EXIT_BUTTON_BACKGROUND_POS = glm::vec2(_UI_BUTTONS_BACKGROUND_RIGHT_POS.x + _UI_BUTTONS_BACKGROUND_RIGHT_SIZE.x - _EXIT_BUTTON_BACKGROUND_SIZE.x - offsetBackgroundX,
																_UI_BUTTONS_BACKGROUND_RIGHT_POS.y + (_UI_BUTTONS_BACKGROUND_RIGHT_SIZE.y - _EXIT_BUTTON_BACKGROUND_SIZE.y) / 2.f);

		static f32 stayOnTopOffsetX = 0.05f * ui_buttons_scale;
		const glm::vec2 _STAY_ON_TOP_BUTTON_BACKGROUND_SIZE = backgroundSize;
		const glm::vec2 _STAY_ON_TOP_BUTTON_BACKGROUND_POS = glm::vec2(_UI_BUTTONS_BACKGROUND_RIGHT_POS.x + (_UI_BUTTONS_BACKGROUND_RIGHT_SIZE.x - _STAY_ON_TOP_BUTTON_BACKGROUND_SIZE.x) / 2.f - stayOnTopOffsetX,
																	   _UI_BUTTONS_BACKGROUND_RIGHT_POS.y + (_UI_BUTTONS_BACKGROUND_RIGHT_SIZE.y - _STAY_ON_TOP_BUTTON_BACKGROUND_SIZE.y) / 2.f);

		const glm::vec2 _MINIMIZE_BUTTON_BACKGROUND_SIZE = backgroundSize;
		const glm::vec2 _MINIMIZE_BUTTON_BACKGROUND_POS = glm::vec2(_UI_BUTTONS_BACKGROUND_RIGHT_POS.x + offsetBackgroundX,
																	_UI_BUTTONS_BACKGROUND_RIGHT_POS.y + (_UI_BUTTONS_BACKGROUND_RIGHT_SIZE.y - _EXIT_BUTTON_BACKGROUND_SIZE.y) / 2.f);

		static glm::vec2 settingsBackgroundSize = glm::vec2(17.f, 15.6f) * ui_buttons_scale;
		const glm::vec2 _SETTINGS_BUTTON_BACKGROUND_SIZE = glm::vec2(settingsBackgroundSize);
		const glm::vec2 _SETTINGS_BUTTON_BACKGROUND_POS = glm::vec2(_UI_BUTTONS_BACKGROUND_LEFT_POS.x + (_UI_BUTTONS_BACKGROUND_LEFT_SIZE.x - _SETTINGS_BUTTON_BACKGROUND_SIZE.x) / 2.f, 
																	_UI_BUTTONS_BACKGROUND_LEFT_POS.y + (_UI_BUTTONS_BACKGROUND_LEFT_SIZE.y - _SETTINGS_BUTTON_BACKGROUND_SIZE.y) / 2.f);


		const f32 _UI_BUTTONS_GLOW_SCALE = 1.5f;
		
		static f32 buttonsSize = 10.4f * ui_buttons_scale;
		const glm::vec2 _EXIT_BUTTON_SIZE = glm::vec2(buttonsSize);
		const glm::vec2 _EXIT_BUTTON_POS = glm::vec2(_EXIT_BUTTON_BACKGROUND_POS.x + (_EXIT_BUTTON_BACKGROUND_SIZE.x - _EXIT_BUTTON_SIZE.x) / 2.f,
													 _EXIT_BUTTON_BACKGROUND_POS.y + (_EXIT_BUTTON_BACKGROUND_SIZE.y - _EXIT_BUTTON_SIZE.y) / 2.f);

		const glm::vec2 _STAY_ON_TOP_BUTTON_SIZE = glm::vec2(buttonsSize);
		const glm::vec2 _STAY_ON_TOP_BUTTON_POS = glm::vec2(_UI_BUTTONS_BACKGROUND_RIGHT_POS.x + _UI_BUTTONS_BACKGROUND_RIGHT_SIZE.x / 2.f - _STAY_ON_TOP_BUTTON_SIZE.x / 2.f,
															_STAY_ON_TOP_BUTTON_BACKGROUND_POS.y + (_STAY_ON_TOP_BUTTON_BACKGROUND_SIZE.y - _STAY_ON_TOP_BUTTON_SIZE.y) / 2.f);

		const glm::vec2 _MINIMIZE_BUTTON_SIZE = glm::vec2(buttonsSize);
		static s32 minimizeOffsetY = 1;
		static s32 minimizeOffsetX = 1;
		const glm::vec2 _MINIMIZE_BUTTON_POS = glm::vec2(_MINIMIZE_BUTTON_BACKGROUND_POS.x + (_MINIMIZE_BUTTON_BACKGROUND_SIZE.x -_MINIMIZE_BUTTON_SIZE.x) / 2.f + minimizeOffsetX,
														 _MINIMIZE_BUTTON_BACKGROUND_POS.y + (_MINIMIZE_BUTTON_BACKGROUND_SIZE.y - _MINIMIZE_BUTTON_SIZE.y) / 2.f - minimizeOffsetY);



		const glm::vec2 _SETTINGS_BUTTON_SIZE = glm::vec2(buttonsSize);
		const glm::vec2 _SETTINGS_BUTTON_POS = glm::vec2(Data::_SETTINGS_BUTTON_BACKGROUND_POS.x + (Data::_SETTINGS_BUTTON_BACKGROUND_SIZE.x - Data::_SETTINGS_BUTTON_SIZE.x) / 2.f,
														 Data::_SETTINGS_BUTTON_BACKGROUND_POS.y + (Data::_SETTINGS_BUTTON_BACKGROUND_SIZE.y - Data::_SETTINGS_BUTTON_SIZE.y) / 2.f);


		const glm::vec2 _PLAY_BUTTON_POS = glm::vec2(mdDefaultWidth / 2.f - 20.f - musicUIOffsetX, mdDefaultHeight - 80.f - musicUIOffsetY);;
		const glm::vec2 _PLAY_BUTTON_SIZE = glm::vec2(40.f, 40.f);

		const glm::vec2 _NEXT_BUTTON_POS = glm::vec2(mdDefaultWidth / 2.f + 40.f - musicUIOffsetX, mdDefaultHeight - 67.f - musicUIOffsetY);;
		const glm::vec2 _NEXT_BUTTON_SIZE = glm::vec2(15.f, 15.f);

		const glm::vec2 _PREVIOUS_BUTTON_POS = glm::vec2(mdDefaultWidth / 2.f - 60.f - musicUIOffsetX, mdDefaultHeight - 67.f - musicUIOffsetY);;
		const glm::vec2 _PREVIOUS_BUTTON_SIZE = glm::vec2(15.f, 15.f);

		const glm::vec2 _SHUFFLE_BUTTON_POS = glm::vec2(mdDefaultWidth / 2.f - 110.f - musicUIOffsetX, mdDefaultHeight - 65.f - musicUIOffsetY);;
		const glm::vec2 _SHUFFLE_BUTTON_SIZE = glm::vec2(20.f, 12.f);

		const glm::vec2 _REPEAT_BUTTON_POS = glm::vec2(mdDefaultWidth / 2.f + 90.f - musicUIOffsetX, mdDefaultHeight - 65.f - musicUIOffsetY);;
		const glm::vec2 _REPEAT_BUTTON_SIZE = glm::vec2(20.f, 12.f);

		const glm::vec2 _DOT_BUTTON_STATE_SIZE = glm::vec2(5.f);

		const glm::vec2 _PLAYLIST_BUTTON_POS = glm::vec2(mdDefaultWidth - 40.f, mdDefaultHeight - 40.f - musicUIOffsetY);;
		const glm::vec2 _PLAYLIST_BUTTON_SIZE = glm::vec2(15.f, 15.f);

		extern glm::vec2 _PLAYLIST_ITEMS_SURFACE_POS;
		extern glm::vec2 _PLAYLIST_ITEMS_SURFACE_SIZE;

		const glm::vec2 _PLAYLIST_ITEM_SIZE = glm::vec2(430.f, 30.f);;

		const glm::vec2 _PLAYLIST_SEPARATOR_POS_OFFSET = glm::vec2(10.f, 0.f);
		const glm::vec2 _PLAYLIST_SEPARATOR_SIZE = glm::vec2(_PLAYLIST_ITEM_SIZE.x + _PLAYLIST_SEPARATOR_POS_OFFSET.x, 20.f);

		const glm::vec2 _PLAYLIST_SCROLL_BAR_SIZE = glm::vec2(20.f, 20.f);;
		const glm::vec2 _PLAYLIST_SCROLL_BAR_POS = glm::vec2(mdDefaultWidth - 20.f - _PLAYLIST_SCROLL_BAR_SIZE.x, 
															 _PLAYLIST_FOREGROUND_POS.y);

		extern glm::vec2 _PLAYLIST_ADD_BUTTON_POS;
		extern glm::vec2 _PLAYLIST_ADD_BUTTON_SIZE;

		extern glm::vec2 _PLAYLIST_ADD_BUTTON_TEXTBOX_POS;
		const glm::vec2 _PLAYLIST_ADD_BUTTON_TEXTBOX_SIZE = glm::vec2(120.f, 60);

		const glm::vec2 _PLAYLIST_ITEM_TEXTBOX_SIZE = glm::vec2(180.f, 80.f);

		const s16 _TEXT_BOX_ITEM_HEIGHT = 20;

		extern glm::vec2 _PLAYLIST_ADD_ICON_POS;
		const glm::vec2 _PLAYLIST_ADD_ICON_SIZE = glm::vec2(_TEXT_BOX_ITEM_HEIGHT - 4);

		const glm::vec2 _ALBUM_COVER_IMAGE_SIZE = glm::vec2(270, 150);
		//const glm::vec2 _ALBUM_COVER_IMAGE_POS = glm::vec2((mdDefaultWidth - _ALBUM_COVER_IMAGE_SIZE.x) / 2.f, 70.f);
		const glm::vec2 _ALBUM_COVER_IMAGE_POS = glm::vec2(_PLAY_BUTTON_POS.x + _PLAY_BUTTON_SIZE.x / 2.f - _ALBUM_COVER_IMAGE_SIZE.x / 2.f, 70.f);

		const glm::vec2 _MUSIC_TIME_PROGRESS_POS = glm::vec2(_ALBUM_COVER_IMAGE_POS.x + _MAIN_FOREGROUND_POS.x, _ALBUM_COVER_IMAGE_POS.y + _ALBUM_COVER_IMAGE_SIZE.y / 2.f);


		static f32 textOffsetY = 5.f;
		const glm::vec2 _TEXT_ITEMS_DURATION_POS = glm::vec2(20, mdDefaultHeight + textOffsetY);

		const glm::vec2 _TEXT_ITEMS_SIZE_POS = glm::vec2(170, mdDefaultHeight + textOffsetY);

		const glm::vec2 _TEXT_ITEMS_COUNT_POS = glm::vec2(120, mdDefaultHeight + textOffsetY);

		const glm::vec2 _TEXT_LOADED_ITEMS_COUNT_POS = glm::vec2(400, mdDefaultHeight + textOffsetY);

		const glm::vec2 _TEXT_MUSIC_TITLE_SCROLL_POS = glm::vec2(80, 50);

		extern s32 _MAX_SIZE_RAM_LOADED;

		const s32 StartMusicFadeTime = 1000;

		const u32 PlaylistTextBoxTime = 100;

		extern f32 VolumeLevel;
		extern f32 VolumeKeyMultiplier;
		extern s32 VolumeScrollStep;
		extern  s32 PauseFadeTime;
		extern s32 PlaylistRollMultiplier;
		extern s32 PlaylistBarMovableZoneXOffset;

		extern f32 PlaylistScrollStep;

		const s32 _MAX_SIZE_RAM_LOADED_MAX	= 300;
		const s32 VolumeScrollStepMAX		= 100;
		const s32 PauseFadeTimeMAX			= 10000;
		const s32 VolumeFadeTimeMAX			= 10000;
		const f32 PlaylistScrollStepMAX		= 150.f;

		const f32 MusicInfoScrollingSpeed		= 30.f;
		const f32 MusicInfoScrollingSpeedRewind = 120.f;
		const s32 MusicInfoScrollStopTimer		= 1200;
		const s32 MusicInfoScrollTextDistDiff	= 40;

		namespace Default
		{
			const s32 _MAX_SIZE_RAM_LOADED		= 50;
			const f32 VolumeKeyMultiplier		= 0.8f;
			const s32 VolumeScrollStep			= 2;
			const s32 PauseFadeTime				= 300;
			const s32 VolumeFadeTime			= 500;
			const s32 PlaylistRollMultiplier	= 500;
			const f32 PlaylistScrollStep		= 30.f;


		}

		const std::vector<std::string> SupportedAudioFormats =
		{
			".mp3",
			".wav",
			".wma"
		};

		const std::vector<std::string> SupportedImageFormats =
		{
			".jpeg",
			".jpg",
			".png"
		};

		void InitializeData();

		void UpdateData();

		void UpdateFPS(f32 newFPS);

		void CloseData();
	}

	namespace Settings
	{
		extern b8 IsPathExistenceCheckingEnabled;


	}


}
}


#endif // !MUSIC_PLAYER_SETTINGS_H
