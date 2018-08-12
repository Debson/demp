#ifndef MP_AUDIO_H
#define MP_AUDIO_H

#include <vector>

#include "mp_channel_attrib.h"


namespace Audio
{
	struct AudioItem
	{
		s32 id;
		std::wstring path;
		std::wstring folder;
		std::wstring name;

		Info::ChannelInfo info;
	};

	namespace Items
	{
		std::vector<AudioItem*>& GetContainer();
		AudioItem* GetItem(s32 id);
		u32 GetSize();
	}

	namespace Folders
	{
		std::vector<std::wstring>& GetContainer();
		std::wstring GetItem(s32 id);
		u32 GetSize();
		b8 AddFolder(std::wstring name);
		void PrintContent();
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

