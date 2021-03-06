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

		glm::vec2 _MAIN_BACKGROUND_SIZE;

		glm::vec2 _PLAYLIST_FOREGROUND_SIZE;

		glm::vec2 _PLAYLIST_ITEMS_SURFACE_POS = glm::vec2(mdDefaultWidth / 2.f - 150.f, _PLAYLIST_FOREGROUND_POS.y);;
		glm::vec2 _PLAYLIST_ITEMS_SURFACE_SIZE;

		glm::vec2 _PLAYLIST_ADD_BUTTON_POS;
		glm::vec2 _PLAYLIST_ADD_BUTTON_SIZE;

		glm::vec2 _PLAYLIST_ADD_BUTTON_TEXTBOX_POS;

		glm::vec2 _PLAYLIST_ADD_ICON_POS;

		s32 _MAX_SIZE_RAM_LOADED;

		f32 VolumeLevel;
		f32 VolumeKeyMultiplier;
		s32 VolumeScrollStep;
		s32 PauseFadeTime;
		s32 PlaylistRollMultiplier;

		f32 PlaylistScrollStep;

		s32 PlaylistBarMovableZoneXOffset;

		// private
		f32 mdCurrentWidth;
		f32 mdCurrentHeight;
	}

	namespace Settings
	{
		b8 IsPathExistenceCheckingEnabled(true);
	}

	void Data::InitializeData()
	{
		_MAIN_BACKGROUND_SIZE = glm::vec2(mdDefaultWidth, mdDefaultHeight);

		_PLAYLIST_FOREGROUND_SIZE = glm::vec2(mdDefaultWidth - 40.f, mdDefaultHeight - 40.f);

		//_PLAYLIST_ITEMS_SURFACE_POS = glm::vec2(mdDefaultWidth / 2.f - 150.f, _PLAYLIST_FOREGROUND_POS.y);
		_PLAYLIST_ITEMS_SURFACE_SIZE = glm::vec2(mdDefaultHeight - 400.f, mdDefaultHeight - 30.f);

		_PLAYLIST_ADD_BUTTON_TEXTBOX_POS = glm::vec2(40.f, mdDefaultHeight - 20.f);

		_MAX_SIZE_RAM_LOADED	= 50;

		VolumeLevel				= 0.5f;
		VolumeKeyMultiplier		= 0.8f;
		VolumeScrollStep		= 2.f;
		PauseFadeTime			= 300;
		PlaylistRollMultiplier	= 500;

		PlaylistScrollStep		= 30.f;

		PlaylistBarMovableZoneXOffset = 150;
	}

	void Data::UpdateData()
	{
		// keep width as default for now
		mdCurrentWidth = mdEngine::Window::WindowProperties.m_ApplicationWidth;
		mdCurrentHeight = mdEngine::Window::WindowProperties.m_ApplicationHeight;

		//std::cout << mdCurrentHeight << std::endl;
		mdDefaultWidth = _DEFAULT_PLAYER_SIZE.x;
		mdDefaultHeight = Window::WindowProperties.m_WindowHeight - 350.f;


		_MAIN_BACKGROUND_SIZE = glm::vec2(mdDefaultWidth, mdCurrentHeight);

		_PLAYLIST_FOREGROUND_SIZE = glm::vec2(mdDefaultWidth - 40.f, mdCurrentHeight - _DEFAULT_PLAYER_SIZE.y - 50.f);

		_PLAYLIST_ITEMS_SURFACE_POS = glm::vec2(_PLAYLIST_FOREGROUND_POS.x + (_PLAYLIST_SEPARATOR_SIZE.x - _PLAYLIST_ITEM_SIZE.x), 
												_PLAYLIST_FOREGROUND_POS.y);
		_PLAYLIST_ITEMS_SURFACE_SIZE = glm::vec2(mdCurrentWidth - 100.f, mdCurrentHeight - 30.f);
		_PLAYLIST_ADD_BUTTON_POS = glm::vec2(20.f, _PLAYLIST_ITEMS_SURFACE_SIZE.y);
		_PLAYLIST_ADD_BUTTON_SIZE = glm::vec2(mdCurrentHeight - _PLAYLIST_ITEMS_SURFACE_SIZE.y);

		//Data::_MAIN_BACKGROUND_SIZE.y = mdCurrentHeight;

		/*_PLAYLIST_ADD_BUTTON_TEXTBOX_POS = glm::vec2(_PLAYLIST_ADD_BUTTON_POS.x, mdCurrentHeight - 20.f);
	
		s32 iconOffsetX = 10.f;
		_PLAYLIST_ADD_ICON_POS = glm::vec2(_PLAYLIST_ADD_BUTTON_TEXTBOX_POS.x + iconOffsetX, 0.f);*/

	}

	void Data::UpdateFPS(f32 newFPS)
	{
		_SCREEN_TICK_PER_FRAME = 1000 / newFPS;
	}

	void Data::CloseData()
	{
	
	}
}
}