#ifndef MP_AUDIO_H
#define MP_AUDIO_H

#include <vector>

#include "mp_channel_attrib.h"

#include "mp_audio_object.h"

namespace Audio
{
	

	namespace Folders
	{
		std::vector<std::wstring>& GetContainer();
		std::wstring GetItem(s32 id);
		u32 GetSize();
		b8 AddFolder(std::wstring name);
		void PrintContent();
	}

	namespace Object
	{
		std::vector<AudioObject*>& GetContainer();
		AudioObject* GetItem(u32 id);
		u32 GetSize();

#ifdef _DEBUG_
		//void PrintContent();
#endif
	}

	b8 PushToPlaylist(std::wstring path);

	b8 DeallocateAudioItems();

	void UpdateAudioLogic();

	extern b8 PathsLoaded;

	// temporary
	void Test(std::wstring pa);

	void GetItemsInfo();
}

#endif // !MP_AUDIO_H

