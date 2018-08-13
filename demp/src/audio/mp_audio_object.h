#ifndef MP_AUDIO_OBJECT_H
#define MP_AUDIO_OBJECT_H


#include "mp_channel_attrib.h"
#include "../interface/md_interface.h"
#include "../md_types.h" 

using namespace mdEngine;

namespace Audio
{
	struct AudioProperties
	{
		s32 id;
		std::wstring path;
		std::wstring folder;

		Info::ID3 info;
	};

	class AudioObject
	{
	public:
		AudioObject();
		explicit AudioObject(AudioProperties* ap);


		u32 GetID();
		std::wstring GetPath();
		std::wstring GetFolderPath();
		std::wstring GetArtist();
		std::wstring GetTitle();
		std::wstring GetTrackNum();
		std::wstring GetAlbum();
		std::wstring GetYear();
		std::wstring GetComment();
		std::wstring GetGenre();
		std::wstring GetFormat();
		f32 GetFrequency();
		f32 GetBitrate();
		s32 GetObjectSize();
		f64 GetLength();
		AudioProperties* GetAudioProperty();
		Interface::PlaylistItem* GetPlaylistItem();		

	private:
		AudioProperties* m_AudioProperties;
		Interface::PlaylistItem* m_PlaylistItem;


	};

}

#endif // !MP_AUDIO_OBJECT_H
