#ifndef MP_CHANNEL_ATTRIB_H
#define MP_CHANNEL_ATTRIB_H

#include <iostream>
#include <bass.h>

#include "../md_types.h"

using namespace mdEngine;

namespace Audio
{
	namespace Info
	{
		struct ChannelInfo
		{
			HSTREAM stream;

			std::wstring ext;
			f32 freq;
			f32 bitrate;
			s32 size;	// in bytes
			f64 length;	// in sec
		};


		b8 CheckIfAudio(std::wstring path);

		b8 CheckIfHasItems(std::wstring path);
		
		b8 CheckIfAlreadyLoaded(std::vector<std::wstring>* v, std::wstring path);

		std::wstring GetFolder(std::wstring path);

		std::wstring GetName(std::wstring path);

		std::wstring GetExt(std::wstring path);

		void GetInfo(Info::ChannelInfo* info, std::wstring path);


	}
}


#endif // !MP_CHANNEL_ATTRIB_H

