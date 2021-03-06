#include "music_player_graphics_playlist.h"


#include "../playlist/music_player_playlist.h"
#include "../utility/md_converter.h"
#include "../utility/utf8_to_utf16.h"

namespace mdEngine
{
	namespace Graphics
	{
		namespace MP
		{
			PlaylistObject		m_Playlist;
			MainPlayerObject	m_MainPlayer;
			SeparatorsToRender	playlistSeparatorToRenderVec;
		}


		MP::PlaylistObject::PlaylistObject()
		{
			m_Enabled = false;
			m_Toggled = false;
			m_SelectedID = -1;
			m_PlayingID = -1;
			m_CurrentMinIndex = 0;
			m_CurrentMaxIndex = 0;
			m_ItemsDuration = 0;
			m_ItemsSize = 0;
		}

		void MP::PlaylistObject::Enable()
		{
			m_Enabled = !m_Enabled;
		}

		void MP::PlaylistObject::Toggle()
		{
			m_Toggled = true;
		}

		void MP::PlaylistObject::UnToggle()
		{
			m_Toggled = false;
		}

		b8 MP::PlaylistObject::IsEnabled() const
		{
			return m_Enabled;
		}

		b8 MP::PlaylistObject::IsToggled() const
		{
			return m_Toggled;
		}

		void MP::PlaylistObject::SetSelectedID(s32 id)
		{
			m_SelectedID = id;
		}

		void MP::PlaylistObject::SetPlayingID(s32 id)
		{
			m_PlayingID = id;
		}

		s32 MP::PlaylistObject::GetSelectedID() const
		{
			return m_SelectedID;
		}

		s32 MP::PlaylistObject::GetPlayingID() const
		{
			return mdEngine::MP::Playlist::RamLoadedMusic.get() != NULL ? m_PlayingID : -1;
		}

		glm::vec2 MP::PlaylistObject::GetPos() const
		{
			return m_Pos;
		}

		glm::vec2 MP::PlaylistObject::GetSize() const
		{
			return m_Size;
		}

		void MP::PlaylistObject::SetPos(glm::vec2 pos)
		{
			m_Pos = pos;
		}

		void MP::PlaylistObject::SetSize(glm::vec2 size)
		{
			m_Size = size;
		}

		void MP::PlaylistObject::SetItemsSize(f64 itemsSize)
		{
			m_ItemsSize = itemsSize;
		}

		void MP::PlaylistObject::SetItemsDuration(f64 itemsDuration)
		{
			m_ItemsDuration = itemsDuration;
		}

		void MP::PlaylistObject::AddToItemsDuration(f32 itemDuration)
		{
			m_ItemsDuration += itemDuration;
		}

		void MP::PlaylistObject::AddToItemsSize(f32 itemSize)
		{
			m_ItemsSize += itemSize;
		}

		void MP::PlaylistObject::SetIndexesToRender(std::vector<s32> indexesVec)
		{
			m_IndexesToRender = indexesVec;
		}

		void MP::PlaylistObject::SetCurrentMinIndex(s32 min)
		{
			m_CurrentMinIndex = min;
		}

		void MP::PlaylistObject::SetCurrentMaxIndex(s32 max)
		{
			m_CurrentMaxIndex = max;
		}

		void MP::PlaylistObject::SetCurrentOffset(s32 offset)
		{
			m_CurrentOffset = offset;
		}

		void MP::PlaylistObject::SetHiddenSeparatorCount(s32 count)
		{
			m_HiddenSeparatorCount = count;
		}

		void MP::PlaylistObject::SetSelected(b8 val)
		{
			m_IsSelected = val;
		}

		f64 MP::PlaylistObject::GetItemsSize() const
		{
			return m_ItemsSize;
		}

		f64 MP::PlaylistObject::GetItemsDuration() const
		{
			return m_ItemsDuration;
		}

		std::string MP::PlaylistObject::GetItemsSizeString()
		{
			m_ItemsSizeStr = Converter::BytesToProperSizeFormat(m_ItemsSize);
			return m_ItemsSizeStr;
		}

		std::string MP::PlaylistObject::GetItemsDurationString()
		{
			m_ItemsDurationStr = Converter::SecToProperTimeFormat(m_ItemsDuration);
			return m_ItemsDurationStr;
		}

		std::string MP::PlaylistObject::GetMusicInfoScrollString() const
		{
			std::string str;
			s32 id = GetPlayingID();
			auto audioItem = Audio::Object::GetAudioObject(GetPlayingID());
			assert(audioItem != NULL);
			str += ".:: ";
			str += Converter::SecToProperTimeFormatShort(audioItem->GetLength());
			str += " :: ";
			str += audioItem->GetCompleteName();
			str += " :: ";
			str += audioItem->GetFormat();
			str += " :: ";
			str += Converter::FrequencyToProperFormat(audioItem->GetFrequency());
			str += " :: ";
			str += std::to_string((s32)audioItem->GetBitrate()) + " kbps";
			str += " :: ";
			str += Converter::BytesToProperSizeFormat(audioItem->GetObjectSize());
			str += " ::.";

			return str;
		}

		std::vector<s32>* MP::PlaylistObject::GetIndexesToRender()
		{
			return &m_IndexesToRender;
		}

		/*s32 MP::PlaylistObject::GetCurrentMinIndex() const
		{
			return m_CurrentMinIndex;
		}

		s32 MP::PlaylistObject::GetCurrentMaxIndex() const
		{
			return m_CurrentMaxIndex;
		}*/

		s32 MP::PlaylistObject::GetCurrentOffset() const
		{
			return m_CurrentOffset;
		}

		s32  MP::PlaylistObject::GetHiddenSeparatorCount()  const
		{
			return m_HiddenSeparatorCount;
		}

		b8 MP::PlaylistObject::hasFocus() const
		{
			s32 mouseX, mouseY;
			App::Input::GetMousePosition(&mouseX, &mouseY);

			return mouseX > m_Pos.x && mouseX < m_Size.x &&
				mouseY > m_Pos.y && mouseY < m_Size.y;
		}

		MP::PlaylistObject* MP::GetPlaylistObject()
		{
			return &m_Playlist;
		}

		b8 MP::MainPlayerObject::hasFocus()
		{
			int mouseX, mouseY;
			App::Input::GetMousePosition(&mouseX, &mouseY);

			return mouseX > m_Pos.x && mouseX < m_Size.x &&
				mouseY > m_Pos.y && mouseY < m_Size.y;
		}

		void MP::MainPlayerObject::SetPos(glm::vec2 pos)
		{
			m_Pos = pos;
		}

		void MP::MainPlayerObject::SetSize(glm::vec2 size)
		{
			m_Size = size;
		}

		MP::MainPlayerObject* MP::GetMainPlayerObject()
		{
			return &m_MainPlayer;
		}

		MP::SeparatorsToRender* MP::GetSeparatorsToRender()
		{
			return &playlistSeparatorToRenderVec;
		}



	}
}