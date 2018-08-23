#include "mp_audio.h"

#include <thread>
#include <mutex>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

#include "../app/realtime_system_application.h"
#include "../interface/md_interface.h"
#include "../player/music_player.h"
#include "../player/music_player_state.h"
#include "../settings/music_player_settings.h"
#include "../graphics/music_player_graphics.h"
#include "../ui/music_player_ui.h"
#include "../utility/md_util.h"
#include "../utility/md_time.h"
#include "../utility/md_types.h"
#include "../utility/utf8_to_utf16.h"

#define WORK_THREADS 4


namespace fs = boost::filesystem;
using namespace mdEngine;

namespace Audio
{
	static std::vector<AudioObject*> m_AudioObjectContainer;		
	static std::vector<std::wstring> m_FolderContainer;					// Stores only paths to folders
	static std::vector<std::wstring> m_LoadedPathContainer;				// Stores all paths of supported files	that are currently loaded into playlist
	static std::vector<std::wstring> m_AddedFilesPathContainer;			// Stores all unique valid paths of supported file formats just from one event(drag and drop, file expl. folder expl.)
	static std::vector<std::wstring> m_AddedFilesFoldersPathContainer;	// Stores all unqiue folders paths of supported file formats just from one event(drag and drop, file expl. folder expl.)
	static std::vector<std::wstring> m_InitPaths;						// Stores paths were initially loaded on actions(drag and drop, file expl. folder expl.)
	static std::vector<Audio::Info::ID3> m_ID3Container;				// Used when files are loaded from text file
	static std::vector<std::pair<std::wstring, Interface::PlaylistSeparator*>> m_PlaylistItemFolderContainer;



	static s32 currentContainersSize = 0;
	static s32 currentlyLoadedItemsCount = 0;

	static std::mutex mutex;

	Time::Timer lastFunctionCallTimer;
	Time::Timer lastPathPushTimer;
	b8 startLoadingProperties(false);
	b8 startLoadingPaths(false);
	b8 propertiesLoaded(true);
	b8 foldersRepSet(false);
	b8 insertFiles(false);
	b8 firstFilesLoad(false);
	s32 indexOfDroppedOnItem = -1;
	s32 filesAddedCount = 0;



	void PushToPlaylistWrap();
	b8 AddAudioItem(std::wstring path, s32 id);
	void AddAudioItemWrap(const std::vector<std::wstring> vec, const s32 start, const s32 end);
	void ResizeAllContainers(int size);

