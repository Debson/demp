#ifndef MP_CHANNEL_ATTRIB_H
#define MP_CHANNEL_ATTRIB_H

#include <iostream>
#include <bass.h>

#include "../utility/md_types.h"

using namespace mdEngine;

namespace Audio
{
	namespace Info
	{
		struct ID3
		{
			ID3() 
			{ 
				loaded		= false;
				id			= L"";
				title		= L"";
				artist		= L"";
				composer	= L"";
				encoded_by	= L"";
				track_num	= L"";
				album		= L"";
				year		= L"";
				comment		= L"";
				genre		= L"";
				format		= L"";
				bitrate		= 0;
				channels	= 0;
				freq		= 0;
				size		= 0;
				length		= 0;
			}

			std::wstring id;
			std::wstring title;
			std::wstring artist;
			std::wstring composer;
			std::wstring encoded_by;
			std::wstring track_num;
			std::wstring album;
			std::wstring year;
			std::wstring comment;
			std::wstring genre;
			std::wstring format;

			f32 bitrate;
			f32 channels;
			f32 freq;
			f32 size;	// in bytes
			f64 length;	// in sec

			b8 loaded;
		};

		b8 CheckIfAudio(std::wstring path);

		b8 CheckIfHasItems(std::wstring path);


		
		b8 CheckIfAlreadyLoaded(std::vector<std::wstring>* v, std::wstring path);

		std::wstring GetFolder(std::wstring path);

		std::wstring GetCompleteTitle(std::wstring path);

		std::wstring GetArtist(std::wstring path);

		std::wstring GetExt(std::wstring path);

		void GetInfo(Info::ID3* info, std::wstring path);

		void GetID3Info(Info::ID3* info, std::wstring path);


	}
}


#endif // !MP_CHANNEL_ATTRIB_H

