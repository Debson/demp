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
#include "../music_player_settings.h"



namespace fs = boost::filesystem;
using namespace mdEngine;

namespace Audio
{
	static std::vector<AudioObject*> m_AudioObjectContainer;
	static std::vector<std::wstring*> m_FolderContainer;
	static std::vector<std::wstring*> m_PathContainer;

	static s32 itemCount = 0;
	static u32 processedFileCount = 0;
	static u32 currentPropetyFilePos = 0;
	static u32 currentID3FilePos = 0;

	Time::Timer lastFunctionCallTimer;
	b8 startLoadingProperties(false);;
	b8 propertiesLoaded(true);


	b8 AddAudioItem(const std::wstring path);
	void RetrieveInfo();
	void RetrieveID3Info();
}


b8 Audio::PushToPlaylist(const std::wstring path)
{
	lastFunctionCallTimer.start();

	if (fs::exists(path))
	{
		if (Info::CheckIfAlreadyLoaded(&m_PathContainer, path) == true)
		{
			MD_ERROR("Audio item at path \"" + utf16_to_utf8(path) + "\" already loaded!\n");
			return false;
		}
	
		mdEngine::MP::musicPlayerState = mdEngine::MP::MusicPlayerState::kMusicAdded;

		if (fs::is_directory(path))
		{
			if (Info::CheckIfHasItems(path))
			{
				std::wstring* p = new std::wstring(path);
				m_FolderContainer.push_back(p);
			}
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
	m_FolderContainer.clear();
	m_PathContainer.clear();


	return true;
}

void Audio::UpdateAudioLogic()
{
	//std::cout << State::IsPlaylistEmpty << std::endl;

	if (lastFunctionCallTimer.getTicksStart() > WAIT_TIME_BEFORE_NEXT_CALL)
	{
		startLoadingProperties = true;
		lastFunctionCallTimer.stop();
		//std::cout << "essa\n";
	}

	if (startLoadingProperties == true)
	{
		startLoadingProperties = false;

		State::IsPlaylistEmpty = false;

		std::thread t(RetrieveInfo);
		t.detach();

#ifdef _EXTRACT_ID3_TAGS_
		t = std::thread(RetrieveID3Info);
		t.detach();
#endif
	}
}

void Audio::OnUpdateAudio()
{
	currentPropetyFilePos = m_AudioObjectContainer.size();
	currentID3FilePos = m_AudioObjectContainer.size();
}

void Audio::PerformDeletion(s32 index)
{
	// Won't happen that index will be less than 0, so dont have to check it
	delete m_AudioObjectContainer.at(index);
	m_AudioObjectContainer.erase(m_AudioObjectContainer.begin() + index);
	m_PathContainer.erase(m_PathContainer.begin() + index);

	if (m_AudioObjectContainer.size() == 0)
		State::IsPlaylistEmpty = true;

	itemCount--;
}

void Audio::RetrieveInfo()
{
	s32 i = currentPropetyFilePos;
	std::cout << "i: " << i << std::endl;
	for (; i < m_AudioObjectContainer.size(); i++)
	{
		Info::GetInfo(&m_AudioObjectContainer[i]->GetAudioProperty()->info, 
					   m_AudioObjectContainer[i]->GetPath());

		std::cout << i << "/" << m_AudioObjectContainer.size() << std::endl;
	}
	currentPropetyFilePos = i;
}

void Audio::RetrieveID3Info()
{
	s32 i = currentID3FilePos;
	for (; i < m_AudioObjectContainer.size(); i++)
	{
		AudioProperties* item = m_AudioObjectContainer[i]->GetAudioProperty();

		if (item->info.format.compare(L"MP3") == 0)
		{
			Info::GetID3Info(&item->info, item->path);
		}
		else
		{
			item->info.title = Info::GetCompleteTitle(item->path);
		}

		std::cout << i << "/" << m_AudioObjectContainer.size() << std::endl;
		processedFileCount = i;
	}
	currentID3FilePos = i;
}

std::vector<std::wstring*>& Audio::Folders::GetAudioFoldersContainer()
{
	return m_FolderContainer;
}

std::wstring* Audio::Folders::GetAudioFolder(s32 id)
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
		std::wstring* p = new std::wstring(name);
		m_FolderContainer.push_back(p);
		return true;
	}

	return false;
}

void Audio::Folders::PrintContent()
{
	for (s32 i = 0; i < m_FolderContainer.size(); i++)
		std::cout << utf16_to_utf8(*m_FolderContainer.at(i)) << std::endl;
}

std::vector<Audio::AudioObject*>& Audio::Object::GetAudioObjectContainer()
{
	return m_AudioObjectContainer;
}

Audio::AudioObject* Audio::Object::GetAudioObject(s32 id)
{
	if (id < m_AudioObjectContainer.size() && id >= 0)
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
		
	// Need to retrieve basic properties on create time
	item = new AudioProperties();
	item->id = itemCount;
	item->path = path;
	item->folder = Info::GetFolder(path);
	item->info.format = Info::GetExt(path);


	audioObject = new AudioObject(item);
	m_AudioObjectContainer.push_back(audioObject);
	audioObject->Init(); // Create playlsit item AFTER audio object was push to the container

	// Every created item has it's own unique id in ascending order
	itemCount++;
	// Store path and folder references for processing purposes

	m_PathContainer.push_back(&item->path);
	m_FolderContainer.push_back(&item->folder);


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

u32 Audio::GetProccessedFileCount()
{
	u32 files = processedFileCount;
	return files;
}

