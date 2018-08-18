#ifndef MP_AUDIO_OBJECT_H
#define MP_AUDIO_OBJECT_H

#include "mp_channel_attrib.h"
#include "../interface/md_interface.h"
#include "../utility/md_types.h" 

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
		~AudioObject();

		void Init();

		s32& GetID() const;
		void DecrementID();
		std::wstring GetPath() const;
		std::wstring GetFolderPath() const;
		std::wstring GetArtist() const;
		std::wstring GetTitle();
		std::wstring GetTrackNum() const;
		std::wstring GetAlbum() const;
		std::wstring GetYear() const;
		std::wstring GetComment() const;
		std::wstring GetGenre() const;
		std::wstring GetFormat() const;
		f32 GetFrequency() const;
		f32 GetBitrate() const;
		s32 GetObjectSize() const;
		f64 GetLength() const;
		AudioProperties* GetAudioProperty() const;
		Interface::PlaylistItem* GetPlaylistItem() const;

	private:
		AudioProperties* m_AudioProperties;
		Interface::PlaylistItem* m_PlaylistItem;


	};

}

#endif // !MP_AUDIO_OBJECT_H
