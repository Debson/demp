#ifndef MUSIC_PLAYER_GRAPHICS_PLAYLIST_H
#define MUSIC_PLAYER_GRAPHICS_PLAYLIST_H


#include "../interface/md_interface.h"
#include "../utility/md_types.h"



namespace mdEngine
{
	namespace Graphics
	{
		namespace MP
		{
			typedef std::vector<Interface::PlaylistSeparator*> SeparatorsToRender;

			enum class PlaylistMovement
			{
				Up,
				Down
			};

			class PlaylistObject
			{
			public:
				PlaylistObject();

				b8   IsEnabled()	 const;
				b8   IsToggled()	 const;
				b8   hasFocus()		 const;
				s32  GetSelectedID() const;
				s32  GetPlayingID()	 const;
				void Enable();
				void Toggle();
				void UnToggle();
				void SetSelectedID(s32 id);
				void SetPlayingID(s32 id);
				void SetPos(glm::vec2 pos);
				void SetSize(glm::vec2 size);
				void SetItemsSize(f64 itemsSize);
				void SetItemsDuration(f64 itemsDuration);
				void AddToItemsDuration(f32 itemDuration);
				void AddToItemsSize(f32 itemSize);
				void SetIndexesToRender(std::vector<s32> indexesVec);
				void SetCurrentMinIndex(s32 min);
				void SetCurrentMaxIndex(s32 max);
				void SetCurrentOffset(s32 offset);
				void SetHiddenSeparatorCount(s32 count);
				f64  GetItemsSize()				const;
				f64  GetItemsDuration()			const;
				std::vector<s32>* GetIndexesToRender();
				//s32  GetCurrentMinIndex()		const;
				//s32  GetCurrentMaxIndex()		const;
				s32  GetCurrentOffset()			const;
				s32  GetHiddenSeparatorCount()  const;

				std::string  GetItemsSizeString()		;
				std::string  GetItemsDurationString()	;
				std::string GetMusicInfoScrollString() const;

				glm::vec2 GetPos()  const;
				glm::vec2 GetSize() const;

				b8 PlaylistTextBoxActive;

				/* multipleSelect vector stores pointers to the actual audio object ids, simply because
				   after item deletion every id greater than deleted index is decremented so by having
				   pointers to ids vector has updated ids
				*/
				std::vector<s32*> multipleSelect;

			private:
				b8			m_Enabled;
				b8			m_Toggled;
				b8			m_Focus;
				s32			m_SelectedID;
				s32			m_PlayingID;
				s32			m_CurrentMinIndex;
				s32			m_CurrentMaxIndex;
				s32			m_CurrentOffset;
				s32			m_HiddenSeparatorCount;
				f64			m_ItemsDuration;
				f64			m_ItemsSize;
				glm::vec2	m_Pos;
				glm::vec2	m_Size;
				std::string m_ItemsDurationStr;
				std::string m_ItemsSizeStr;
				std::vector<s32> m_IndexesToRender;

			};


			class MainPlayerObject
			{
			public:

				b8	 hasFocus();
				void SetPos(glm::vec2 pos);
				void SetSize(glm::vec2 size);

			private:
				glm::vec2 m_Pos;
				glm::vec2 m_Size;

			};


			PlaylistObject*		GetPlaylistObject();
			MainPlayerObject*	GetMainPlayerObject();
			SeparatorsToRender* GetSeparatorsToRender();
		}
	}
}

#endif // !MUSIC_PLAYER_GRAPHICS_PLAYLIST_H
