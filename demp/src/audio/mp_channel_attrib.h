#ifndef MP_CHANNEL_ATTRIB_H
#define MP_CHANNEL_ATTRIB_H

#include <iostream>
#include <bass.h>

#include "../utility/md_types.h"
#include "../settings/music_player_settings.h"

using namespace mdEngine;

namespace Audio
{
	struct AudioObject;

	namespace Info
	{
		enum FileType
		{
			MP3,
			MP4,
			WAV,
			WMA,
			OGG
		};


		struct ID3
		{
			ID3() 
			{ 
				loaded		= false;
				title		= "";
				artist		= "";
				composer	= "";
				encoded_by	= "";
				track_num	= 0;
				album		= "";
				year		= 0;
				comment		= "";
				genre		= "";
				format		= "";
				bitrate		= 0;
				channels	= 0;
				freq		= 0;
				size		= 0;
				length		= 0;
				infoCorrupted = false;
			}

			std::string title;
			std::string artist;
			std::string composer;
			std::string encoded_by;
			s32 track_num;
			std::string album;
			s32 year;
			std::string comment;
			std::string genre;
			std::string format;

			f32 bitrate;
			s32 channels;
			f32 freq;
			f32 size;	// in bytes
			f32 length;	// in sec
				
			b8 infoCorrupted;
			DWORD ctype;
			b8 loaded;
		};

		extern s32 LoadedItemsInfoCount;
		extern b8 ItemBeingProcessed;

		void Update();

		std::vector<std::string>* GetLoadedPathsContainer();

		b8 CheckIfAudio(std::string& path);

		b8 CheckIfImage(const std::string& path);

		b8 CheckIfHasItems(std::wstring& path);
		
		b8 IsPathLoaded(std::string& path);


		// Returns a full path of a file folder(path without a filename)
		std::string GetFolderPath(std::string& path);

		// Returns only a name of a file's folder
		std::string GetFolder(std::string& path);

		std::string GetCompleteTitle(std::string& path);

		std::string GetArtist(std::string& path);

		std::string GetExt(std::string& path);

		void GetInfo(std::shared_ptr<AudioObject> audioObj);

		//void GetID3Info(Info::ID3* info, std::string& path);

		std::string GetProcessedItemsCountStr();

		s32 GetProcessedItemsCount();
	}
}


#endif // !MP_CHANNEL_ATTRIB_H

