#ifndef MP_AUDIO_H
#define MP_AUDIO_H

#include <vector>

#include "mp_channel_attrib.h"
#include "mp_audio_object.h"

namespace Audio
{
	// Stores pointers to created audio objects
	typedef std::vector<std::shared_ptr<AudioObject>> AudioObjectContainer;
	// Stores only paths to folders
	typedef std::vector<std::wstring> FolderContainer; // Stores only paths to folders
	// Stores all paths of supported files	that are currently loaded into playlist
	typedef std::vector<std::wstring> LoadedPathContainer;
	// Stores all unique valid paths of supported file formats just from one event(drag and drop, file expl. folder expl.)
	typedef std::vector<std::wstring> AddedFilesPathContainer;
	// Stores all unqiue folders paths of supported file formats just from one event(drag and drop, file expl. folder expl.)
	typedef std::vector<std::wstring> AddedFilesFolderContainer;
	typedef std::vector<std::pair<std::wstring, Interface::PlaylistSeparator*>> PlaylistItemFolderContainer;

	namespace Folders
	{
		FolderContainer& GetAudioFoldersContainer();
		std::wstring GetAudioFolder(s32 id);
		u32 GetSize();
		b8 AddFolder(std::wstring name);
#ifdef _DEBUG_
		void PrintContent();
#endif
	}

	namespace Object
	{
		AudioObjectContainer& GetAudioObjectContainer();
		std::shared_ptr<AudioObject> GetAudioObject(s32 id);
		u32 GetSize();
		u32 GetProcessedSize();

#ifdef _DEBUG_
		//void PrintContent();
#endif
	}

	void InitializeConfig();

	void StartAudio();

	void FilesAddedByFileBrowser(b8 val);

	b8 LoadPathsFromFile(std::wstring path, const Info::ID3 id3);

	b8 PushToPlaylist(std::wstring path);

	void DeallocateAudioItems();

	void UpdateAudioLogic();
	
	void PerformDeletion(s32 index);


	// Returns amount of added items to he playlist from event + index in playlist on which files were dropped
	s32 GetFilesAddedCount();

	s32 GetDroppedOnIndex();

	u32 GetProccessedFileCount();

	u32 GetProcessedID3Tags();

#ifdef _DEBUG_

	void GetItemsInfo();


	void PrintTest();
#endif
}

#endif // !MP_AUDIO_H

