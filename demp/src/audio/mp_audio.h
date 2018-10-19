#ifndef MP_AUDIO_H
#define MP_AUDIO_H

#include <vector>

#include "mp_channel_attrib.h"
#include "mp_audio_object.h"
#include "../interface/md_helper_windows.h"

namespace Audio
{
	// Stores pointers to created audio objects
	typedef std::vector<std::shared_ptr<AudioObject>> AudioObjectContainer;
	// Stores all unique valid paths of supported file formats just from one event(drag and drop, file expl. folder expl.)
	typedef std::vector<std::string*> AddedFilesPathContainer;

	extern u32 AudioContainerSizeBeforeDeletion;

	extern u32 DroppedItemsCount;

	//extern u32 timeCount;

	namespace Object
	{
		AudioObjectContainer* GetAudioObjectContainer();
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

	b8 LoadPathsFromFile(std::string& path, Info::ID3* id3);

	b8 PushToPlaylist(std::string path, b8 firstCall = true);

	void SaveDroppedPath(std::string path);

	void OnDropComplete();

	void PushToPlaylistTest(std::string path, b8 firstCall = true);



	void DeallocateAudioItems();

	void UpdateAudioLogic();
	
	/* @param: index of deleted file 
	   @param: is amount of deleted files small? */
	void PerformDeletion(s32 index, b8 smallDeletion);

	void PerformSetFoldersRep();

	void LoadFilesInfo();

	// Returns amount of added items to he playlist from event + index in playlist on which files were dropped
	s32 GetFilesAddedCount();

	s32 GetDroppedOnIndex();

	u32 GetProccessedFileCount();

	u32 GetProcessedID3Tags();

	u32 GetIndexOfLoadingObject();

#ifdef _DEBUG_
	void GetItemsInfo();
#endif
}

#endif // !MP_AUDIO_H

