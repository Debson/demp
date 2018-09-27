#include "mp_audio.h"

#include <thread>
#include <mutex>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

#include "../app/realtime_system_application.h"
#include "../interface/md_interface.h"
#include "../player/music_player.h"
#include "../player/music_player_state.h"
#include "../playlist/music_player_playlist.h"
#include "../settings/music_player_settings.h"
#include "../settings/music_player_string.h"
#include "../graphics/music_player_graphics_playlist.h"
#include "../ui/music_player_ui.h"
#include "../utility/md_util.h"
#include "../utility/md_time.h"
#include "../utility/md_types.h"
#include "../utility/md_parser.h"
#include "../utility/utf8_to_utf16.h"


#define WORK_THREADS 4


namespace fs = boost::filesystem;
using namespace mdEngine;

namespace Audio
{
	static AddedFilesPathContainer		m_AddedFilesPathContainer;
	static AudioObjectContainer			m_AudioObjectContainer;

	static std::vector<Audio::Info::ID3*> m_ID3Container;

	static s32 currentContainersSize = 0;
	static s32 previousContainerSize = 0;
	static s32 currentlyLoadedItemsCount = 0;

	static s32 filesAddedCount = 0;

	static std::mutex mutex;

	Time::Timer lastFunctionCallTimer;
	Time::Timer lastPathPushTimer;
	b8 startLoadingProperties(false);
	b8 startLoadingPaths(false);
	b8 propertiesLoaded(true);
	b8 foldersRepSet(false);
	b8 insertFiles(false);
	b8 firstFilesLoaded(false);
	b8 firstFileFolderRepChecked(true);
	b8 filesLoadedFromFile(false);
	b8 droppedOnTop(false);
	b8 filesAddedByFileBrowser(false);

	// Flag that says if music files loaded from text are scanned for detailed info
	b8 filesInfoScanned(false);

	s32 indexOfDroppedOnItem = -1;

	b8 AddAudioItem(std::wstring& path, s32 id);
	void AddAudioItemWrap(std::vector<std::wstring*> vec, const s32 start, const s32 end);
	void ResizeAllContainers(int size);

	void SetFoldersRep();
	void CalculateDroppedPosInPlaylist();
}

void Audio::InitializeConfig()
{
	filesInfoScanned = Parser::GetInt(Strings::_PLAYLIST_FILE, Strings::_CONTENT_LOADED);
	s32 filesDuration = Parser::GetInt(Strings::_PLAYLIST_FILE, Strings::_CONTENT_DURATION);
	if (filesDuration <= 0 && filesInfoScanned > 0)
	{
		filesInfoScanned = 0;
	}
}

void Audio::StartAudio()
{
	
}

void Audio::FilesAddedByFileBrowser(b8 val)
{
	filesAddedByFileBrowser = val;
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

	if (State::CheckState(State::PathLoadedFromFileVolatile) == false)
	{
		auto indexesToRender = Graphics::MP::GetPlaylistObject()->GetIndexesToRender();
		
		

		s32 mouseX, mouseY;
		s32 winX, winY;
		App::Input::GetMousePosition(&mouseX, &mouseY);
		if (mouseY < MP::Data::_PLAYLIST_ITEMS_SURFACE_POS.y)
		{
			MP::UI::DeleteAllFiles();
			indexOfDroppedOnItem = 0;
			firstFilesLoaded = false;
			droppedOnTop = false;
			//md_log("File dropped on main player");
			return;
		}

		if (m_AudioObjectContainer.empty() == true)
			return;

		for (auto i : indexesToRender)
		{
			if (m_AudioObjectContainer[i]->topHasFocus)
			{
				//md_log(utf16_to_utf8(m_AudioObjectContainer[i]->GetTitle()));
				indexOfDroppedOnItem = i;
				insertFiles = true;
				droppedOnTop = true;
				//md_log("File dropped on index: " + std::to_string(i));
				return;
			}
			if (m_AudioObjectContainer[i]->bottomHasFocus)
			{
				//md_log(utf16_to_utf8(m_AudioObjectContainer[i]->GetTitle()));
				indexOfDroppedOnItem = i + 1;
				insertFiles = true;
				droppedOnTop = false;
				//md_log("File dropped on index: " + std::to_string(i));
				return;
			}

		}

		indexOfDroppedOnItem = m_AudioObjectContainer.size();
		if (m_AudioObjectContainer.empty() == false)
			insertFiles = true;
		else
			insertFiles = false;
		droppedOnTop = false;
		//md_log("File not dropped on playlist items");
	}
	
}

