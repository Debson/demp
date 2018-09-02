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
#include "../settings/music_player_string.h"
#include "../graphics/music_player_graphics.h"
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
	static PlaylistItemFolderContainer	m_PlaylistItemFolderContainer;
	static AddedFilesFolderContainer	m_AddedFilesFoldersPathContainer;
	static AddedFilesPathContainer		m_AddedFilesPathContainer;
	static AudioObjectContainer			m_AudioObjectContainer;
	static LoadedPathContainer			m_LoadedPathContainer;		
	static FolderContainer				m_FolderContainer;	

	static std::vector<Audio::Info::ID3> m_ID3Container;
	static std::vector<std::wstring> m_InitPaths;


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




	void PushToPlaylistWrap();
	b8 AddAudioItem(std::wstring path, s32 id);
	void AddAudioItemWrap(const std::vector<std::wstring> vec, const s32 start, const s32 end);
	void ResizeAllContainers(int size);

	void SetFoldersRep();
	void CalculateDroppedPosInPlaylist();


}

void Audio::StartAudio()
{
	std::string file = Strings::_PATHS_FILE;
	filesInfoScanned = Parser::GetInt(file, Strings::_CONTENT_LOADED);
	
}

b8 Audio::SavePathFiles(std::wstring path)
{
	if (Info::CheckIfAlreadyLoaded(&m_LoadedPathContainer, path) == true)
	{
		MD_ERROR("Audio item at path \"" + utf16_to_utf8(path) + "\" already loaded!\n");
		return false;
	}

	filesLoadedFromFile = true;
	
	lastPathPushTimer.start();
	m_InitPaths.push_back(path);

	return true;
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
		s32 min = Graphics::MP::GetPlaylistObject()->GetCurrentMinIndex();
		s32 max = Graphics::MP::GetPlaylistObject()->GetCurrentMaxIndex();

		s32 mouseX, mouseY;
		s32 winX, winY;
		App::Input::GetMousePosition(&mouseX, &mouseY);
		if (mouseY < MP::Data::_PLAYLIST_ITEMS_SURFACE_POS.y)
		{
			MP::UI::DeleteAllFiles();
			indexOfDroppedOnItem = 0;
			firstFilesLoaded = false;
			droppedOnTop = false;
			md_log("File dropped on main player");
			return;
		}


		for (s32 i = min; i < max; i++)
		{
			if (m_AudioObjectContainer[i]->topHasFocus)
			{
				//md_log(utf16_to_utf8(m_AudioObjectContainer[i]->GetTitle()));
				indexOfDroppedOnItem = i;
				insertFiles = true;
				droppedOnTop = true;
				md_log("File dropped on index: " + std::to_string(i));
				return;
			}
			if (m_AudioObjectContainer[i]->bottomHasFocus)
			{
				//md_log(utf16_to_utf8(m_AudioObjectContainer[i]->GetTitle()));
				indexOfDroppedOnItem = i + 1;
				insertFiles = true;
				droppedOnTop = false;
				md_log("File dropped on index: " + std::to_string(i));
				return;
			}

		}

		indexOfDroppedOnItem = m_AudioObjectContainer.size();
		insertFiles = true;
		droppedOnTop = false;
		md_log("File not dropped on playlist items");
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
	Audio::CalculateDroppedPosInPlaylist();

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
			std::vector<std::wstring> tempFolderPathsVec;
			for (auto & i : fs::directory_iterator(path))
			{
				if (Info::CheckIfAlreadyLoaded(&m_LoadedPathContainer, i.path().wstring()) == true)
				{
					md_log("Audio item at path \"" + i.path().string() + "\" already loaded!\n");
				}
				else
				{
					if (Info::CheckIfAudio(i.path().wstring()))
					{
						//CalculateDroppedPosInPlaylist();
						m_AddedFilesPathContainer.push_back(i.path().wstring());
					}
					else if (fs::is_directory(i.path().wstring()))
						tempFolderPathsVec.push_back(i.path().wstring());
				}
			}
			for(auto & i : tempFolderPathsVec)
				PushToPlaylist(i);

			tempFolderPathsVec.clear();
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

				//CalculateDroppedPosInPlaylist();

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


	if (State::CheckState(State::PathLoadedFromFileVolatile) == true)
		filesLoadedFromFile = true;

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

		State::SetState(State::AudioAdded);

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

		std::vector<std::wstring> tempVec;
		if (insertFiles == true)
			tempVec = std::vector<std::wstring>(indexOfDroppedOnItem);
		else
			tempVec = std::vector<std::wstring>(currentContainersSize);

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
			tt[i].detach();
		}

		// Keep track of size of containers
		previousContainerSize = currentContainersSize;
		currentContainersSize += toProcessCount;

		insertFiles = false;

		if(m_AddedFilesFoldersPathContainer.empty() == false)
			foldersRepSet = false;
		
		if(filesLoadedFromFile == true)
			foldersRepSet = true;


		delete[] tt;
	}

	if (m_AudioObjectContainer.empty() == false)
	{
		// If added files are loaded(only audio objets are created), set appropriate flag
		if (currentContainersSize == currentlyLoadedItemsCount)
		{
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
			State::SetState(State::FilesInfoLoaded);
			Info::SingleItemInfoLoaded = true;
		}
		else
			State::ResetState(State::FilesInfoLoaded);
	}
}

