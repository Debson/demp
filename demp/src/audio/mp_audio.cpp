#include "mp_audio.h"

#include <thread>
#include <mutex>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <future>

#include "../app/realtime_system_application.h"
#include "../interface/md_interface.h"
#include "../interface/md_helper_windows.h"
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
	AudioObjectContainer				m_AudioObjectContainerTemp;

	std::vector<std::string> m_DroppedPathsContainer;
	std::vector<std::string> m_CommandLinePathsContainer;

	static std::vector<Audio::Info::ID3*> m_ID3Container;

	u32 DroppedItemsCount;

	static s32 currentContainersSize = 0;
	static s32 previousContainerSize = 0;
	static s32 currentlyLoadedItemsCount = 0;

	static s32 filesAddedCount;
	static u32 indexOfLoadingObject;

	static std::mutex mutex;
	static std::mutex fileAddMutex;

	static u8 threadsFinishedCount;
	static u8 workingThreadsCount;

	// Counts amount of times PushToPlaylist with @param: firstCall = true was called
	// By that, program can detect if all paths were processed and start creating audio objects
	u32 functionCallCount;

	// test
	b8 pathsProceeded(false);
	std::vector<std::string> pathCont;


	//u32 timeCount;

	u32 AudioContainerSizeBeforeDeletion = 0;

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
	b8 anyFileCorrupted(false);

	b8 cancelPathProcessing(false);

	s32 indexOfDroppedOnItem = -1;
	b8 droppedOnMainPlayer(false);

	b8 AddAudioItem(std::string& path, s32 id);
	void AddAudioItemWrap(std::vector<std::string*> vec, const s32 start, const s32 end);
	void ResizeAllContainers(int size);

	void SetFoldersRep();
	void CalculateDroppedPosInPlaylist();
	b8 CheckIfThreadsFinsishedWork();

	void LoadFilesInfoWrap(std::vector<std::shared_ptr<Audio::AudioObject>*>* tempVec);

	void ActiveLoadInfoWindow();

	void ResetStateFlags();

	void ListenForCommunicationFromChild();
}

void Audio::InitializeConfig()
{
	/*filesInfoScanned = Parser::GetInt(Strings::_PLAYLIST_FILE, Strings::_CONTENT_LOADED);
	filesInfoScanned ? State::SetState(State::FilesInfoLoaded) : State::ResetState(State::FilesInfoLoaded);
	s32 filesDuration = Parser::GetInt(Strings::_PLAYLIST_FILE, Strings::_CONTENT_DURATION);
	if (filesDuration <= 0 && filesInfoScanned > 0)
	{
		filesInfoScanned = 0;
	}*/
}

void Audio::StartAudio()
{
	State::SetState(State::FilesLoaded);
	State::SetState(State::FilesInfoLoaded);
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

		auto indexesToRender = Graphics::MP::GetPlaylistObject()->GetIndexesToRender();

		s32 mouseX, mouseY;
		s32 winX, winY;
		App::Input::GetMousePosition(&mouseX, &mouseY);
		droppedOnMainPlayer = false;
		if (mouseY < MP::Data::_PLAYLIST_ITEMS_SURFACE_POS.y && Window::mdLoadInfoWindow == nullptr)
		{
			State::ResetState(State::FilesDroppedNotLoaded);
			if(m_AudioObjectContainerTemp.empty() == true)
				m_AudioObjectContainerTemp = AudioObjectContainer(m_AudioObjectContainer);
			MP::UI::DeleteAllFiles();
			indexOfDroppedOnItem = 0;
			droppedOnMainPlayer = true;
			firstFilesLoaded = false;
			droppedOnTop = false;
			return;
		}

		if (m_AudioObjectContainer.empty() == true)
			return;

		//md_log(State::CheckState(State::AddedByFileBrowser));
		if (State::CheckState(State::AddedByFileBrowser) == false)
		{
			for (auto i : *indexesToRender)
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
		}

		indexOfDroppedOnItem = m_AudioObjectContainer.size();
		//if (m_AudioObjectContainer.empty() == false)
			insertFiles = true;
		//else
			//insertFiles = false;
		droppedOnTop = false;
		//md_log("File not dropped on playlist items");
	
}