b8 Audio::LoadPathsFromFile(std::wstring& const path, Info::ID3* const id3)
{
	//if (fs::is_regular_file(path) == true)
	{
		//lastFunctionCallTimer.Start();
		m_AddedFilesPathContainer.push_back(&path);
		m_ID3Container.push_back(id3);
	}

	return true;
}

b8 Audio::PushToPlaylist(std::wstring& const path)
{
	lastFunctionCallTimer.Start();
	Audio::CalculateDroppedPosInPlaylist();

	if (fs::exists(path))
	{
		if (Info::IsPathLoaded(path) == true)
		{
			md_log("Audio item at path \"" + utf16_to_utf8(path) + "\" already loaded!\n");
			State::ResetState(State::FilesDroppedNotLoaded);
			return false;
		}
	

		if (fs::is_directory(path))
		{
			// Iterate throught all items in folder, if another folder found, resursively process it
			std::vector<std::wstring> tempFolderPathsVec;
			for (auto & i : fs::directory_iterator(path))
			{
				auto str = new std::wstring(i.path().wstring());
				if (Info::IsPathLoaded(*str) == true)
				{
					md_log("Audio item at path \"" + i.path().string() + "\" already loaded!\n");
					State::ResetState(State::FilesDroppedNotLoaded);
					delete str;
				}
				else
				{
					if (Info::CheckIfAudio(*str))
					{
						//CalculateDroppedPosInPlaylist();
						m_AddedFilesPathContainer.push_back(str);
					}
					else if (fs::is_directory(*str))
						tempFolderPathsVec.push_back(*str);
				}
			}

			for(auto & i : tempFolderPathsVec)
				PushToPlaylist(i);

			tempFolderPathsVec.clear();
		}
		else
		{
			if (Info::IsPathLoaded(path) == true)
			{
				md_log("Audio item at path \"" + utf16_to_utf8(path) + "\" already loaded!\n");
				State::ResetState(State::FilesDroppedNotLoaded);
			}
			else if (Info::CheckIfAudio(path) )
			{
				//CalculateDroppedPosInPlaylist();

				auto str = new std::wstring(path);

				m_AddedFilesPathContainer.push_back(str);
			}
		}
	}
	else
	{
		md_log("File at path \"" + utf16_to_utf8(path) + "\" does not exist!");
		State::ResetState(State::FilesDroppedNotLoaded);
	}

	return true;
}

void Audio::UpdateAudioLogic()
{
	if (State::CheckState(State::PathLoadedFromFileVolatile) == true)
	{
		filesLoadedFromFile = true;
	}

	if (lastFunctionCallTimer.GetTicksStart() > WAIT_TIME_BEFORE_NEXT_CALL)
	{
		startLoadingProperties = true;
		lastFunctionCallTimer.Stop();
	}

	if (State::CheckState(State::PathLoadedFromFile) == true &&
		m_AddedFilesPathContainer.size() > 0)
	{
		startLoadingProperties = true;
		State::ResetState(State::PathLoadedFromFile);
	}


	static f32 start = 0;
	if (startLoadingProperties == true && m_AddedFilesPathContainer.size() > 0)
	{
		start = SDL_GetTicks();
		startLoadingProperties = false;

		//State::SetState(State::AudioAdded);

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

		std::vector<std::wstring*> tempVec;
		if (insertFiles == true)
			tempVec = std::vector<std::wstring*>(indexOfDroppedOnItem);
		else
			tempVec = std::vector<std::wstring*>(currentContainersSize);

		// All used vectors must be resized before any operations are performed on them
		ResizeAllContainers(toProcessCount);

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
			if(filesLoadedFromFile == true)
				tt[i].join();
			else
				tt[i].detach();
		}

		State::ResetState(State::FileDropped);
		// Keep track of size of containers
		previousContainerSize = currentContainersSize;
		currentContainersSize += toProcessCount;

		insertFiles = false;

		/*if(m_AddedFilesFoldersPathContainer.empty() == false)
			foldersRepSet = false;*/

		if(m_AddedFilesPathContainer.empty() == false)
			foldersRepSet = false;
		
		/*if(filesLoadedFromFile == true)
			foldersRepSet = true;*/


		delete[] tt;
	}

	if (m_AudioObjectContainer.empty() == false && 
		State::CheckState(State::FileDropped) == false)
	{
		// If added files are loaded(only audio objets are created), set appropriate flag
		if (currentContainersSize == currentlyLoadedItemsCount)
		{
			State::ResetState(State::FilesDroppedNotLoaded);

			//md_log(SDL_GetTicks() - start);

			State::SetState(State::FilesLoaded);
			State::ResetState(State::PlaylistEmpty);
			firstFileFolderRepChecked = false;
			Audio::SetFoldersRep();
			firstFilesLoaded = true;
		}
		else
			State::ResetState(State::FilesLoaded);

		// If added files info is extraced, set appropriate flag
		if (currentContainersSize == Info::LoadedItemsInfoCount)
		{
			State::ResetState(State::FilesAddedInfoNotLoaded);

			State::SetState(State::FilesInfoLoaded);
			Info::SingleItemInfoLoaded = true;
		}
		else
			State::ResetState(State::FilesInfoLoaded);
	}
}

