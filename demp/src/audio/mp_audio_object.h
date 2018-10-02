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
		virtual ~AudioProperties() { };
		std::string* path;
		std::string folder;

		Info::ID3* info;
	};

	class AudioObject : private AudioProperties, public Interface::PlaylistItem
	{
	public:
		AudioObject();
		~AudioObject();

		void Init();

		s32& GetID();
		s32* GetIDP();
		void DecrementID();
		void IncrementID();
		std::string& GetPath()				const;
		const std::string GetFolderPath()	const;
		const std::string& GetArtist()		const;
		const std::string GetTitle();
		const s32& GetTrackNum()			const;
		const std::string& GetAlbum()		const;
		const s32& GetYear()				const;
		const std::string& GetComment()		const;
		const std::string& GetGenre()		const;
		const std::string& GetFormat()		const;
		void SetID(s32 id);
		void SetPath(std::string& path);
		void SetFolderPath(std::string& path);
		f32 GetFrequency()					const;
		f32 GetBitrate()					const;
		s32 GetObjectSize()					const;
		f64 GetLength()						const;
		Info::ID3* GetID3Struct();
		void SetID3Struct(Info::ID3* id3);

	private:


	};

}

#endif // !MP_AUDIO_OBJECT_H