void Audio::PerformDeletion(s32 index)
{
	assert(index >= 0);
	assert(index < m_AudioObjectContainer.size());

	auto sepCont = Interface::Separator::GetContainer();

	// Find separator sub files container that has id of deleted index
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
			delete sepCont->at(i).second;
			sepCont->at(i).second = nullptr;
			sepCont->erase(sepCont->begin() + i);
		}
	}


	if (m_AudioObjectContainer.at(index)->IsFolderRep() == true &&
		index + 1 < m_AudioObjectContainer.size())
	{
		m_AudioObjectContainer.at(index + 1)->SetAsFolderRep();
	}
	/* Decrement all playlist item indexes that are greater than deleted pos
	   (to keep all indexes in in continuous ascending fashion)
	*/
	for (s32 i = index; i < m_AudioObjectContainer.size(); i++)
		m_AudioObjectContainer[i]->DecrementID();

	delete m_AudioObjectContainer.at(index);
	m_AudioObjectContainer.at(index) = nullptr;
	m_AudioObjectContainer.erase(m_AudioObjectContainer.begin() + index);

	// Delete that path from loaded paths container
	m_LoadedPathContainer.erase(m_LoadedPathContainer.begin() + index);


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
		m_FolderContainer.clear();
		indexOfDroppedOnItem = -1;
	}
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
	if (filesLoadedFromFile == false || filesInfoScanned == false)
	{
		i = beg;
		for (; i < end; i++)
		{
			if (mdEngine::IsAppClosing() == false)
			{
				Info::SingleItemInfoLoaded = false;
				Info::GetInfo(&m_AudioObjectContainer[i]->GetID3Struct(), vec[i]);
			}
		}

	}
}