void Audio::PerformDeletion(s32 index, b8 smallDeletion)
{
	assert(index >= 0);
	assert(index < m_AudioObjectContainer.size());

	auto sepCont = Interface::Separator::GetContainer();

	// Find separator sub files container that has id of deleted index

	if (smallDeletion == true)
	{
		s32 sepIndex = -1;
		s32 sepSubIndex = -1;
		for (s32 i = 0; i < sepCont->size(); i++)
		{
			auto sepSubCont = sepCont->at(i).second->GetSubFilesContainer();
			if (*sepSubCont->at(0).first <= index && *sepSubCont->at(0).first + sepSubCont->size() > index)
			{
				for (s32 k = 0; k < sepSubCont->size(); k++)
				{
					if (*sepSubCont->at(k).first == index)
					{
						sepSubIndex = k;
						break;
					}
				}
				sepIndex = i;
				break;
			}
		}

		assert(sepIndex != -1);
		assert(sepSubIndex != -1);
		// erase item form sep sub files container that has the same index as deleted index
		auto sepSubCont = sepCont->at(sepIndex).second->GetSubFilesContainer();
		sepSubCont->erase(sepSubCont->begin() + sepSubIndex);

		// Delete playlist separators that have empty sub files containers
		for (s32 i = 0; i < sepCont->size(); i++)
		{
			auto sepSubCon = sepCont->at(i).second->GetSubFilesContainer();
			if (sepSubCon->empty() == true)
			{
				//delete sepCont->at(i).second;
				//sepCont->at(i).second = nullptr;
				sepCont->erase(sepCont->begin() + i);
			}
		}
	}

	m_AudioObjectContainer[index]->DeleteTexture();

	if (m_AudioObjectContainer.at(index)->IsFolderRep() == true &&
		index + 1 < m_AudioObjectContainer.size())
	{
		m_AudioObjectContainer.at(index + 1)->SetFolderRep(true);
	}
	/* Decrement all playlist item indexes that are greater than deleted pos
	   (to keep all indexes in in continuous ascending fashion)
	*/
	for (s32 i = index; i < m_AudioObjectContainer.size(); i++)
		m_AudioObjectContainer[i]->DecrementID();

	//delete m_AudioObjectContainer.at(index);
	//m_AudioObjectContainer.at(index) = nullptr;
	m_AudioObjectContainer.erase(m_AudioObjectContainer.begin() + index);

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
		State::SetState(State::PlaylistEmpty);
		firstFilesLoaded = false;
		indexOfDroppedOnItem = -1;
		currentContainersSize = 0;
		currentlyLoadedItemsCount = 0;
		Info::LoadedItemsInfoCount = 0;
	}
}

void Audio::AddAudioItemWrap(std::vector<std::wstring*> vec, const s32 beg, const s32 end)
{
	u32 start = SDL_GetTicks();
	s32 i = beg;
	for (; i < end; i++)
	{
		AddAudioItem(*vec[i], i);
	}
	//md_log(SDL_GetTicks() - start);

	/* Start loading files info after audio objects are created.
	   It will display items much faster on the screen.
	*/
	if (filesLoadedFromFile == false || filesInfoScanned == false)
	{
		i = beg;
		for (; i < end; i++)
		{
			if (mdEngine::IsAppClosing() == false)
			{
				Info::SingleItemInfoLoaded = false;
				Info::GetInfo(m_AudioObjectContainer[i]->GetID3Struct(), *vec[i]);
			}
		}
	}
}