b8 Audio::LoadPathsFromFile(std::string& path, Info::ID3* id3)
{
	//if (fs::is_regular_file(path) == true)
	{
		//lastFunctionCallTimer.Start();
		m_AddedFilesPathContainer.push_back(&path);
		m_ID3Container.push_back(id3);
	}

	return true;
}

void Audio::SaveDroppedPath(std::string path)
{
	m_DroppedPathsContainer.push_back(path);
}

void Audio::SavePathFromCommandLine(const char* path)
{
	m_CommandLinePathsContainer.push_back(path);
	State::SetState(State::AddedByCommandLine);
}

void Audio::OnDropComplete()
{
	State::SetState(State::PathsBeingProcessed);
	for (auto & i : m_DroppedPathsContainer)
		PushToPlaylist(i, true);
	m_DroppedPathsContainer.clear();
}

b8 Audio::PushToPlaylist(std::string path, b8 firstCall)
{
	if (cancelPathProcessing == true)
		return false;

	
	// Updates the vector with currently loaded paths, so it's up to date for IsPathLoaded function
	Info::Update();

	// Create wide string path, so filesystem will be able read paths properly
#ifdef _WIN32_
	std::wstring pathW = utf8_to_utf16(path);
#else
#define pathW path
#endif

	// Make note how many times this function was called directly from a SDL_DROPFILE event.
	if(firstCall == true)
		functionCallCount++;

	// Check if loaded path still exists
	if (fs::exists(pathW) == true)
	{
		if (m_AudioObjectContainer.empty() == false)
		{
			if (Info::IsPathLoaded(path) == true &&
				MP::Settings::IsPathExistenceCheckingEnabled == true)
			{
				md_log("Audio item at path \"" + path + "\" already loaded!\n");
				State::ResetStateOnLoadError();
				return false;
			}
		}
		
		if (fs::is_directory(pathW))
		{
			std::vector<std::string> tempFolderPathsVec;
			// Iterate throught all items in folder
			for (auto & i : fs::directory_iterator(pathW))
			{
				if (cancelPathProcessing == true)
					break;

				// Since every audio object will have only one copy of this path in RAM, create in now
#ifdef _WIN32_
				auto str = new std::string(utf16_to_utf8(i.path().wstring()));
#else

#endif
				if (Info::CheckIfAudio(*str) == true)
				{
					//CalculateDroppedPosInPlaylist();
					if (m_AudioObjectContainer.empty() == true)
					{
						// If playlist is empty, IsPathLoaded check dno't have to be run
						m_AddedFilesPathContainer.push_back(str);
						State::SetState(State::TerminateWorkingThreads);
						Audio::CalculateDroppedPosInPlaylist();
					}
					else if (Info::IsPathLoaded(*str) == true &&
							 MP::Settings::IsPathExistenceCheckingEnabled == true)
					{
						md_log("Audio item at path \"" + i.path().string() + "\" already loaded!\n");
						/*lastFunctionCallTimer.Stop();
						lastFunctionCallTimer.Reset();
						State::ResetStateOnLoadError();*/
						delete str;;
					}
					else
					{
						// Check if thath path is already loaded
						m_AddedFilesPathContainer.push_back(str);
						State::SetState(State::TerminateWorkingThreads);
						Audio::CalculateDroppedPosInPlaylist();
					}
				}
				else if (fs::is_directory(utf8_to_utf16(*str)))
				{
					// Iterated path is a folder, save it to process it later
					tempFolderPathsVec.push_back(*str);
				}
				else
				{
					delete str;
				}

			}


			// Iterate throught all the folders that were found in origin path
			for(auto & i : tempFolderPathsVec)
				PushToPlaylist(i, false);

			tempFolderPathsVec.clear();
		}
		else
		{
			if (Info::IsPathLoaded(path) == true &&
				MP::Settings::IsPathExistenceCheckingEnabled == true)
			{
				md_log("Audio item at path \"" + path + "\" already loaded!\n");
				State::ResetStateOnLoadError();
			}
			else if (Info::CheckIfAudio(path))
			{
#ifdef _WIN32_
				auto str = new std::string(utf16_to_utf8(pathW));
#else

#endif
				m_AddedFilesPathContainer.push_back(str);
				State::SetState(State::TerminateWorkingThreads);
				Audio::CalculateDroppedPosInPlaylist();
			}
		}
	}
	else
	{
		md_log("File at path \"" + path + "\" does not exist!");
		State::ResetStateOnLoadError();
		State::ResetState(State::FilesDroppedNotLoaded);
	}

	if (m_AddedFilesPathContainer.empty() == true)
	{
		State::ResetStateOnLoadError();
	}


	// Check if all that's the last function call and start process of creating audio objects
	if (firstCall == true && functionCallCount == DroppedItemsCount)
	{
		startLoadingProperties = true;
		DroppedItemsCount = 0;
		functionCallCount = 0;
	}

	return true;
}

