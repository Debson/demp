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
			// Returns float value from 0 to 1 signifying current roll state
			f32 GetRollProgress();
		private:
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

		void RenderTest();

		void CloseMainWindow();

	}
}
}

#endif // !MUSIC_PLAYER_GRAPHICS_H