b8 Audio::AddAudioItem(std::wstring& path, s32 id)
{
	// Lock the mutex
	std::lock_guard<std::mutex> lockGuard(mutex);

	// Need to get basic properties before creating audio object
	auto audioObject = std::make_shared<AudioObject>();
	if (filesLoadedFromFile == true)
	{
		//State::SetState(State::PathLoadedFromFile);
		audioObject->SetID3Struct(m_ID3Container[id]);
		m_ID3Container[id]->folder_rep == true ? audioObject->SetFolderRep(true) : (void)0;
	}
	else
	{
		audioObject->SetID3Struct(new Info::ID3());
		//auto str = new std::wstring(path);
	}

	audioObject->SetID(id);
	audioObject->SetPath(path);
	//audioObject->SetFolderPath(Info::GetFolderPath(str));
	audioObject->GetID3Struct()->format = Info::GetExt(path);
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
		/*	All files are loaded in a proper order as they should be in separators. Just create new separators for every fodler path
			and add sub files to it
		*/
		
		State::SetState(State::AudioAdded);
		State::SetState(State::ShuffleAfterLoad);
		State::ResetState(State::CurrentlyPlayingDeleted);

		auto sepCon = Interface::Separator::GetContainer();
		sepCon->clear();

		u32 start = SDL_GetTicks();
		std::wstring previousFolderPath = L"";
		Interface::PlaylistSeparator* ps = nullptr;
		s32 counter = 0;
		for (auto & i : m_AudioObjectContainer)
		{
			if (i->GetFolderPath().compare(previousFolderPath) != 0)
			{
				ps = new Interface::PlaylistSeparator(i->GetPath());
				ps->InitItem();
				i->SetFolderRep(true);
			}
			else
			{
				i->SetFolderRep(false);
			}
			
			previousFolderPath = i->GetFolderPath();

			ps->SeparatorSubFilePushBack(&i->GetID(), i->GetPath());

			counter++;
			if(filesLoadedFromFile == true && filesInfoScanned == true)
				Info::LoadedItemsInfoCount++;

		}
		if (Graphics::MP::GetPlaylistObject()->GetPlayingID() >= 0)
			 MP::Playlist::RamLoadedMusic.m_ID += m_AddedFilesPathContainer.size();

		filesLoadedFromFile = false;

		md_log(SDL_GetTicks() - start);

		/*for (auto & i : m_AddedFilesPathContainer)
		{
			delete i;
		}*/
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
			playlistButtonsCon->resize(size + currentContainersSize);
		}
		else
		{
			/* Create a space between indexOfDroppedOnItem and preceding item.
			*/
			

			s32 newSize = currentContainersSize + size;
			std::vector<std::wstring*> tempLoadedPathsVec(newSize);
			std::vector<std::shared_ptr<AudioObject>> tempAudioObjectVec(newSize);
			std::vector<std::pair<s32*, Interface::Button*>> tempPlaylistButtonsVec(newSize);

			for (s32 i = 0; i < indexOfDroppedOnItem; i++)
			{
				tempAudioObjectVec[i] = m_AudioObjectContainer[i];
				tempPlaylistButtonsVec[i] = playlistButtonsCon->at(i);
			}

			for (s32 i = indexOfDroppedOnItem + size, t = indexOfDroppedOnItem; t < currentContainersSize; i++, t++)
			{
				tempAudioObjectVec[i] = m_AudioObjectContainer[t];
				tempPlaylistButtonsVec[i] = playlistButtonsCon->at(t);

				// Every ID of items beneath must be incremented the same amount of times as the amount of file added
				for(s32 j = 0; j < size; j++)
					m_AudioObjectContainer[t]->IncrementID();
			}

			m_AudioObjectContainer.resize(size + currentContainersSize);
			playlistButtonsCon->resize(size + currentContainersSize);


			for (s32 i = 0; i < playlistButtonsCon->size(); i++)
			{
				playlistButtonsCon->at(i) = tempPlaylistButtonsVec[i];
			}
			m_AudioObjectContainer = tempAudioObjectVec;
			
			State::SetState(State::ContainersResized);
		}
	}
}

void Audio::DeallocateAudioItems()
{
	/*for (s32 i = 0; i < m_AudioObjectContainer.size(); i++)
	{
		delete m_AudioObjectContainer[i];
		m_AudioObjectContainer[i] = nullptr;
	}*/

	// Vector's destructor is called automatically so I dont have to clear them(recheck all containers)
	m_AudioObjectContainer.clear();
	m_ID3Container.clear();
	m_AddedFilesPathContainer.clear();
}

s32 Audio::GetFilesAddedCount()
{
	return filesAddedCount;
}

s32 Audio::GetDroppedOnIndex()
{
	return indexOfDroppedOnItem;
}


// Containers functions





Audio::AudioObjectContainer& Audio::Object::GetAudioObjectContainer()
{
	return m_AudioObjectContainer;
}

std::shared_ptr<Audio::AudioObject> Audio::Object::GetAudioObject(s32 id)
{
	if (id < m_AudioObjectContainer.size() && id >= 0 &&
		m_AudioObjectContainer.empty() == false &&
		m_AudioObjectContainer.at(id) != NULL)
	{

		return m_AudioObjectContainer.at(id);
	}

	return nullptr;
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

#ifdef _DEBUG_
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

#endif
