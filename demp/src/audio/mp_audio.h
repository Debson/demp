#ifndef MP_AUDIO_H
#define MP_AUDIO_H

#include <vector>

#include "mp_channel_attrib.h"
#include "mp_audio_object.h"

namespace Audio
{
	// Stores pointers to created audio objects
	typedef std::vector<std::shared_ptr<AudioObject>> AudioObjectContainer;
	// Stores all unique valid paths of supported file formats just from one event(drag and drop, file expl. folder expl.)
	typedef std::vector<std::wstring*> AddedFilesPathContainer;


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

	b8 LoadPathsFromFile(std::wstring& const path, Info::ID3* const id3);

	b8 PushToPlaylist(std::wstring& const path);

	void DeallocateAudioItems();

	void UpdateAudioLogic();
	
	/* @param: index of deleted file 
	   @param: is amount of deleted files small? */
	void PerformDeletion(s32 index, b8 smallDeletion);

	// Returns amount of added items to he playlist from event + index in playlist on which files were dropped
	s32 GetFilesAddedCount();

	s32 GetDroppedOnIndex();

	u32 GetProccessedFileCount();

	u32 GetProcessedID3Tags();

#ifdef _DEBUG_
	void GetItemsInfo();
#endif
}

#endif // !MP_AUDIO_H