void Audio::PushToPlaylistTest(std::string path, b8 firstCall)
{
	std::wstring pathW = utf8_to_utf16(path);


	if (fs::is_directory(pathW) == true)
	{
		for (auto & i : fs::directory_iterator(pathW))
		{
			if (fs::is_directory(i.path().wstring()) == true)
				Audio::PushToPlaylistTest(utf16_to_utf8(i.path().wstring()), false);
			else
				pathCont.push_back(i.path().string());
		}
	}
	else
	{
		pathCont.push_back(path);
	}


	static s32 count = 0;
	if (firstCall == true)
		count++;

	if (firstCall == true && DroppedItemsCount == count)
		pathsProceeded = true;

}

void Audio::UpdateAudioLogic()
{
	if (State::CheckState(State::Window::Exit) == true)
		return;

	if (m_CommandLinePathsContainer.empty() == false && State::CheckState(State::AppStarted) == true)
	{
		for (auto & i : m_CommandLinePathsContainer)
		{
			State::OnFileAddition();
			State::ResetState(State::AddedByFileBrowser);
			Audio::DroppedItemsCount++;
			PushToPlaylist(i, true);
		}
		m_CommandLinePathsContainer.clear();
	}


	if (State::CheckState(State::InitialLoadFromFile) == true &&
		m_AddedFilesPathContainer.size() > 0)
	{
		startLoadingProperties = true;
		filesLoadedFromFile = true;
	}

	if (cancelPathProcessing == true)
	{
		m_DroppedPathsContainer.clear();
		m_AddedFilesPathContainer.clear();
		cancelPathProcessing = false;
		startLoadingProperties = false;
		State::ResetStateOnLoadError();
	}

	// Files added, load them. If there were some errors durning loading config file, state ReloadFilesInfo is set.
	if ((startLoadingProperties == true && m_AddedFilesPathContainer.empty() == false) || 
		 State::CheckState(State::ReloadFilesInfo) == true)
	{
		//md_log(SDL_GetTicks() - timeCount);
		State::ResetState(State::PathsBeingProcessed);

		s32 toProcessCount = m_AddedFilesPathContainer.size();
		filesAddedCount = toProcessCount;

		// All used vectors must be resized before any operations are performed on them
		/*	Wait till copy of current state is created, so container resize won't affect visual
			display of playlist items.
		*/
		if (m_AudioObjectContainer.empty() == true ||
			State::CheckState(State::OldAudioObjectsSaved) == true)
		{
			ResizeAllContainers(toProcessCount);
			State::ResetState(State::OldAudioObjectsSaved);
		}
		else
		{
			return;
		}

		startLoadingProperties = false;


		std::vector<std::string*> tempVec;
		if (insertFiles == true)
			tempVec = std::vector<std::string*>(indexOfDroppedOnItem);
		else
			tempVec = std::vector<std::string*>(currentContainersSize);

		for (s32 i = 0; i < toProcessCount; i++)
			tempVec.push_back(m_AddedFilesPathContainer[i]);

		// Don't run all threads if work load is pretty low, shouldn't be hardcored tbh
		if (toProcessCount > 500)
			workingThreadsCount = WORK_THREADS;
		else
			workingThreadsCount = 1;

		

		// split the work between work threads
		s32 div = toProcessCount / workingThreadsCount;

		std::thread* tt = new std::thread[workingThreadsCount];
		// In some cases division will leave a remainder
		s8 rest = toProcessCount % workingThreadsCount;

		for (s8 i = 0; i < workingThreadsCount; i++)
		{
			if (i == workingThreadsCount - 1)
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

		State::ResetState(State::FilesLoaded);

		for (s8 i = 0; i < workingThreadsCount; i++)
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

		if(m_AddedFilesPathContainer.empty() == false)
			foldersRepSet = false;

		delete[] tt;
	}

	if (m_AddedFilesPathContainer.empty() == false &&
		m_AudioObjectContainer.empty() == true)
	{
		State::ResetState(State::FilesLoaded);
		Info::LoadedItemsInfoCount = 0;
	}

	if (m_AudioObjectContainer.empty() == false && 
		State::CheckState(State::FileDropped) == false)
	{
		currentContainersSize = m_AudioObjectContainer.size();
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
		{
			State::ResetState(State::FilesLoaded);
		}


		if (Info::LoadedItemsInfoCount > currentContainersSize)
			Info::LoadedItemsInfoCount = currentContainersSize;
		// If added files info is extraced, set appropriate flag
		if (currentContainersSize == Info::LoadedItemsInfoCount)
		{
			State::ResetState(State::FilesAddedInfoNotLoaded);
			//State::SetState(State::UpdatePlaylistInfoStrings);

			State::SetState(State::FilesInfoLoaded);
			State::ResetState(State::CommunicationWithChildCreated);
		}
		else
		{
			State::ResetState(State::FilesInfoLoaded);
		}
	}
	else if (m_AddedFilesPathContainer.size() > 500)
	{
		//State::ResetState(State::FilesLoaded);
		//State::ResetState(State::FilesInfoLoaded);
	}

	if (m_AudioObjectContainer.empty() == true && 
		m_AddedFilesPathContainer.empty() == true)
	{
		State::SetState(State::PlaylistEmpty);
	}

	// Listen if queue has any items, if has then process it
	if (Audio::GetProcessAlbumImageQueue()->empty() == false)
	{
		auto queue = Audio::GetProcessAlbumImageQueue();
		if (queue->front() != NULL)
		{
			m_AudioObjectContainer[*queue->front()]->LoadAlbumImageLargeSize();
		}
		
		Audio::GetProcessAlbumImageQueue()->clear();
	}

	ActiveLoadInfoWindow();

	ListenForCommunicationFromChild();

}

void Audio::PerformDeletion(s32 index, b8 smallDeletion)
{
	//assert(index >= 0);
	//assert(index < m_AudioObjectContainer.size());

	State::SetState(State::UpdatePlaylistInfoStrings);

	auto sepCont = Interface::Separator::GetContainer();
	auto audioCon = Audio::Object::GetAudioObjectContainer();


	// TODO add possibility to delete items when their info is still loading
	while(Info::ItemBeingProcessed == true) { }
		

	// Find separator sub files container that has id of deleted index

	Graphics::MP::GetPlaylistObject()->AddToItemsDuration(m_AudioObjectContainer[index]->GetLength() * -1);
	Graphics::MP::GetPlaylistObject()->AddToItemsSize(m_AudioObjectContainer[index]->GetObjectSize() * -1);

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
				sepCont->erase(sepCont->begin() + i);
			}
		}
	}
	else
	{
		sepCont->clear();
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

	if(m_AudioObjectContainer.empty() == true)
		md_log(m_AudioObjectContainer.size());

	// Decrement variables that manages the size of all containers and currently loaded items
	currentContainersSize--;
	currentlyLoadedItemsCount--;
	//if(State::CheckState(State::FilesInfoLoaded) == true)
		Info::LoadedItemsInfoCount--;

	// These values cant be less than 0
	currentContainersSize < 0 ? (currentContainersSize = 0) : 0;
	currentlyLoadedItemsCount < 0 ? (currentlyLoadedItemsCount = 0) : 0;
	Info::LoadedItemsInfoCount < 0 ? (Info::LoadedItemsInfoCount = 0) : 0;


	
	// If there is no audio objects, set playlist state to empty
	if (m_AudioObjectContainer.empty() == true)
	{
		Graphics::MP::GetPlaylistObject()->GetIndexesToRender()->clear();
		State::SetState(State::UpdatePlaylistInfoStrings);
		State::ResetState(State::FilesDroppedNotLoaded);
		State::ResetState(State::FilesAddedInfoNotLoaded);
		State::SetState(State::PlaylistEmpty);
		firstFilesLoaded = false;
		indexOfDroppedOnItem = 0;
		currentContainersSize = 0;
		currentlyLoadedItemsCount = 0;
		Info::LoadedItemsInfoCount = 0;
	}
}

