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
		AudioItem* GetItem(s32 id);
		u32 GetSize();
	}

	b8 PushToPlaylist(const std::wstring path);

	b8 DeallocateAudioItems();

	// temporary
	void GetItemsInfo();
}

#endif // !MP_AUDIO_H

