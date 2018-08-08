#include "music_player_settings.h"

#include "realtime_system_application.h"

namespace mdEngine
{
namespace MP
{
	namespace Data
	{
		u16 _PLAYLIST_CHOOSE_ITEM_DELAY;

		TTF_Font* _MUSIC_PLAYER_FONT;

		glm::vec2 _MIN_PLAYER_SIZE;

		glm::vec2 _DEFAULT_PLAYER_POS;
		glm::vec2 _DEFAULT_PLAYER_SIZE;

		glm::vec2 _DEFAULT_WINDOW_POS;
		glm::vec2 _DEFAULT_WINDOW_SIZE;

		glm::vec2 _MAIN_BACKGROUND_POS;
		glm::vec2 _MAIN_BACKGROUND_SIZE;

		glm::vec2 _MAIN_FOREGROUND_POS;
		glm::vec2 _MAIN_FOREGROUND_SIZE;

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

		glm::vec2 _UI_WINDOW_BAR_POS;
		glm::vec2 _UI_WINDOW_BAR_SIZE;

		glm::vec2 _EXIT_BUTTON_POS;
		glm::vec2 _EXIT_BUTTON_SIZE;

		glm::vec2 _MINIMIZE_BUTTON_POS;
		glm::vec2 _MINIMIZE_BUTTON_SIZE;

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

		glm::vec2 _PLAYLIST_SCROLL_BAR_POS;
		glm::vec2 _PLAYLIST_SCROLL_BAR_SIZE;

		f32 VolumeKeyMultiplier = 0.8f;

		f32 PlaylistScrollStep = 5.f;

		f32 mdDefaultWidth;
		f32 mdDefaultHeight;
		f32 mdCurrentWidth;
		f32 mdCurrentHeight;
	}

	void Data::InitializeData()
	{
		mdCurrentWidth = mdEngine::Window::windowProperties.mWindowWidth;
		mdCurrentHeight = mdEngine::Window::windowProperties.mApplicationHeight;

		mdDefaultWidth = 500.f;
		mdDefaultHeight = 350.f;;

		s16 musicUIOffsetX = -60;
		s16 musicUIOffsetY = 35;
		s16 musicProgressBarOffsetY = 10;

		_PLAYLIST_CHOOSE_ITEM_DELAY = 400;

		_MUSIC_PLAYER_FONT = TTF_OpenFont("assets/font/Raleway-Regular.ttf", 14);

		_MIN_PLAYER_SIZE = glm::vec2(500.f, 500.f);

		_DEFAULT_PLAYER_POS = glm::vec2(0.f, 0.f);
		_DEFAULT_PLAYER_SIZE = glm::vec2(500.f, 350.f);

		_DEFAULT_WINDOW_POS = glm::vec2(0.f, 0.f);
		_DEFAULT_WINDOW_SIZE = glm::vec2(mdDefaultWidth, 700.f);

		_MAIN_BACKGROUND_POS = glm::vec2(0.f, 0.f);
		_MAIN_BACKGROUND_SIZE = glm::vec2(mdDefaultWidth, mdDefaultHeight);

		_MAIN_FOREGROUND_POS = glm::vec2(20.0f, 20.0f);
		_MAIN_FOREGROUND_SIZE = glm::vec2(mdDefaultWidth - 40.f, mdDefaultHeight - 75.f);

		_PLAYLIST_FOREGROUND_POS = glm::vec2(20.f, mdDefaultHeight - 20.f);
		_PLAYLIST_FOREGROUND_SIZE = glm::vec2(mdDefaultWidth - 40.f, mdDefaultHeight);

		_EXIT_BUTTON_POS = glm::vec2(mdDefaultWidth - 50.f, 5.f);
		_EXIT_BUTTON_SIZE = glm::vec2(15.f, 15.f);


		/* Initialize later */

		_VOLUME_BAR_POS = glm::vec2(mdDefaultWidth / 2.f - 170.f, mdDefaultHeight - 60.f - musicUIOffsetY);
		_VOLUME_BAR_SIZE = glm::vec2(90.f, 4.f);

		_VOLUME_SPEAKER_POS = glm::vec2(mdDefaultWidth / 2.f - 200.f, mdDefaultHeight - 66.f - musicUIOffsetY);
		_VOLUME_SPEAKER_SIZE = glm::vec2(15.f, 15.f);

		_MUSIC_PROGRESS_BAR_POS = glm::vec2(mdDefaultWidth / 2.f - 175.f, mdDefaultHeight - 35.f - musicProgressBarOffsetY);
		_MUSIC_PROGRESS_BAR_SIZE = glm::vec2(mdDefaultWidth / 2.f + 100.f, 5.f);


		_MUSIC_PROGRESS_BAR_DOT_POS = glm::vec2(mdDefaultWidth / 2.f - 175.f, mdDefaultHeight - 37.f - musicProgressBarOffsetY);
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

		_PLAYLIST_ITEMS_SURFACE_POS = glm::vec2(mdCurrentWidth / 2.f - 150.f, mdCurrentHeight - (mdCurrentHeight - 350.f));
		_PLAYLIST_ITEMS_SURFACE_SIZE = glm::vec2(mdCurrentWidth - 400.f, mdCurrentHeight);

		_PLAYLIST_ITEM_SIZE = glm::vec2(300.f, 30.f);

		_PLAYLIST_SCROLL_BAR_POS = glm::vec2(mdCurrentWidth - 60.f, mdCurrentHeight - (mdCurrentHeight - 350.f) - 20.f);
		_PLAYLIST_SCROLL_BAR_SIZE = glm::vec2(20.f, 15.f);
	}

	void Data::UpdateData()
	{
		// keep width as default for now
		mdCurrentWidth = mdEngine::Window::windowProperties.mWindowWidth;
		mdCurrentHeight = mdEngine::Window::windowProperties.mApplicationHeight;

		//std::cout << mdCurrentHeight << std::endl;
		mdDefaultWidth = 500.f;
		mdDefaultHeight = Window::windowProperties.mWindowHeight - 350.f;

		_PLAYLIST_ITEMS_SURFACE_POS = glm::vec2(mdCurrentWidth / 2.f - 150.f, mdCurrentHeight - (mdCurrentHeight - 350.f) - 20.f);
		_PLAYLIST_ITEMS_SURFACE_SIZE = glm::vec2(mdCurrentWidth - 400.f, mdCurrentHeight - 20.f);

	}
}
}