void Audio::AddAudioItemWrap(std::vector<std::string*> vec, const s32 beg, const s32 end)
{
	u32 start = SDL_GetTicks();
	s32 i = beg;
	for (; i < end; i++)
	{
		if (Window::mdLoadInfoWindow != NULL)
		{
			if (Window::mdLoadInfoWindow->CancelWasPressed == true)
				break;
		}

		AddAudioItem(*vec[i], i);
	}
}

b8 Audio::AddAudioItem(std::string& path, s32 id)
{
	// Lock the mutex
	std::lock_guard<std::mutex> lockGuard(mutex);

	// Need to get basic properties before creating audio object
	auto audioObject = std::make_shared<AudioObject>();
	audioObject->SetID(id);
	audioObject->SetPath(path);

	if (filesLoadedFromFile == true)
	{
		//State::SetState(State::PathLoadedFromFile);
		audioObject->SetID3Struct(m_ID3Container[id]);
		audioObject->GetID3Struct()->format = Info::GetExt(audioObject->GetPath());
		//m_ID3Container[id]->is_processed == true ? audioObject->SetFolderRep(true) : (void)0;
	}
	else
	{
		audioObject->SetID3Struct(new Info::ID3());
		//auto str = new std::string(path);
	}
;
	m_AudioObjectContainer[id] = audioObject;

	audioObject->Init();
	
	indexOfLoadingObject = id;
	currentlyLoadedItemsCount++;

	return true;
}

