#include "mp_audio.h"

#include <thread>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

#include "../utility/utf8_to_utf16.h"
#include "../utility/md_util.h"
#include "../utility/md_time.h"
#include "../utility/md_types.h"
#include "../interface/md_interface.h"
#include "../player/music_player.h"
#include "../player/music_player_state.h"
#include "../settings/music_player_settings.h"
#include "../ui/music_player_ui.h"

#define WORK_THREADS 4


namespace fs = boost::filesystem;
using namespace mdEngine;

namespace Audio
{
	static std::vector<AudioObject*> m_AudioObjectContainer;		
	static std::vector<std::wstring> m_FolderContainer;				// Stores only paths to folders
	static std::vector<std::wstring> m_LoadedPathContainer;			// Stores all paths of supported files	that are currently loaded into playlist
	static std::vector<std::wstring> m_AddedFilesPathContainer;		// Stores all valid paths of supported file formats just from one event(drag and drop, file expl. folder expl.)
	static std::vector<std::wstring> m_InitPaths;					// Stores paths were initially loaded on actions(drag and drop, file expl. folder expl.)

	static std::thread id3ProcessThread;
	static std::thread infoProcessThread;

	static s32 currentContainersSize = 0;

	Time::Timer lastFunctionCallTimer;
	Time::Timer lastPathPushTimer;
	b8 startLoadingProperties(false);
	b8 startLoadingPaths(false);
	b8 propertiesLoaded(true);


	void PushToPlaylistWrap();
	b8 AddAudioItem(std::wstring path, s32 id);
	void AddAudioItemWrap(const std::vector<std::wstring> vec, const s32 start, const s32 end);
	void ResizeAllContainers(int size);
}

b8 Audio::SavePathFiles(std::wstring path)
{
	if (Info::CheckIfAlreadyLoaded(&m_LoadedPathContainer, path) == true)
	{
		MD_ERROR("Audio item at path \"" + utf16_to_utf8(path) + "\" already loaded!\n");
		return false;
	}
	
	lastPathPushTimer.start();
	m_InitPaths.push_back(path);

	return true;
}

void Audio::PushToPlaylistWrap()
{
	for (s32 i = 0; i < m_InitPaths.size(); i++)
	{
		PushToPlaylist(m_InitPaths[i]);
	}
	m_InitPaths.clear();
}

b8 Audio::PushToPlaylist(std::wstring path)
{
	lastFunctionCallTimer.start();

	if (fs::exists(path))
	{
		if (Info::CheckIfAlreadyLoaded(&m_LoadedPathContainer, path) == true)
		{
			md_log("Audio item at path \"" + utf16_to_utf8(path) + "\" already loaded!\n");
			return false;
		}
	
		mdEngine::MP::musicPlayerState = mdEngine::MP::MusicPlayerState::kMusicAdded;

		if (fs::is_directory(path))
		{
			if (Info::CheckIfHasItems(path) && Info::CheckIfAlreadyLoaded(&m_FolderContainer, path) == false)
			{
				m_FolderContainer.push_back(path);
			}

			// Iterate throught all items in folder, if another folder found, resursively process it
			for (auto & i : fs::directory_iterator(path))
			{
				if (Info::CheckIfAlreadyLoaded(&m_LoadedPathContainer, i.path().wstring()) == true)
				{
					md_log("Audio item at path \"" + i.path().string() + "\" already loaded!\n");
				}
				else
				{
					if (fs::is_directory(i.path().wstring()))
						PushToPlaylist(i.path().wstring());
					else if(Info::CheckIfAudio(i.path().wstring()))
						m_AddedFilesPathContainer.push_back(i.path().wstring());
				}

			}

		}
		else
		{
			if (Info::CheckIfAlreadyLoaded(&m_LoadedPathContainer, path) == true)
			{
				md_log("Audio item at path \"" + utf16_to_utf8(path) + "\" already loaded!\n");
			}
			else if (Info::CheckIfAudio(path) )
			{
				m_AddedFilesPathContainer.push_back(path);
			}
		}
	}
	else
	{
		md_log("File at path \"" + utf16_to_utf8(path) + "\" does not exist!");
	}

	return true;
}

void Audio::UpdateAudioLogic()
{
	//std::cout << State::IsPlaylistEmpty << std::endl;

	if (lastPathPushTimer.getTicksStart() > MAX_PATH_WAIT_TIME)
	{
		startLoadingPaths = true;
		lastPathPushTimer.stop();
	}

	if (lastFunctionCallTimer.getTicksStart() > WAIT_TIME_BEFORE_NEXT_CALL)
	{
		startLoadingProperties = true;
		lastFunctionCallTimer.stop();
	}


	if (startLoadingPaths == true)
	{
		startLoadingPaths = false;
		PushToPlaylistWrap();
	}

	if (startLoadingProperties == true && m_AddedFilesPathContainer.size() > 0)
	{
		startLoadingProperties = false;

		State::IsPlaylistEmpty = false;

		// split the work between work threads
		std::thread* tt = new std::thread[WORK_THREADS];
		s32 toProcessCount = m_AddedFilesPathContainer.size();
		s32 div = toProcessCount / WORK_THREADS;
		ResizeAllContainers(toProcessCount);

		std::vector<std::wstring> tempVec(currentContainersSize);
		for (s32 i = 0; i < toProcessCount; i++)
			tempVec.push_back(m_AddedFilesPathContainer[i]);

		m_AddedFilesPathContainer.clear();

		// All vectors must be resized before any operations are performed on them


		// In some cases division will leave a remainder
		s8 rest = toProcessCount % WORK_THREADS;

		for (s8 i = 0; i < WORK_THREADS; i++)
		{
			if (i == WORK_THREADS - 1)
				tt[i] = std::thread(AddAudioItemWrap, tempVec, div * i + currentContainersSize, 
													  div * (i + 1) + rest + currentContainersSize);
			else
				tt[i] = std::thread(AddAudioItemWrap, tempVec, div * i + currentContainersSize, 
													  div * (i + 1) + currentContainersSize);

			md_log(std::to_string(div * i) + "    " + std::to_string(div * (i + 1)) + "\n");
		}

		for (s8 i = 0; i < WORK_THREADS; i++)
		{
			tt[i].detach();
		}
		
		// Keep track of size of containers
		currentContainersSize += toProcessCount;
		

		delete[] tt;
	}
}

