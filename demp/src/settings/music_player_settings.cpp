#include "music_player_settings.h"

#include "music_player_string.h"
#include "../app/realtime_system_application.h"
#include "../utility/md_parser.h"


namespace mdEngine
{
namespace MP
{
	namespace Data
	{
		u16 _PLAYLIST_CHOOSE_ITEM_DELAY;

		TTF_Font* _MUSIC_PLAYER_FONT;
		TTF_Font* _MUSIC_PLAYER_NUMBER_FONT;

		glm::vec2 _MIN_PLAYER_SIZE;

		glm::vec2 _DEFAULT_WINDOW_POS;
		glm::vec2 _DEFAULT_WINDOW_SIZE;

		glm::vec2 _MAIN_BACKGROUND_POS;
		glm::vec2 _MAIN_BACKGROUND_SIZE;

		glm::vec2 _PLAYLIST_FOREGROUND_POS;
		glm::vec2 _PLAYLIST_FOREGROUND_SIZE;

		glm::vec2 _VOLUME_BAR_POS;
		glm::vec2 _VOLUME_BAR_SIZE;

		glm::vec2 _MUSIC_PROGRESS_BAR_POS;
		glm::vec2 _MUSIC_PROGRESS_BAR_SIZE;

		glm::vec2 _VOLUME_SPEAKER_POS;
		glm::vec2 _VOLUME_SPEAKER_SIZE;

		glm::vec2 _MUSIC_PROGRESS_BAR_DOT_POS;
		glm::vec2 _VOLUME_BAR_DOT_POS;
		glm::vec2 _SLIDER_DOT_SIZE;

		/*const glm::vec2 _UI_BUTTONS_BACKGROUND_RIGHT_POS;
		const glm::vec2 _UI_BUTTONS_BACKGROUND_RIGHT_SIZE;

		const glm::vec2 _UI_BUTTONS_BACKGROUND_LEFT_POS;
		const glm::vec2 _UI_BUTTONS_BACKGROUND_LEFT_SIZE;*/

		glm::vec2 _UI_WINDOW_BAR_POS;
		glm::vec2 _UI_WINDOW_BAR_SIZE;

		glm::vec2 _STAY_ON_TOP_BUTTON_POS;
		glm::vec2 _STAY_ON_TOP_BUTTON_SIZE;

		glm::vec2 _PLAY_BUTTON_POS;
		glm::vec2 _PLAY_BUTTON_SIZE;

		glm::vec2 _NEXT_BUTTON_POS;
		glm::vec2 _NEXT_BUTTON_SIZE;

		glm::vec2 _PREVIOUS_BUTTON_POS;
		glm::vec2 _PREVIOUS_BUTTON_SIZE;

		glm::vec2 _SHUFFLE_BUTTON_POS;
		glm::vec2 _SHUFFLE_BUTTON_SIZE;

		glm::vec2 _REPEAT_BUTTON_POS;
		glm::vec2 _REPEAT_BUTTON_SIZE;

		glm::vec2 _DOT_BUTTON_STATE_SIZE;

		glm::vec2 _PLAYLIST_BUTTON_POS;
		glm::vec2 _PLAYLIST_BUTTON_SIZE;

		glm::vec2 _PLAYLIST_ITEMS_SURFACE_POS;
		glm::vec2 _PLAYLIST_ITEMS_SURFACE_SIZE;

		glm::vec2 _PLAYLIST_ITEM_SIZE;

		glm::vec2 _PLAYLIST_SEPARATOR_POS_OFFSET;
		glm::vec2 _PLAYLIST_SEPARATOR_SIZE;

		glm::vec2 _PLAYLIST_SCROLL_BAR_POS;
		glm::vec2 _PLAYLIST_SCROLL_BAR_SIZE;

		glm::vec2 _PLAYLIST_ADD_BUTTON_POS;
		glm::vec2 _PLAYLIST_ADD_BUTTON_SIZE;

		glm::vec2 _PLAYLIST_ADD_BUTTON_TEXTBOX_POS;
		glm::vec2 _PLAYLIST_ADD_BUTTON_TEXTBOX_SIZE;


		s32 _MAX_SIZE_RAM_LOADED;

		f32 VolumeLevel;
		f32 VolumeKeyMultiplier;
		s32 VolumeScrollStep ;
		s32 PauseFadeTime;
		s32 PlaylistRollMultiplier;

		f32 PlaylistScrollStep;

		s32 PlaylistBarMovableZoneXOffset;

		// private
		f32 mdCurrentWidth;
		f32 mdCurrentHeight;
	}