void Audio::PerformSetFoldersRep()
{
	foldersRepSet = false;
	SetFoldersRep();
}

void Audio::SetFoldersRep()
{
	if (foldersRepSet == false)
	{
		/*	All files are loaded in a proper order as they should be in separators. Just create new separators for every fodler path
			and add sub files to it
		*/
		
		// ??????????????????

		// Info is loaded again after deletion, even though 
		State::ResetState(State::AudioDeleted);
		AudioContainerSizeBeforeDeletion = 0;


		State::ResetState(State::AddedByFileBrowser);
		State::SetState(State::AudioAdded);
		State::SetState(State::ShuffleAfterLoad);
		State::ResetState(State::CurrentlyPlayingDeleted);
		State::ResetState(State::PathContainerSorted);
		State::ResetState(State::TerminateWorkingThreads);
		State::ResetState(State::PathsBeingProcessed);
		Info::GetLoadedPathsContainer()->clear();

		auto sepCon = Interface::Separator::GetContainer();
		sepCon->clear();

		m_AudioObjectContainerTemp.clear();

		//u32 start = SDL_GetTicks();
		std::string previousFolderPath = "";
		Interface::PlaylistSeparator* ps = nullptr;
		anyFileCorrupted = false;
		filesInfoScanned = true;
		s32 counter = 0;
		for (auto & i : m_AudioObjectContainer)
		{
			i->SetID(counter);
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
			if (i->GetID3Struct()->loaded == false)
				filesInfoScanned = false;

			if (i->GetID3Struct()->infoCorrupted == false &&
				i->GetID3Struct()->loaded == true)
			{
				Info::LoadedItemsInfoCount++;
			}
			else if (i->GetID3Struct()->infoCorrupted == true)
				anyFileCorrupted = true;

		}
		if (Graphics::MP::GetPlaylistObject()->GetPlayingID() >= 0)
			 MP::Playlist::RamLoadedMusic.m_ID += m_AddedFilesPathContainer.size();


		//md_log(SDL_GetTicks() - start);

		
		m_AddedFilesPathContainer.clear();
		foldersRepSet = true;
		ResetStateFlags();

		sepCon = Interface::Separator::GetContainer();
		State::SetState(State::UpdatePlaylistInfoStrings);
		
		/*	If files are newly added or files are loaded from file but were not fully scanned,
			retrieve audio info
		*/
		if(filesLoadedFromFile == false || filesInfoScanned == false || anyFileCorrupted == true)
			LoadFilesInfo();

		filesLoadedFromFile = false;
	}
}

