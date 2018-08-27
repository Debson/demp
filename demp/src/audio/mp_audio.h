#ifndef MP_AUDIO_H
#define MP_AUDIO_H

#include <vector>

#include "mp_channel_attrib.h"
#include "mp_audio_object.h"

namespace Audio
{
	namespace Folders
	{
		std::vector<std::wstring>& GetAudioFoldersContainer();
		std::wstring GetAudioFolder(s32 id);
		u32 GetSize();
		b8 AddFolder(std::wstring name);
#ifdef _DEBUG_
		void PrintContent();
#endif
	}

	namespace Object
	{
		std::vector<Audio::AudioObject*>& GetAudioObjectContainer();
		AudioObject* GetAudioObject(s32 id);
		u32 GetSize();
		u32 GetProcessedSize();

#ifdef _DEBUG_
		//void PrintContent();
#endif
	}


	b8 SavePathFiles(std::wstring path);

	b8 SavePathFiles(std::wstring path, const Info::ID3 id3);

	b8 PushToPlaylist(std::wstring path);

	void DeallocateAudioItems();

	void UpdateAudioLogic();
	
	void PerformDeletion(s32 index);

#ifdef _DEBUG_

	void GetItemsInfo();

	// Returns amount of added items to he playlist from event + index in playlist on which files were dropped
	s32 GetFilesAddedCount();

	u32 GetProccessedFileCount();

	u32 GetProcessedID3Tags();

	void PrintTest();
#endif
}

#endif // !MP_AUDIO_H

