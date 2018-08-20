#pragma once
#ifndef MUSIC_PLAYER_GRAPHICS_H
#define MUSIC_PLAYER_GRAPHICS_H
#include <glm.hpp>

#include "../utility/md_types.h"
#include "../utility/md_time.h"
#include "../interface/md_interface.h"

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

		
			b8 IsEnabled();
			b8 IsToggled();
			b8 hasFocus();
			s32 GetSelectedID();
			s32 GetPlayingID();
			void Enable();
			void Toggle();
			void UnToggle();
			void SetSelectedID(s32 id);
			void SetPlayingID(s32 id);
			void setPos(glm::vec2 pos);
			void setSize(glm::vec2 size);
			glm::vec2 getPos();
			glm::vec2 getSize();

			/* multipleSelect vector stores pointers to the actual audio object ids, simply because 
			   after item deletion every id greater than deleted index is decremented so by having
			   pointers to ids vector has updated ids 
			*/
			std::vector<s32*> multipleSelect;

		private:
			b8 m_Focus;
			glm::vec2 m_Pos;
			glm::vec2 m_Size;
			s32 m_SelectedID;
			s32 m_PlayingID;
			b8 m_Enabled;
			b8 m_Toggled;

		};

		class MainPlayerObject
		{
		public:


			b8 hasFocus();
			void setPos(glm::vec2 pos);
			void setSize(glm::vec2 size);

		private:
			glm::vec2 m_Pos;
			glm::vec2 m_Size;

		};

		extern PlaylistObject m_Playlist;
		extern MainPlayerObject m_MainPlayer;
		extern Interface::TextBox m_AddFileTextBox;

		void StartMainWindow();

		void UpdateMainWindow();

		void RenderMainWindow();

		void CloseMainWindow();

	}
}
}

#endif // !MUSIC_PLAYER_GRAPHICS_H