void Audio::PerformDeletion(s32 index)
{
	assert(index >= 0);

	delete m_AudioObjectContainer.at(index);
	m_AudioObjectContainer.erase(m_AudioObjectContainer.begin() + index);
	for (s32 i = index; i < m_AudioObjectContainer.size(); i++)
		m_AudioObjectContainer[i]->DecrementID();
	m_LoadedPathContainer.erase(m_LoadedPathContainer.begin() + index);
	ResizeAllContainers(-1); // Is equal to current size - 1

	currentContainersSize--;
	if (currentContainersSize < 0)
	{
		currentContainersSize = 0;
	}

	if (m_AudioObjectContainer.empty())
	{
		State::IsPlaylistEmpty = true;
	}
}

std::vector<std::wstring>& Audio::Folders::GetAudioFoldersContainer()
{
	return m_FolderContainer;
}

std::wstring* Audio::Folders::GetAudioFolder(s32 id)
{
	if (m_FolderContainer.size() > 0 &&
		m_FolderContainer.size() > id &&
		id >= 0)
	{
		return &m_FolderContainer.at(id);
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

std::vector<Audio::AudioObject*>& Audio::Object::GetAudioObjectContainer()
{
	return m_AudioObjectContainer;
}

Audio::AudioObject* Audio::Object::GetAudioObject(s32 id)
{
	if (id < m_AudioObjectContainer.size() && id >= 0)
	{

		return m_AudioObjectContainer.at(id);
	}

	return NULL;
}

u32 Audio::Object::GetSize()
{
	return currentContainersSize;
}

u32 Audio::Object::GetProcessedSize()
{
	//TODO
	return 0;
}

void Audio::AddAudioItemWrap(const std::vector<std::wstring> vec, const s32 beg, const s32 end)
{
	s32 i = beg;
	for (; i < end; i++)
	{
		AddAudioItem(vec[i], i);
	}
}

b8 Audio::AddAudioItem(std::wstring path, s32 id)
{
	if (Info::CheckIfAudio(path) == false)
		return false;

	// Need to get basic properties before creating audio object
	auto item = new AudioProperties();
	item->id = id;
	item->path = path;
	item->folder = Info::GetFolder(path);
	item->info.format = Info::GetExt(path);


	auto audioObject = new AudioObject(item);
	m_AudioObjectContainer[id] = audioObject;
	/* Create playlsit item AFTER audio object was push to the container, 
	   so it can access AO class methods
	*/
	audioObject->Init(); 

	//std::cout << item->id << std::endl;
	/* By getting info struct throught object of given ID I avoid attempts to retrieve info multiple times 
	   for the same struct (hint 4 threads are working simultaneously)
	*/
	Info::GetInfo(&Object::GetAudioObject(id)->GetAudioProperty()->info, 
				   Object::GetAudioObject(id)->GetPath());

#ifdef _EXTRACT_ID3_TAGS_
	// TODO: not finished, should be run by different thread after advanced file informations are retrieved
	if (Object::GetAudioObject(id)->GetAudioProperty()->info.format.compare(L"MP3") == 0)
	{
		Info::GetID3Info(&Object::GetAudioObject(id)->GetAudioProperty()->info, Object::GetAudioObject(id)->GetPath());
	}
	else
	{
		item->info.title = Info::GetCompleteTitle(Object::GetAudioObject(id)->GetPath());
	}
#endif

	
	m_LoadedPathContainer[id] = path;

	return true;
}

void Audio::ResizeAllContainers(int size)
{
	if (size + currentContainersSize >= 0)
	{
		m_AudioObjectContainer.resize(size + currentContainersSize);
		m_LoadedPathContainer.resize(size + currentContainersSize);
		MP::UI::mdPlaylistButtonsContainer.reserve(size + currentContainersSize);
	}
}

void Audio::DeallocateAudioItems()
{
	for (s32 i = 0; i < m_AudioObjectContainer.size(); i++)
	{
		delete m_AudioObjectContainer[i];
	}

	m_AudioObjectContainer.clear();
	m_FolderContainer.clear();
	m_LoadedPathContainer.clear();
}

#ifdef _DEBUG_
void Audio::Folders::PrintContent()
{
	for (s32 i = 0; i < m_FolderContainer.size(); i++)
		std::cout << utf16_to_utf8(m_FolderContainer.at(i)) << std::endl;
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
	return 0; // TODO
}

u32 Audio::GetProcessedID3Tags()
{
	return 0; // TODO
}

void Audio::PrintTest()
{
	for (int i = 0; i < m_LoadedPathContainer.size(); i++)
	{
		std::cout << utf16_to_utf8(m_LoadedPathContainer[i]) << std::endl;
	}
}

#endif
