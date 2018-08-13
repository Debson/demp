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
	static std::vector<AudioObject*> m_AudioObjectContainer;
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
	for (s32 i = 0; i < m_AudioObjectContainer.size(); i++)
	{
		delete m_AudioObjectContainer[i];
	}

	m_AudioObjectContainer.clear();

	return true;
}

void Audio::UpdateAudioLogic()
{
	if (firstEnter == false)
	{
		lastVecSize = m_AudioObjectContainer.size();
		firstEnter = true;
	}


	if (lastVecSize != m_AudioObjectContainer.size())
	{
		lastVecSize = m_AudioObjectContainer.size();
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
		std::thread t(RetrieveInfo);
		t.detach();
		std::cout << "Ticks after load: " << Time::GetTicks() << std::endl;
	}
}

void Audio::RetrieveInfo()
{
	for (s32 i = 0; i < m_AudioObjectContainer.size(); i++)
	{
		Info::GetInfo(&m_AudioObjectContainer[i]->GetAudioProperty()->info, 
					   m_AudioObjectContainer[i]->GetPath());
	}
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

std::vector<Audio::AudioObject*>& Audio::Object::GetContainer()
{
	return m_AudioObjectContainer;
}

Audio::AudioObject* Audio::Object::GetItem(u32 id)
{
	if (id < m_AudioObjectContainer.size())
		return m_AudioObjectContainer.at(id);

	return NULL;
}

u32 Audio::Object::GetSize()
{
	return m_AudioObjectContainer.size();
}


b8 Audio::AddAudioItem(const std::wstring path)
{
	if (Info::CheckIfAudio(path) == false)
		return false;

	AudioObject* audioObject = NULL;
	AudioProperties* item = NULL;
		
	item = new AudioProperties();
	item->id = itemCount;
	item->path = path;
	item->folder = Info::GetFolder(path);
	item->info.format = Info::GetExt(path);

	if (item->info.format.compare(L"MP3") == 0)
	{
		//Info::GetID3Info(&item->info, path);
	}
	else
	{
		//item->info.title = Info::GetCompleteTitle(path);
	}


	audioObject = new AudioObject(item);
	m_AudioObjectContainer.push_back(audioObject);

	itemCount++;
	m_PathContainer.push_back(path);
	m_FolderContainer.push_back(item->folder);


	return true;
}

void Audio::GetItemsInfo()
{
	
	for (s32 i = 0; i < m_AudioObjectContainer.size(); i++)
	{
		std::cout << std::endl;
		std::cout << "ID: " << m_AudioObjectContainer[i]->GetID() << std::endl;
		std::cout << "Path: "<< utf16_to_utf8(m_AudioObjectContainer[i]->GetPath()) << std::endl;
		std::cout << "Fodler path: " << utf16_to_utf8(m_AudioObjectContainer[i]->GetFolderPath()) << std::endl;
		std::cout << "Artist: "<< utf16_to_utf8(m_AudioObjectContainer[i]->GetArtist()) << std::endl;
		std::cout << "Title: " << utf16_to_utf8(m_AudioObjectContainer[i]->GetTitle()) << std::endl;
		std::cout << "Track nr: " << utf16_to_utf8(m_AudioObjectContainer[i]->GetTrackNum()) << std::endl;
		std::cout << "Album: " << utf16_to_utf8(m_AudioObjectContainer[i]->GetAlbum()) << std::endl;
		std::cout << "Year: " << utf16_to_utf8(m_AudioObjectContainer[i]->GetYear()) << std::endl;
		std::cout << "Comment: " << utf16_to_utf8(m_AudioObjectContainer[i]->GetComment()) << std::endl;
		std::cout << "Genre: " << utf16_to_utf8(m_AudioObjectContainer[i]->GetGenre()) << std::endl;
		std::wcout << "Ext: " << m_AudioObjectContainer[i]->GetFormat() << std::endl;
		std::cout << "Freq: " << m_AudioObjectContainer[i]->GetFrequency() << std::endl;
		std::cout << "Bitrate: " << m_AudioObjectContainer[i]->GetBitrate() << std::endl;
		std::cout << "Size: " << m_AudioObjectContainer[i]->GetObjectSize() << std::endl;
		std::cout << "Length: " << m_AudioObjectContainer[i]->GetLength() << "s" << std::endl;
	}

}

