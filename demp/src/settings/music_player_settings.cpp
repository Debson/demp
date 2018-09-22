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
		s32 _SCREEN_TICK_PER_FRAME = 1000 / _SCREEN_FPS;

		TTF_Font* _MUSIC_PLAYER_FONT;
		TTF_Font* _MUSIC_PLAYER_NUMBER_FONT;

		glm::vec2 _MAIN_BACKGROUND_SIZE;

		glm::vec2 _PLAYLIST_FOREGROUND_SIZE;

		glm::vec2 _PLAYLIST_ITEMS_SURFACE_POS;
		glm::vec2 _PLAYLIST_ITEMS_SURFACE_SIZE;

		glm::vec2 _PLAYLIST_ADD_BUTTON_POS;

		glm::vec2 _PLAYLIST_ADD_BUTTON_TEXTBOX_POS;

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
		_MUSIC_PLAYER_FONT = TTF_OpenFont("assets/font/Times New Roman.ttf", 14);
		_MUSIC_PLAYER_NUMBER_FONT = TTF_OpenFont("assets/font/Times New Roman.ttf", 14);

		_MAIN_BACKGROUND_SIZE = glm::vec2(mdDefaultWidth, mdDefaultHeight);

		_PLAYLIST_FOREGROUND_SIZE = glm::vec2(mdDefaultWidth - 40.f, mdDefaultHeight - 40.f);

		_PLAYLIST_ITEMS_SURFACE_POS = glm::vec2(mdDefaultWidth / 2.f - 150.f, _PLAYLIST_FOREGROUND_POS.y);
		_PLAYLIST_ITEMS_SURFACE_SIZE = glm::vec2(mdDefaultHeight - 400.f, mdDefaultHeight - 30.f);

		_PLAYLIST_ADD_BUTTON_TEXTBOX_POS = glm::vec2(40.f, mdDefaultHeight - 20.f);

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

	void Data::UpdateFPS(f32 newFPS)
	{
		_SCREEN_TICK_PER_FRAME = 1000 / newFPS;
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