b8 Audio::AddAudioItem(std::wstring path, s32 id)
{
	// Lock the mutes

	std::wstring p = path;

	std::lock_guard<std::mutex> lockGuard(mutex);

	if (Info::CheckIfAudio(path) == false)
		return false;

	State::SetState(State::AudioAdded);


	m_LoadedPathContainer[id] = path;
	// Need to get basic properties before creating audio object
	auto audioObject = new AudioObject();
	if (filesLoadedFromFile == true)
	{
		State::SetState(State::PathLoadedFromFile);
		audioObject->GetID3Struct() = m_ID3Container[id];
		m_ID3Container[id].folder_rep == true ? audioObject->SetAsFolderRep() : (void)0;
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
	if (filesLoadedFromFile == true || firstFilesLoaded == false)
	{
		/*	All files are loaded in a proper order as they should be in separators. Just create new separators for every fodler path
			and add sub files to it
		*/
		
		std::wstring previousFolderPath = L"";
		Interface::PlaylistSeparator* ps = nullptr;
		s32 counter = 0;
		for (auto & i : m_AddedFilesPathContainer)
		{
			std::wstring currentFolderPath = Info::GetFolderPath(i);

			if (currentFolderPath.compare(previousFolderPath) != 0)
			{
				ps = new Interface::PlaylistSeparator(currentFolderPath);
				ps->InitItem(counter);
				m_AudioObjectContainer[counter]->SetAsFolderRep();
			}
			
			previousFolderPath = currentFolderPath;

			ps->SeparatorSubFilePushBack(m_AudioObjectContainer[counter]->GetIDP(), i);

			counter++;
			if(filesLoadedFromFile == true && filesInfoScanned == true)
				Info::LoadedItemsInfoCount++;
		}
		

		filesLoadedFromFile = false;
		m_AddedFilesFoldersPathContainer.clear();
		m_AddedFilesPathContainer.clear();
		foldersRepSet = true;
	}


	if (foldersRepSet == false && firstFilesLoaded == true)
	{
		/* Every file's folder added to the playlist IN ONE EVENT is stored in m_AddedFilesFoldersPathContainer.
		   For every folder's path in that container, find a FIRST audio file from that folder path and then
		   find an audio objects with exactly the same path as that audio file and set it as that folder representant.

		   NOTE: If folder's path of file on which audio file(or folder with audio files) was dropped is the same
				 as folder's path of dropped audio file(or folder with audio files), don't set an another folder repesentant
				 (because file on which audio files were dropped is that folder's rep or another file from that folder is 
				 already a folder representant).

			(1) -> If playlist is not empty and first file(s) folder is set(firstFileFolderRepChecked, because most 
					of the cases some weird things are happening only to the first file(s) of drag&drop event. Rest of the
					folders from that event are pretty straight forward) then find the folder path of file on which new
					files were dropped and check if first file of drag&drop event has the same folder as file on which new files
					were dropped. If yes, it means that all files from drag&drop event associated with first file's folder
					from drag&drop event are from the same folder as file on which files from drag&drop event were dropped.
					Then set if statement logic, that add folder's rep to false in that case(don't create a new folder rep)

			(2) -> If playlist is not empty and file right beneath file on which files from drop&event were dropped
					has the same folder'path as first file from drag&drop event, then take away folder's rep from that file
					give it to the first file from that drag&drop event and reassign playlist separator of file, from which
					folder rep was taken to a new file.

		*/

		// New files are loaded to the playlist
		State::ResetState(State::PathLoadedFromFile);

		s32 start = 0;
		for (s32 i = 0; i < m_AddedFilesFoldersPathContainer.size(); i++)
		{
			std::wstring folderPath(m_AddedFilesFoldersPathContainer[i]);

			Interface::PlaylistSeparator *ps = nullptr;
			b8 folderRepSet(false);
			b8 folderRepTakenAway(false);
			b8 filesFromTheSameFolder(false);
			s32 insertIndex = 0;

			for (s32 k = start; k < m_AddedFilesPathContainer.size(); k++)
			{
				s32 indexTemp = indexOfDroppedOnItem - 1;
				if (indexTemp < 0)
					indexTemp = 0;

				std::wstring filePath = m_AddedFilesPathContainer[k];
				assert(indexOfDroppedOnItem + k < m_AudioObjectContainer.size());
				s32 *fileIndex = m_AudioObjectContainer[indexOfDroppedOnItem + k]->GetIDP();
				b8 statementLogic(false);

			
				std::wstring droppedOnFolderPath = L"";
				// Find proper folder path on which files were dropped. It's different when dropped on first file on the playlist.
				if(droppedOnTop == true && indexOfDroppedOnItem > 0)
					droppedOnFolderPath = m_AudioObjectContainer[indexTemp + filesAddedCount + 1]->GetFolderPath();
				else if(droppedOnTop == true)
					droppedOnFolderPath = m_AudioObjectContainer[indexTemp + filesAddedCount]->GetFolderPath();
				else
					droppedOnFolderPath = m_AudioObjectContainer[indexTemp]->GetFolderPath();
				
				// (1)
				if (indexOfDroppedOnItem >= 0 && folderRepSet == false)
				{
					statementLogic = Info::GetFolderPath(filePath).compare(droppedOnFolderPath) != 0;
					filesFromTheSameFolder = Info::GetFolderPath(filePath).compare(droppedOnFolderPath) == 0;
					// (2)

					if (filesFromTheSameFolder == true && m_AddedFilesFoldersPathContainer.size() <= 1)
					{
						if (m_AudioObjectContainer.size() > indexTemp + filesAddedCount)
						{
							std::wstring pathOfFileBeneath = m_AudioObjectContainer[indexTemp + filesAddedCount]->GetFolderPath();
							if (m_AudioObjectContainer[indexTemp + filesAddedCount]->IsFolderRep() == true &&
								folderPath.compare(pathOfFileBeneath) == 0)
							{
								m_AudioObjectContainer[indexTemp + filesAddedCount]->TakeFolderRep();
								folderRepTakenAway = true;
							}
						}
					}
			
					// (4)
					
					firstFileFolderRepChecked = true;
					if (filesFromTheSameFolder == true || folderRepTakenAway == true)
					{
						std::wstring pathOfFile = L"";
						if(droppedOnTop == true)
							indexTemp += filesAddedCount;
						
						pathOfFile = m_AudioObjectContainer[indexTemp]->GetPath();

						for (auto & s : *Interface::Separator::GetContainer())
						{
							auto sepSubCon = s.second->GetSubFilesContainer();
							auto it = std::find_if(sepSubCon->begin(), sepSubCon->end(), 
								[&](std::pair<s32*, std::wstring> & ref)
							{ return ref.second.compare(pathOfFile) == 0; });
							if (it != sepSubCon->end())
							{
								ps = s.second;
								break;
							}
						}
					}

				}
				else if (folderRepSet == false)
				{
					statementLogic = folderPath.compare(Info::GetFolderPath(filePath)) == 0;
				}

				if (statementLogic == true ||
					folderRepTakenAway == true &&
					folderRepSet == false)
				{
					s32 index = start;
					if (indexOfDroppedOnItem >= 0)
						index = indexOfDroppedOnItem + start;
					assert(index < m_AudioObjectContainer.size());
					auto audioObj = m_AudioObjectContainer[index];

					audioObj->SetAsFolderRep();

					if (folderRepTakenAway == false)
					{
						ps = new Interface::PlaylistSeparator(folderPath);
						ps->InitItem(k);
					}
					folderRepSet = true;
				}
				
				assert(ps != nullptr);

				// insert file to separator sub files container in the same order as it is in audio object's container
				if (ps != nullptr && folderPath.compare(Info::GetFolderPath(filePath)) == 0)
				{
					auto subCon = ps->GetSubFilesContainer();

					if (filesFromTheSameFolder == true && 
						firstFilesLoaded == true && 
						subCon->empty() == false && 
						indexOfDroppedOnItem + filesAddedCount < m_AudioObjectContainer.size())
					{
						std::wstring filePathBeneathDroppedOn = m_AudioObjectContainer[indexOfDroppedOnItem + filesAddedCount]->GetPath();

						for (s32 t = 0; t < subCon->size(); t++)
						{
							if (subCon->at(t).second.compare(filePathBeneathDroppedOn) == 0)
							{
								
								ps->SeparatorSubFileInsert(fileIndex, filePath, t);
								insertIndex++;
								break;
							}
						}
					}
					else
					{
						s32 index = firstFilesLoaded == false ? k : indexTemp + k + 1;
						ps->SeparatorSubFilePushBack(fileIndex, filePath);
					}
				}
				else
				{
					// Next folder's files will start from k position(last file in previous folder)
					start = k;
					break;
				}
			}
		}


		// Add label to the first item of the second part
		if (indexOfDroppedOnItem >= 0 && 
			indexOfDroppedOnItem < previousContainerSize)
		{
			s32 index = indexOfDroppedOnItem + filesAddedCount;

			std::wstring folderPathTemp = m_AudioObjectContainer[index]->GetFolderPath();
			std::wstring nextFolderPath = m_AudioObjectContainer[index]->GetFolderPath();
			std::wstring nextFilePath = m_AudioObjectContainer[index]->GetPath();

			if (m_AudioObjectContainer[indexOfDroppedOnItem]->GetFolderPath().compare(nextFolderPath) != 0)
			{
				// Set first file of second half as a folder rep
				m_AudioObjectContainer[index]->SetAsFolderRep();

				auto sepCon = Interface::Separator::GetContainer();
				std::vector<std::wstring>::iterator it;
				[&]
				{
					// find a separator sub files container that has file which is a first file of second half
					for (auto i : *sepCon)
					{
						// Find a first file of second half
						auto sepSubCon = i.second->GetSubFilesContainer();
						for (s32 k = 0; k < sepSubCon->size(); k++)
						{
							if (sepSubCon->at(k).second.compare(nextFilePath) == 0)
							{
								// Delete files from first half that are part of second half
								s32 size = sepSubCon->size();
								for (s32 p = k; p < size; p++)
								{
									sepSubCon->pop_back();
									i.second->SeparatorSubFileErased();
								}
								return;
							}
						}
					}
				}();

				// Create a new playlist separator for second half and rewrite files that belong to second half
				auto ps = new Interface::PlaylistSeparator(folderPathTemp);
				ps->InitItem(index);
				while (folderPathTemp.compare(m_AudioObjectContainer[index]->GetFolderPath()) == 0 && 
					   index < m_AudioObjectContainer.size())
				{
					assert(index < m_AudioObjectContainer.size());
					nextFolderPath = m_AudioObjectContainer[index]->GetFolderPath();
					nextFilePath = m_AudioObjectContainer[index]->GetPath();
					ps->GetSubFilesContainer()->resize(index + 1);

					ps->SeparatorSubFileInsert(m_AudioObjectContainer[index]->GetIDP(), nextFilePath, index);
					index++;
					if (index >= m_AudioObjectContainer.size())
						break;
				}
			}
		}

		/*	Separator sub files container size is determined by the highest position of the file from that 
			sub files continaer in audio objects container. It will create files in proper order but leave 
			also empty fields. Delete these fields.
		*/
		auto sepCon = Interface::Separator::GetContainer();
		for (auto i : *sepCon)
		{
			for (s32 k = i.second->GetSubFilesContainer()->size() - 1; k >= 0; k--)
			{
				auto con = i.second->GetSubFilesContainer();
				if (con->at(k).second.empty() == true)
					con->erase(con->begin() + k);
			}
		}

		// Sort all playlist separators basing on index of the first file in sub files container
		Interface::Separator::SortSeparatorContainer();

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

			
			State::SetState(State::ContainersResized);
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

s32 Audio::GetFilesAddedCount()
{
	return filesAddedCount;
}

s32 Audio::GetDroppedOnIndex()
{
	return indexOfDroppedOnItem;
}


// Containers functions
Audio::FolderContainer& Audio::Folders::GetAudioFoldersContainer()
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

Audio::AudioObjectContainer& Audio::Object::GetAudioObjectContainer()
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