	void Data::InitializeData()
	{
		_MIN_PLAYER_SIZE = glm::vec2(500.f, 500.f);

		Window::windowProperties.mApplicationHeight = Parser::GetInt(Strings::_SETTINGS_FILE, Strings::_APP_HEIGHT);
		Window::windowProperties.mStartApplicationHeight = Window::windowProperties.mApplicationHeight;
		if (Window::windowProperties.mApplicationHeight < _MIN_PLAYER_SIZE.y)
			Window::windowProperties.mApplicationHeight = _MIN_PLAYER_SIZE.y + 20.f;

		mdCurrentWidth = mdEngine::Window::windowProperties.mWindowWidth;
		mdCurrentHeight = mdEngine::Window::windowProperties.mApplicationHeight;

		s16 musicUIOffsetX = -60;
		s16 musicUIOffsetY = 10;
		s16 musicProgressBarOffsetY = 8;

		_PLAYLIST_CHOOSE_ITEM_DELAY = 300;

		_MUSIC_PLAYER_FONT = TTF_OpenFont("assets/font/Times New Roman.ttf", 14);
		_MUSIC_PLAYER_NUMBER_FONT = TTF_OpenFont("assets/font/Times New Roman.ttf", 14);

		_DEFAULT_WINDOW_POS = glm::vec2(0.f, 0.f);
		_DEFAULT_WINDOW_SIZE = glm::vec2(mdDefaultWidth, 700.f);

		_MAIN_BACKGROUND_POS = glm::vec2(0.f, 0.f);
		_MAIN_BACKGROUND_SIZE = glm::vec2(mdDefaultWidth, mdDefaultHeight);

		_PLAYLIST_FOREGROUND_POS = glm::vec2(20.f, mdDefaultHeight + 20.f);
		_PLAYLIST_FOREGROUND_SIZE = glm::vec2(mdDefaultWidth - 40.f, mdDefaultHeight - 40.f);


		/* Initialize later */

		_VOLUME_BAR_POS = glm::vec2(mdDefaultWidth / 2.f - 170.f, mdDefaultHeight - 60.f - musicUIOffsetY);
		_VOLUME_BAR_SIZE = glm::vec2(90.f, 4.f);

		_VOLUME_SPEAKER_POS = glm::vec2(mdDefaultWidth / 2.f - 200.f, mdDefaultHeight - 66.f - musicUIOffsetY);
		_VOLUME_SPEAKER_SIZE = glm::vec2(15.f, 15.f);

		_MUSIC_PROGRESS_BAR_POS = glm::vec2(mdDefaultWidth / 2.f - 175.f, mdDefaultHeight - musicUIOffsetY - musicProgressBarOffsetY);
		_MUSIC_PROGRESS_BAR_SIZE = glm::vec2(mdDefaultWidth / 2.f + 100.f, 5.f);


		_MUSIC_PROGRESS_BAR_DOT_POS = glm::vec2(_MUSIC_PROGRESS_BAR_POS.x, _MUSIC_PROGRESS_BAR_POS.y - 2.f);

		_VOLUME_BAR_DOT_POS = glm::vec2(mdDefaultWidth / 2.f - 130.f, mdDefaultHeight - 63.f - musicUIOffsetY);
		_SLIDER_DOT_SIZE = glm::vec2(9.f, 9.f);

		_UI_WINDOW_BAR_POS;
		_UI_WINDOW_BAR_SIZE;

		_MINIMIZE_BUTTON_POS;
		_MINIMIZE_BUTTON_SIZE;

		_STAY_ON_TOP_BUTTON_POS;
		_STAY_ON_TOP_BUTTON_SIZE;

		_SHUFFLE_BUTTON_POS = glm::vec2(mdDefaultWidth / 2.f - 110.f - musicUIOffsetX, mdDefaultHeight - 65.f - musicUIOffsetY);
		_SHUFFLE_BUTTON_SIZE = glm::vec2(20.f, 12.f);

		_PREVIOUS_BUTTON_POS = glm::vec2(mdDefaultWidth / 2.f - 60.f - musicUIOffsetX, mdDefaultHeight - 67.f - musicUIOffsetY);;
		_PREVIOUS_BUTTON_SIZE = glm::vec2(15.f, 15.f);

		_PLAY_BUTTON_POS = glm::vec2(mdDefaultWidth / 2.f - 20.f - musicUIOffsetX, mdDefaultHeight - 80.f - musicUIOffsetY);
		_PLAY_BUTTON_SIZE = glm::vec2(40.f, 40.f);

		_NEXT_BUTTON_POS = glm::vec2(mdDefaultWidth / 2.f + 40.f - musicUIOffsetX, mdDefaultHeight - 67.f - musicUIOffsetY);
		_NEXT_BUTTON_SIZE = glm::vec2(15.f, 15.f);

		_REPEAT_BUTTON_POS = glm::vec2(mdDefaultWidth / 2.f + 90.f - musicUIOffsetX, mdDefaultHeight - 65.f - musicUIOffsetY);;
		_REPEAT_BUTTON_SIZE = glm::vec2(20.f, 12.f);

		_DOT_BUTTON_STATE_SIZE = glm::vec2(5.f);

		_PLAYLIST_BUTTON_POS = glm::vec2(mdDefaultWidth - 40.f, mdDefaultHeight - 40.f - musicUIOffsetY);
		_PLAYLIST_BUTTON_SIZE = glm::vec2(15.f, 15.f);

		_PLAYLIST_ITEMS_SURFACE_POS = glm::vec2(mdCurrentWidth / 2.f - 150.f, _PLAYLIST_FOREGROUND_POS.y);
		_PLAYLIST_ITEMS_SURFACE_SIZE = glm::vec2(mdCurrentWidth - 400.f, mdCurrentHeight - 30.f);

		_PLAYLIST_ITEM_SIZE = glm::vec2(300.f, 30.f);

		_PLAYLIST_SEPARATOR_POS_OFFSET = glm::vec2(10.f, 0.f);
		_PLAYLIST_SEPARATOR_SIZE = glm::vec2(300.f + _PLAYLIST_SEPARATOR_POS_OFFSET.x, 20.f);

		_PLAYLIST_SCROLL_BAR_POS = glm::vec2(mdCurrentWidth - 60.f, _PLAYLIST_FOREGROUND_POS.y);
		_PLAYLIST_SCROLL_BAR_SIZE = glm::vec2(20.f, 20.f);

		_PLAYLIST_ADD_BUTTON_POS = glm::vec2(40.f, mdCurrentHeight - 20.f);
		_PLAYLIST_ADD_BUTTON_SIZE = glm::vec2(35.f, 35.f);

		_PLAYLIST_ADD_BUTTON_TEXTBOX_POS = glm::vec2(40.f, mdCurrentHeight - 20.f);
		_PLAYLIST_ADD_BUTTON_TEXTBOX_SIZE = glm::vec2(120.f, 100);


		_MAX_SIZE_RAM_LOADED	= 50;

		VolumeLevel				= 50;
		VolumeKeyMultiplier		= 0.8f;
		VolumeScrollStep		= 2.f;
		PauseFadeTime			= 500;
		PlaylistRollMultiplier	= 500;

		PlaylistScrollStep		= 30.f;

		PlaylistBarMovableZoneXOffset = 150;
	}