void Audio::ResizeAllContainers(int size)
{
	if (size + currentContainersSize >= 0)
	{
		if (insertFiles == false)
		{
			m_AudioObjectContainer.resize(size + currentContainersSize);
		}
		else
		{
			/* Create a space between indexOfDroppedOnItem and preceding item.
			*/
			

			s32 newSize = currentContainersSize + size;
			std::vector<std::string*> tempLoadedPathsVec(newSize);
			std::vector<std::shared_ptr<AudioObject>> tempAudioObjectVec(newSize);
			std::vector<std::pair<s32*, Interface::Button*>> tempPlaylistButtonsVec(newSize);

			for (s32 i = 0; i < indexOfDroppedOnItem; i++)
			{
				tempAudioObjectVec[i] = m_AudioObjectContainer[i];
			}

			for (s32 i = indexOfDroppedOnItem + size, t = indexOfDroppedOnItem; t < currentContainersSize; i++, t++)
			{
				tempAudioObjectVec[i] = m_AudioObjectContainer[t];

				// Every ID of items beneath must be incremented the same amount of times as the amount of file added
			}

			m_AudioObjectContainer.resize(size + currentContainersSize);
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

void Audio::ResetStateFlags()
{
	//State::ResetState(State::InitialLoadFromFile);
}

b8 Audio::CheckIfThreadsFinsishedWork()
{
	if (threadsFinishedCount == workingThreadsCount)
	{
		State::ResetState(State::SafeExitPossible);
		threadsFinishedCount = 0;
		return true;
	}

	return false;
}

void Audio::LoadFilesInfoWrap(std::vector<std::shared_ptr<AudioObject>*>* tempVec)
{
	u32 size = tempVec->size();
	for(u32 i = 0 ;i < size;)
	{
		// If deletion or addition occurs, exit thread that is currently working on retreiving info
		if (m_AudioObjectContainer.size() <= AudioContainerSizeBeforeDeletion ||
			State::CheckState(State::TerminateWorkingThreads) == true ||
			State::CheckState(State::Window::Exit) == true)
		{
			break;
		}

		if (State::CheckState(State::AudioDeleted) == false)
		{
			if (tempVec->at(i) == nullptr)
			{
				i++;
				continue;
			}
			Info::GetInfo(*tempVec->at(i));
			i++;
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			break;
		}
	}
	
	threadsFinishedCount++;
	CheckIfThreadsFinsishedWork();

	delete tempVec;
}

void Audio::LoadFilesInfo()
{
	u8 threadsToUse;
	if (m_AudioObjectContainer.size() > 500)
		threadsToUse = WORK_THREADS;
	else
		threadsToUse = 1;

	u32 corruptedFilesCount = 0;
	if (anyFileCorrupted == true)
	{
		for (auto & i : m_AudioObjectContainer)
		{
			if (i->GetID3Struct()->infoCorrupted == true)
				corruptedFilesCount++;
		}

		if (corruptedFilesCount > 500)
			threadsToUse = WORK_THREADS;
		else
			threadsToUse = 1;
	}

	std::thread* tt = new std::thread[threadsToUse];;

	s32 div = m_AudioObjectContainer.size() / threadsToUse;

	s32 rest = m_AudioObjectContainer.size() % threadsToUse;
	for (u8 i = 0; i < threadsToUse; i++)
	{
		auto tempVec = new std::vector<std::shared_ptr<Audio::AudioObject>*>;
		if (i == threadsToUse - 1)
		{
			tempVec->resize(div + rest);
			for (s32 k = div * i, j = 0; k < (i + 1) * div + rest; k++, j++)
			{
				if (m_AudioObjectContainer[k]->GetID3Struct()->loaded == false ||
					m_AudioObjectContainer[k]->GetID3Struct()->infoCorrupted == true)
				{
					tempVec->at(j) = &m_AudioObjectContainer[k];
				}
			}
			md_log_compare(div * i, (i + 1) * div + rest);
		}
		else
		{
			tempVec->resize(div);
			for (s32 k = div * i, j = 0; k < (i + 1) * div; k++, j++)
			{
				if (m_AudioObjectContainer[k]->GetID3Struct()->loaded == false ||
					m_AudioObjectContainer[k]->GetID3Struct()->infoCorrupted == true)
				{
					tempVec->at(j) = &m_AudioObjectContainer[k];
				}
			}
			md_log_compare(div * i, (i + 1) * div);
		}

		tt[i] = std::thread(LoadFilesInfoWrap, tempVec);
	}


	for (u8 i = 0; i < threadsToUse; i++)
	{
		if (tt[i].joinable() == true)
			tt[i].detach();
	}


	delete[] tt;
}

void Audio::ActiveLoadInfoWindow()
{
#if 1
	//md_log(State::CheckState(State::PathsBeingProcessed));
	
	// Files are loading and windows hasn't been created
	if (State::CheckState(State::PathsBeingProcessed) == true &&
		m_AddedFilesPathContainer.size() > 500 &&
		Window::mdLoadInfoWindow == nullptr)
	{
		Window::mdLoadInfoWindow = new Window::LoadInfoWindow(glm::vec2(600, 100), glm::vec4(Window::GetWindowPos(), MP::Data::_DEFAULT_PLAYER_SIZE.x,
																					 Window::WindowProperties.m_ApplicationHeight));
		Window::WindowsContainer.insert(std::pair< std::string, Window::WindowObject*>("LoadInfoWindow", Window::mdLoadInfoWindow));
	}

	// Window is created, can safely update it
	if (Window::mdLoadInfoWindow != nullptr)
	{
		//md_log("load info window opened");
		Window::mdLoadInfoWindow->Update();
	}

	// Files are loaded, delete window
	if(State::CheckState(State::FilesLoaded) == true &&
		State::CheckState(State::PathsBeingProcessed) == false &&
		Window::mdLoadInfoWindow != NULL)
	{
		delete Window::mdLoadInfoWindow;

		Window::mdLoadInfoWindow = nullptr;
	}

	// Manage window only isn't null
	if(Window::mdLoadInfoWindow != nullptr)
	{ 
		if(Window::mdLoadInfoWindow->CancelWasPressed == true)
		{
			auto test = &m_AudioObjectContainer;

			// If were processed and are being loaded iterate throught this if statemtent
			if (State::CheckState(State::PathsBeingProcessed) == true)
			{
				// If paths weren't processed iterate throught this block
				DroppedItemsCount = 0;
				functionCallCount = 0;
				cancelPathProcessing = true;
				m_AddedFilesPathContainer.clear();

				State::SetState(State::FilesLoaded);
				State::ResetState(State::PathsBeingProcessed);
			}
			else
			{
				u32 size = m_AudioObjectContainer.size();
				for (s32 i = size - 1; i >= 0; i--)
				{
					if (i >= indexOfDroppedOnItem && i < indexOfDroppedOnItem + filesAddedCount)
					{
						m_AudioObjectContainer.erase(m_AudioObjectContainer.begin() + i);
					}
				}

				if (droppedOnMainPlayer == true && m_AudioObjectContainerTemp.empty() == false)
				{
					m_AudioObjectContainer = AudioObjectContainer(m_AudioObjectContainerTemp);
					m_AudioObjectContainerTemp.clear();
					md_log("main player");
					f64 size = 0;
					f64 duration = 0;
					for (auto & i : m_AudioObjectContainer)
					{
						size += i->GetID3Struct()->size;
						duration += i->GetID3Struct()->length;
					}
					Graphics::MP::GetPlaylistObject()->SetItemsSize(size);
					Graphics::MP::GetPlaylistObject()->SetItemsDuration(duration);

				}
				else
				{
					m_AudioObjectContainer.resize(previousContainerSize);
				}


				State::SetState(State::AudioAdded);
				State::SetState(State::FilesLoaded);
				State::ResetState(State::PathsBeingProcessed);
				State::SetState(State::ShuffleAfterLoad);
				State::ResetState(State::PathContainerSorted);
				State::ResetState(State::TerminateWorkingThreads);
				//State::SetState(State::UpdatePlaylistInfoStrings);
				State::ResetState(State::FilesAddedInfoNotLoaded);
				State::ResetState(State::FilesDroppedNotLoaded);

				Info::GetLoadedPathsContainer()->clear();


				firstFilesLoaded = false;
				indexOfDroppedOnItem = 0;
			}

			if (droppedOnMainPlayer == true || State::CheckState(State::FilesLoaded) == true)
			{
				currentContainersSize = m_AudioObjectContainer.size();
			}
			else
			{
				currentContainersSize = previousContainerSize;
			}
			currentlyLoadedItemsCount = currentContainersSize;
			Info::LoadedItemsInfoCount = currentContainersSize;
			ResetStateFlags();

			m_AddedFilesPathContainer.clear();

			md_log_compare(indexOfDroppedOnItem, filesAddedCount);
			Window::mdLoadInfoWindow->CancelWasPressed = false;

			delete Window::mdLoadInfoWindow;
			Window::mdLoadInfoWindow = nullptr;
		}

	}


#endif // 0

#if 0
	static b8 active = false;

	if (App::Input::IsKeyPressed(App::KeyCode::F6) == true)
	{
		active = !active;
	}

	if (mdLoadInfoWindow.CancelWasPressed == true)
	{

		//MP::Playlist::DeleteMusic();
		active = !active;
		mdLoadInfoWindow.CancelWasPressed = false;
	}

	if (active)
	{
		mdLoadInfoWindow.Init(glm::vec4(Window::GetWindowPos(), Window::windowProperties.mWindowWidth,
										Window::windowProperties.mApplicationHeight));
		mdLoadInfoWindow.Update();
	}
	else
	{
		mdLoadInfoWindow.Free();
	}



#endif

}

void Audio::ListenForCommunicationFromChild()
{
	if (fs::exists(utf8_to_utf16(Strings::_TEMP_CHILD_CONSOLE_ARG)) == true &&
		State::CheckState(State::CommunicationWithChildCreated) == false)
	{
		State::SetState(State::CommunicationWithChildCreated);
		std::fstream file;
		char buff[1024];
		file.open(utf8_to_utf16(Strings::_TEMP_CHILD_CONSOLE_ARG), std::ios::in | std::ios::binary);
		if (file.is_open() == false)
		{
			MD_ERROR("Error: Could not open a file for read!\n");
		}

		file.getline(buff, 1024);
		//SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "File found!", buff, NULL);

		file.close();
		fs::remove(utf8_to_utf16(Strings::_TEMP_CHILD_CONSOLE_ARG));

		// Delete all files from playlist and add detected file
		MP::UI::DeleteAllFiles();
		MP::Playlist::StopMusic();
		State::OnFileAddition();
		State::ResetState(State::AddedByFileBrowser);
		Audio::DroppedItemsCount++;
		PushToPlaylist(buff, true);
		State::SetState(State::AddedByCommandLine);
		if(State::CheckState(State::Window::InTray) == true)
			State::SetState(State::AudioChangedInTray);
	}
}

u32 Audio::GetIndexOfLoadingObject()
{
	return indexOfLoadingObject;
}

// Containers functions


Audio::AudioObjectContainer* Audio::Object::GetAudioObjectContainer()
{
	return &m_AudioObjectContainer;
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
		std::cout << "Path: "<< m_AudioObjectContainer[i]->GetPath() << std::endl;
		std::cout << "Fodler path: " << m_AudioObjectContainer[i]->GetFolderPath() << std::endl;
		std::cout << "Artist: "<< m_AudioObjectContainer[i]->GetArtist() << std::endl;
		std::cout << "Title: " << m_AudioObjectContainer[i]->GetTitle() << std::endl;
		std::cout << "Track nr: " << m_AudioObjectContainer[i]->GetTrackNum() << std::endl;
		std::cout << "Album: " << m_AudioObjectContainer[i]->GetAlbum() << std::endl;
		std::cout << "Year: " << m_AudioObjectContainer[i]->GetYear() << std::endl;
		std::cout << "Comment: " << m_AudioObjectContainer[i]->GetComment() << std::endl;
		std::cout << "Genre: " << m_AudioObjectContainer[i]->GetGenre() << std::endl;
		std::cout << "Ext: " << m_AudioObjectContainer[i]->GetFormat() << std::endl;
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
