#ifndef MP_AUDIO_H
#define MP_AUDIO_H

#include <vector>

#include "mp_channel_attrib.h"

#include "mp_audio_object.h"

namespace Audio
{
	namespace Folders
	{
		std::vector<std::wstring*>& GetAudioFoldersContainer();
		std::wstring* GetAudioFolder(s32 id);
		u32 GetSize();
		b8 AddFolder(std::wstring name);
		void PrintContent();
	}

	namespace Object
	{
		std::vector<AudioObject*>& GetAudioObjectContainer();
		AudioObject* GetAudioObject(s32 id);
		u32 GetSize();

#ifdef _DEBUG_
		//void PrintContent();
#endif
	}

	b8 PushToPlaylist(std::wstring path);

	b8 DeallocateAudioItems();

	void UpdateAudioLogic();

	void OnUpdateAudio();
	
	void PerformDeletion(s32 index);

	// temporary

	void GetItemsInfo();

	u32 GetProccessedFileCount();
}

#endif // !MP_AUDIO_H

