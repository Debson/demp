#pragma once
#ifndef MUSIC_PLAYER_GRAPHICS_H
#define MUSIC_PLAYER_GRAPHICS_H
#include <glm.hpp>

#include "md_types.h"
#include "md_time.h"

namespace mdEngine
{
namespace Graphics
{
	namespace MP
	{
		enum class PlaylistMovement
		{
			Up,
			Down
		};

		class PlaylistObject
		{
		public:
			PlaylistObject();

			void Enable();
			void Disable();
			void Update();

		
			b8 IsEnabled();
			b8 IsToggled();
			b8 IsRolling();
			void SetRollTime(s16 time);
			void SetSelectedID(s32 id);
			void SetPlayingID(s32 id);
			s32 GetSelectedID();
			s32 GetPlayingID();
			// Returns float value from 0 to 1 signifying current roll state
			f32 GetRollProgress();
			std::vector<s32*> multipleSelect;
		private:
			s32 selectedID;
			s32 playingID;
			Time::Timer timer;
			f32 currTime;
			f32 prevTime;
			s16 unwindTime;
			b8 enabled;
			b8 toggled;

		};

		extern PlaylistObject playlist;

		void StartMainWindow();

		void UpdateMainWindow();

		void RenderMainWindow();

		void CloseMainWindow();

	}
}
}

#endif // !MUSIC_PLAYER_GRAPHICS_H