	void SetFoldersRep();
	void CalculateDroppedPosInPlaylist();


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


void Audio::CalculateDroppedPosInPlaylist()
{
	
	/*
		I have an exact position of item on which file was dropped. Now when file was dropped
		resize container to its size + dropped items count, and move all item that were beneath
		file on which item was dropped, move to end of resized container. 
		Then add new files into that gap. Files will have ids: 
		ID of item on which files were dropped - to - item that was beneath that file.
	
	*/

	if (m_AudioObjectContainer.empty() == false && State::PathLoadedFromFile == false)
	{
		s32 min = Graphics::MP::GetPlaylistObject()->GetCurrentMinIndex();
		s32 max = Graphics::MP::GetPlaylistObject()->GetCurrentMaxIndex();


		for (s32 i = min; i < max; i++)
		{
			if (m_AudioObjectContainer[i]->hasFocus)
			{
				//md_log(utf16_to_utf8(m_AudioObjectContainer[i]->GetTitle()));
				indexOfDroppedOnItem = i;
				insertFiles = true;
			}
		}

	}
}

b8 Audio::SavePathFiles(std::wstring path, const Info::ID3 id3)
{
	if (Info::CheckIfAlreadyLoaded(&m_LoadedPathContainer, path) == true)
	{
		MD_ERROR("Audio item at path \"" + utf16_to_utf8(path) + "\" already loaded!\n");
		return false;
	}

	lastPathPushTimer.start();
	m_InitPaths.push_back(path);
	m_ID3Container.push_back(id3);

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
	

		if (fs::is_directory(path))
		{
			if (Info::CheckIfHasItems(path) == true 
				&& Info::CheckIfAlreadyLoaded(&m_FolderContainer, path) == false)
			{
				m_FolderContainer.push_back(path);;


				//auto folderObject = new Interface::PlaylistSeparator(Info::GetFolder(path));
				//folderObject->InitItem();
			}

			if (Info::CheckIfHasItems(path) == true
				&& Info::CheckIfAlreadyLoaded(&m_AddedFilesFoldersPathContainer, path) == false)
			{
				m_AddedFilesFoldersPathContainer.push_back(path);
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
					else if (Info::CheckIfAudio(i.path().wstring()))
					{
						CalculateDroppedPosInPlaylist();
						m_AddedFilesPathContainer.push_back(i.path().wstring());
					}
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
				std::wstring folderPath = Info::GetFolderPath(path);
				if (Info::CheckIfAlreadyLoaded(&m_FolderContainer, folderPath) == false)
				{
					m_FolderContainer.push_back(folderPath);

					//auto folderObject = new Interface::PlaylistSeparator(Info::GetFolder(path));
					//folderObject->InitItem();
				}

				if (Info::CheckIfAlreadyLoaded(&m_AddedFilesFoldersPathContainer, folderPath) == false)
				{
					m_AddedFilesFoldersPathContainer.push_back(folderPath);
				}

				CalculateDroppedPosInPlaylist();

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

		mdEngine::MP::musicPlayerState = mdEngine::MP::MusicPlayerState::kMusicAdded;

		s32 toProcessCount = m_AddedFilesPathContainer.size();
		filesAddedCount = toProcessCount;

		// Don't run all threads if work load is pretty low
		s8 threadsToUse = 0;
		if (toProcessCount > 500)
			threadsToUse = WORK_THREADS;
		else
			threadsToUse = 1;

		// split the work between work threads
		s32 div = toProcessCount / threadsToUse;

		// All used vectors must be resized before any operations are performed on them
		ResizeAllContainers(toProcessCount);

		std::vector<std::wstring> tempVec;
		if (insertFiles == true)
		{
			tempVec = std::vector<std::wstring>(indexOfDroppedOnItem);
		}
		else
		{
			tempVec = std::vector<std::wstring>(currentContainersSize);
		}

		for (s32 i = 0; i < toProcessCount; i++)
			tempVec.push_back(m_AddedFilesPathContainer[i]);


		std::thread* tt = new std::thread[threadsToUse];

		// In some cases division will leave a remainder
		s8 rest = toProcessCount % threadsToUse;

		for (s8 i = 0; i < threadsToUse; i++)
		{
			if (i == threadsToUse - 1)
			{
				if (insertFiles == false)
				{

					tt[i] = std::thread(AddAudioItemWrap, tempVec, div * i + currentContainersSize,
						div * (i + 1) + rest + currentContainersSize);
				}
				else
				{
					tt[i] = std::thread(AddAudioItemWrap, tempVec, div * i + indexOfDroppedOnItem,
						div * (i + 1) + rest + indexOfDroppedOnItem);

				}
			}
			else
			{
				if (insertFiles == false)
				{
					tt[i] = std::thread(AddAudioItemWrap, tempVec, div * i + currentContainersSize,
						div * (i + 1) + currentContainersSize);
				}
				else
				{
					tt[i] = std::thread(AddAudioItemWrap, tempVec, div * i + indexOfDroppedOnItem,
						div * (i + 1) + indexOfDroppedOnItem);

				}
			}
		}

		for (s8 i = 0; i < threadsToUse; i++)
		{
			assert(tt[i].joinable());
			tt[i].detach();
		}

		// Keep track of size of containers
		currentContainersSize += toProcessCount;

		insertFiles = false;

		if(m_AddedFilesFoldersPathContainer.empty() == false)
			foldersRepSet = false;
		

		delete[] tt;
	}

	if (m_AudioObjectContainer.empty() == false)
	{
		// If added files are loaded(only audio objets are created), set appropriate flag
		if (currentContainersSize == currentlyLoadedItemsCount)
		{
			State::MusicFilesLoaded = true;
			State::IsPlaylistEmpty = false;
			Audio::SetFoldersRep();
			firstFilesLoad = true;
		}
		else
			State::MusicFilesLoaded = false;

		// If added files info is extraced, set appropriate flag
		if (currentContainersSize == Info::LoadedItemsInfoCount)
		{
			State::MusicFilesInfoLoaded = true;
		}
		else
			State::MusicFilesInfoLoaded = false;
	}
}

void Audio::PerformDeletion(s32 index)
{
	assert(index >= 0);

	delete m_AudioObjectContainer.at(index);
	m_AudioObjectContainer.erase(m_AudioObjectContainer.begin() + index);

	/* Decrement all playlist item indexes that are greater than deleted pos
	   (to keep all indexes in in continuous ascending fashion)
	*/
	for (s32 i = index; i < m_AudioObjectContainer.size(); i++)
		m_AudioObjectContainer[i]->DecrementID();


	// Get deleted item folder's path before deleting it's path 
	std::wstring folderPath = Info::GetFolderPath(m_LoadedPathContainer[index]);

	// Delete that path from loaded paths container
	m_LoadedPathContainer.erase(m_LoadedPathContainer.begin() + index);

	// Check if any of audio object's folder path is the same as deleted item' folder path
	auto it = std::find_if(m_AudioObjectContainer.begin(), m_AudioObjectContainer.end(),
		[&](const AudioObject* ref)
	{ 
		return ref->GetFolderPath().compare(folderPath) == 0;
	});

	// Finally check if there is no audio items from that folder's path, then erase this folder's path
	if (it == m_AudioObjectContainer.end())
	{
		auto itFolder = std::find(m_FolderContainer.begin(), m_FolderContainer.end(), folderPath);
		m_FolderContainer.erase(itFolder);
	}

	// This is equal to: (current size - 1)
	ResizeAllContainers(-1);

	// Decrement variables that manages the size of all containers and currently loaded items
	currentContainersSize--;
	currentlyLoadedItemsCount--;
	Info::LoadedItemsInfoCount--;

	// These values cant be less than 0
	currentContainersSize < 0 ? (currentContainersSize = 0) : 0;
	currentlyLoadedItemsCount < 0 ? (currentlyLoadedItemsCount = 0) : 0;
	Info::LoadedItemsInfoCount < 0 ? (Info::LoadedItemsInfoCount = 0) : 0;

	// If there is no audio objects, set playlist state to empty
	if (m_AudioObjectContainer.empty() == true)
	{
		State::IsPlaylistEmpty = true;
		m_FolderContainer.clear();
		indexOfDroppedOnItem = -1;
	}
}

std::vector<std::wstring>& Audio::Folders::GetAudioFoldersContainer()
{
	return m_FolderContainer;
}

std::wstring Audio::Folders::GetAudioFolder(s32 id)
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

std::vector<Audio::AudioObject*>& Audio::Object::GetAudioObjectContainer()
{
	return m_AudioObjectContainer;
}

Audio::AudioObject* Audio::Object::GetAudioObject(s32 id)
{
	if (id < m_AudioObjectContainer.size() && id >= 0 &&
		m_AudioObjectContainer.empty() == false &&
		m_AudioObjectContainer.at(id) != NULL)
	{

		return m_AudioObjectContainer.at(id);
	}

	return NULL;
}

u32 Audio::Object::GetSize()
{
	return currentlyLoadedItemsCount;
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

	/* Start loading files info after audio objects are created.
	   It will display items much faster on the screen.
	*/
	i = beg;
	for (; i < end; i++)
	{
		if (State::PathLoadedFromFile == false)
			Info::GetInfo(&m_AudioObjectContainer[i]->GetID3Struct(), vec[i]);
	}
}

b8 Audio::AddAudioItem(std::wstring path, s32 id)
{
	// Lock the mutes
	std::lock_guard<std::mutex> lockGuard(mutex);

	if (Info::CheckIfAudio(path) == false)
		return false;

	m_LoadedPathContainer[id] = path;
	// Need to get basic properties before creating audio object
	auto audioObject = new AudioObject();
	if (State::PathLoadedFromFile == true)
	{
		mdEngine::MP::musicPlayerState = mdEngine::MP::MusicPlayerState::kMusicAddedFromFile;
		audioObject->GetID3Struct() = m_ID3Container[id];
	}

	audioObject->SetID(id);
	audioObject->SetPath(path);
	audioObject->SetFolderPath(Info::GetFolderPath(path));
	audioObject->GetID3Struct().format = Info::GetExt(path);
	m_AudioObjectContainer[id] = audioObject;

	audioObject->Init();

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

	currentlyLoadedItemsCount++;

	return true;
}

void Audio::SetFoldersRep()
{
	if (foldersRepSet == false)
	{
		/* Every file's folder added to the playlist IN ONE EVENT is stored in m_AddedFilesFoldersPathContainer.
		   For every folder's path in that container, find a FIRST audio file from that folder path and then
		   find an audio objects with exactly the same path as that audio file and set it as that folder representant.

		   NOTE: If folder's path of file on which audio file(or folder with audio files) was dropped is the same
				 as folder's path of dropped audio file(or folder with audio files), don't set an another folder repesentant
				 (because file on which audio files were dropped is that folder's rep or another file from that folder is 
				 already a folder representant).
		*/
		for (s32 i = 0; i < m_AddedFilesFoldersPathContainer.size(); i++)
		{
			std::wstring folderPath(m_AddedFilesFoldersPathContainer[i]);

			for (s32 k = 0; k < m_AddedFilesPathContainer.size(); k++)
			{
				std::wstring filePath = m_AddedFilesPathContainer[k];
				b8 statementLogic(false);
				if (indexOfDroppedOnItem >= 0)
				{
					std::wstring droppedOnFolderPath = m_AudioObjectContainer[indexOfDroppedOnItem + filesAddedCount]->GetFolderPath();
					statementLogic = Info::GetFolderPath(filePath).compare(droppedOnFolderPath) != 0;
				}
				else
				{
					statementLogic = folderPath.compare(Info::GetFolderPath(filePath)) == 0;
				}

				if (statementLogic == true)
				{
					for (auto & i : m_AudioObjectContainer)
					{
						if (i->GetPath().compare(filePath) == 0)
							i->SetAsFolderRep();
					}
				}
				break;
			}
		}

		m_AddedFilesFoldersPathContainer.clear();
		m_AddedFilesPathContainer.clear();
		foldersRepSet = true;
	}
}

void Audio::ResizeAllContainers(int size)
{
	if (size + currentContainersSize >= 0)
	{
		auto playlistButtonsCon = Interface::PlaylistButton::GetContainer();

		if (insertFiles == false)
		{
			m_AudioObjectContainer.resize(size + currentContainersSize);
			m_LoadedPathContainer.resize(size + currentContainersSize);
			playlistButtonsCon->resize(size + currentContainersSize);
		}
		else
		{
			/* Create a space between indexOfDroppedOnItem and preceding item.
			*/

			s32 newSize = currentContainersSize + size;
			std::vector<std::wstring> tempLoadedPathsVec(newSize);
			std::vector<AudioObject*> tempAudioObjectVec(newSize);
			std::vector<std::pair<s32*, Interface::Button*>> tempPlaylistButtonsVec(newSize);

			for (s32 i = 0; i < indexOfDroppedOnItem; i++)
			{
				tempLoadedPathsVec[i] = m_LoadedPathContainer[i];
				tempAudioObjectVec[i] = m_AudioObjectContainer[i];
				tempPlaylistButtonsVec[i] = playlistButtonsCon->at(i);
			}

			for (s32 i = indexOfDroppedOnItem + size, t = indexOfDroppedOnItem; t < currentContainersSize; i++, t++)
			{
				tempLoadedPathsVec[i] = m_LoadedPathContainer[t];
				tempAudioObjectVec[i] = m_AudioObjectContainer[t];
				tempPlaylistButtonsVec[i] = playlistButtonsCon->at(t);

				// Every ID of items beneath must be incremented the same amount of times as the amount of file added
				for(s32 j = 0; j < size; j++)
					m_AudioObjectContainer[t]->IncrementID();
			}

			m_AudioObjectContainer.resize(size + currentContainersSize);
			m_LoadedPathContainer.resize(size + currentContainersSize);
			playlistButtonsCon->resize(size + currentContainersSize);


			for (s32 i = 0; i < playlistButtonsCon->size(); i++)
			{
				playlistButtonsCon->at(i) = tempPlaylistButtonsVec[i];
			}
			m_LoadedPathContainer = tempLoadedPathsVec;
			m_AudioObjectContainer = tempAudioObjectVec;

		}
	}
}

void Audio::DeallocateAudioItems()
{
	for (s32 i = 0; i < m_AudioObjectContainer.size(); i++)
	{
		delete m_AudioObjectContainer[i];
		m_AudioObjectContainer[i] = nullptr;
	}

	// Vector's destructor is called automatically so I dont have to clear them(recheck all containers)
	m_AudioObjectContainer.clear();
	m_FolderContainer.clear();
	m_LoadedPathContainer.clear();
	m_ID3Container.clear();
	m_InitPaths.clear();
	m_AddedFilesPathContainer.clear();
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