	void Data::UpdateData()
	{
		// keep width as default for now
		mdCurrentWidth = mdEngine::Window::windowProperties.mWindowWidth;
		mdCurrentHeight = mdEngine::Window::windowProperties.mApplicationHeight;

		//std::cout << mdCurrentHeight << std::endl;
		mdDefaultWidth = 500.f;
		mdDefaultHeight = Window::windowProperties.mWindowHeight - 350.f;


		_MAIN_BACKGROUND_SIZE = glm::vec2(mdDefaultWidth, mdCurrentHeight);

		_PLAYLIST_FOREGROUND_SIZE = glm::vec2(mdDefaultWidth - 40.f, mdCurrentHeight - _DEFAULT_PLAYER_SIZE.y - 50.f);

		_PLAYLIST_ITEMS_SURFACE_POS = glm::vec2(mdCurrentWidth / 2.f - 150.f, _PLAYLIST_FOREGROUND_POS.y);
		_PLAYLIST_ITEMS_SURFACE_SIZE = glm::vec2(mdCurrentWidth - 100.f, mdCurrentHeight - 30.f);
		_PLAYLIST_ADD_BUTTON_POS = glm::vec2(40.f, mdCurrentHeight - 35.f);

		Data::_MAIN_BACKGROUND_SIZE.y = mdCurrentHeight;

		_PLAYLIST_ADD_BUTTON_TEXTBOX_POS = glm::vec2(40.f, mdCurrentHeight - 20.f);

	}

	void Data::CloseData()
	{
		TTF_CloseFont(_MUSIC_PLAYER_FONT);
		TTF_CloseFont(_MUSIC_PLAYER_NUMBER_FONT);
		_MUSIC_PLAYER_FONT = NULL;
		_MUSIC_PLAYER_NUMBER_FONT = NULL;
	}
}
}