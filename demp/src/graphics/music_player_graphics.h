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
		
			b8 IsEnabled() const;
			b8 IsToggled() const;
			b8 hasFocus() const;
			s32 GetSelectedID() const;
			s32 GetPlayingID() const;
			void Enable();
			void Toggle();
			void UnToggle();
			void SetSelectedID(s32 id);
			void SetPlayingID(s32 id);
			void SetPos(glm::vec2 pos);
			void SetSize(glm::vec2 size);
			void SetItemsSize(f64 itemsSize);
			void SetItemsDuration(f64 itemsDuration);
			f64 GetItemsSize() const;
			f64 GetItemsDuration() const;

			std::string GetItemsSizeString() const;
			std::string GetItemsDurationString() const;

			glm::vec2 GetPos() const;
			glm::vec2 GetSize() const;

			/* multipleSelect vector stores pointers to the actual audio object ids, simply because 
			   after item deletion every id greater than deleted index is decremented so by having
			   pointers to ids vector has updated ids 
			*/
			std::vector<s32*> multipleSelect;

		private:
			b8 m_Focus;
			glm::vec2 m_Pos;
			glm::vec2 m_Size;
			f64 m_ItemsDuration;
			f64 m_ItemsSize;
			std::string m_ItemsDurationStr;
			std::string m_ItemsSizeStr;

			s32 m_SelectedID;
			s32 m_PlayingID;
			b8 m_Enabled;
			b8 m_Toggled;

		};

		class MainPlayerObject
		{
		public:

			b8 hasFocus();
			void SetPos(glm::vec2 pos);
			void SetSize(glm::vec2 size);

		private:
			glm::vec2 m_Pos;
			glm::vec2 m_Size;

		};

		PlaylistObject* GetPlaylistObject();
		MainPlayerObject* GetMainPlayerObject();

		// TODO: It cant be an extern variable...
		extern Interface::TextBox m_AddFileTextBox;

		void StartMainWindow();

		void UpdateMainWindow();

		void RenderMainWindow();

		void CloseMainWindow();

	}
}
}

#endif // !MUSIC_PLAYER_GRAPHICS_H
