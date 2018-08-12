#include "mp_audio.h"

#include <thread>
#include <map>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

#include "../utf8_to_utf16.h"
#include "../sqlite/md_sqlite.h"
#include "../md_util.h"
#include "../md_time.h"
#include "../interface/md_interface.h"
#include "../music_player.h"
#include "../md_types.h"
#include "../music_player_state.h"

namespace fs = boost::filesystem;
using namespace mdEngine;

namespace Audio
{
	static std::vector<AudioItem*> m_AudioItemContainer;
	static std::vector<std::wstring> m_FolderContainer;
	static std::vector<std::wstring> m_PathContainer;

	static s32 itemCount = 0;

	extern b8 PathsLoaded(false);


	b8 loadingPaths(false);
	b8 loadInfo(false);
	b8 firstEnter(false);
	s32 lastVecSize = 0;


	b8 AddAudioItem(const std::wstring path);

	void RetrieveInfo();

}

void Audio::Test(std::wstring pa)
{

}


b8 Audio::PushToPlaylist(const std::wstring path)
{
	if (fs::exists(path))
	{
		if (Info::CheckIfAlreadyLoaded(&m_PathContainer, path) == true)
		{
			MD_ERROR("Audio item at path \"" + utf16_to_utf8(path) + "\" already loaded!\n");
			return false;
		}
		
		loadingPaths = true;
		mdEngine::MP::musicPlayerState = mdEngine::MP::MusicPlayerState::kMusicAdded;

		if (fs::is_directory(path))
		{
			if(Info::CheckIfHasItems(path))
				m_FolderContainer.push_back(path);
			for (auto & i : fs::directory_iterator(path))
			{
				if (Info::CheckIfAlreadyLoaded(&m_PathContainer, i.path().wstring()) == true)
				{
					MD_ERROR("Audio item at path \"" + utf16_to_utf8(path) + "\" already loaded!\n");
					return false;
				}

				AddAudioItem(i.path().wstring());
				if (fs::is_directory(i.path().wstring()))
					PushToPlaylist(i.path().wstring());
			}
		}
		else
		{
			AddAudioItem(path);
		}
	}
	else
	{
		MD_ERROR("File at path \"" + utf16_to_utf8(path) + "\" does not exist!");
	}

	return true;
}

b8 Audio::DeallocateAudioItems()
{
	for (s32 i = 0; i < m_AudioItemContainer.size(); i++)
	{
		delete m_AudioItemContainer[i];
	}

	m_AudioItemContainer.clear();

	return true;
}

void Audio::UpdateAudioLogic()
{
	if (firstEnter == false)
	{
		lastVecSize = m_AudioItemContainer.size();
		firstEnter = true;
	}


	if (lastVecSize != m_AudioItemContainer.size())
	{
		lastVecSize = m_AudioItemContainer.size();
		loadingPaths = false;
		loadInfo = true;
	}
	else
	{
		loadingPaths = true;
	}

	if (loadingPaths && loadInfo && State::PathLoadedFromFile == false)
	{
		State::IsPlaylistEmpty = false;
		loadInfo = false;
		//std::cout << m_AudioItemContainer.size() << std::endl;
		std::thread t(RetrieveInfo);
		t.detach();
	}
}

void Audio::RetrieveInfo()
{
	for (s32 i = 0; i < m_AudioItemContainer.size(); i++)
	{
		Info::GetInfo(&m_AudioItemContainer[i]->info, m_AudioItemContainer[i]->path);
	}
}

std::vector<Audio::AudioItem*>& Audio::Items::GetContainer()
{
	return m_AudioItemContainer;
}

Audio::AudioItem* Audio::Items::GetItem(s32 id)
{
	if (m_AudioItemContainer.size() > 0 &&
		m_AudioItemContainer.size() > id &&
		id >= 0)
	{
		return m_AudioItemContainer.at(id);
	}

	return NULL;
}

u32 Audio::Items::GetSize()
{
	return m_AudioItemContainer.size();
}


std::vector<std::wstring>& Audio::Folders::GetContainer()
{
	return m_FolderContainer;
}

std::wstring Audio::Folders::GetItem(s32 id)
{
	if (m_FolderContainer.size() > 0 &&
		m_FolderContainer.size() > id &&
		id >= 0)
	{
		return m_FolderContainer.at(id);
	}

	return NULL;
}

u32 Audio::Folders::GetSize()
{
	return m_FolderContainer.size();
}

b8 Audio::Folders::AddFolder(std::wstring name)
{
	fs::path p(name);

	if (fs::exists(name))
	{
		m_FolderContainer.push_back(name);
		return true;
	}

	return false;
}

void Audio::Folders::PrintContent()
{
	for (s32 i = 0; i < m_FolderContainer.size(); i++)
		std::cout << utf16_to_utf8(m_FolderContainer.at(i)) << std::endl;
}

b8 Audio::AddAudioItem(const std::wstring path)
{
	if (Info::CheckIfAudio(path) == false)
		return false;

	AudioItem * item = new AudioItem();
	item->id = itemCount;
	item->path = path;
	item->folder = Info::GetFolder(path);
	item->name = Info::GetName(path);
	item->info.ext = Info::GetExt(path);
	itemCount++;
	m_PathContainer.push_back(path);

	m_AudioItemContainer.push_back(item);

	mdEngine::Interface::PlaylistItem * pItem = new mdEngine::Interface::PlaylistItem();
	pItem->InitFont();
	pItem->InitItem();

	return true;
}

void Audio::GetItemsInfo()
{
	
	for (s32 i = 0; i < m_AudioItemContainer.size(); i++)
	{
		std::cout << std::endl;
		std::cout << m_AudioItemContainer[i]->id << std::endl;
		std::cout << utf16_to_utf8(m_AudioItemContainer[i]->path) << std::endl;
		std::cout << utf16_to_utf8(m_AudioItemContainer[i]->folder) << std::endl;
		std::cout << utf16_to_utf8(m_AudioItemContainer[i]->name) << std::endl;
		std::wcout << m_AudioItemContainer[i]->info.ext << std::endl;
		std::cout << m_AudioItemContainer[i]->info.freq << std::endl;
		std::cout << m_AudioItemContainer[i]->info.bitrate << std::endl;
		std::cout << m_AudioItemContainer[i]->info.size << std::endl;
		std::cout << m_AudioItemContainer[i]->info.length << "s" << std::endl;
	}

}

