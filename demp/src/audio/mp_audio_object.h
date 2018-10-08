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
		std::string& GetPath()					const;
#ifdef _WIN32_
		std::wstring GetPathUTF8()				const;
#else

#endif
		const std::string GetFolderPath()		const;
		const std::string& GetArtist()			const;
		const std::string GetTitle();
		const std::string GetNameToPlaylist()	const;
		const std::string GetCompleteName()		const;
		const s32& GetTrackNum()				const;
		const std::string& GetAlbum()			const;
		const s32& GetYear()					const;
		const std::string& GetComment()			const;
		const std::string& GetGenre()			const;
		const std::string& GetFormat()			const;
		void SetID(s32 id);
		void SetPath(std::string& path);
		void SetFolderPath(std::string& path);
		void ReloadTextTexture();
		f32 GetFrequency()						const;
		f32 GetBitrate()						const;
		s32 GetObjectSize()						const;
		f64 GetLength()							const;
		Info::ID3* GetID3Struct();
		// Function that loads the id3v2 image on different thread(image too large)
		void LoadAlbumImageThreadFun();
		// Finds ID3v2 image and loads it to the texture
		void LoadAlbumImage();
		// Function that is called right after thread finished searching for a large-sized IDv2 image
		void LoadAlbumImageLargeSize();
		void DeleteAlbumImageTexture();
		GLuint GetAlbumPictureTexture();
		void SetID3Struct(Info::ID3* id3);

	private:

		b8 LoadAlbumImageFromFolder();


		b8 m_LoadAlbumTexture;
		void* m_AlbumImageData;
		u32 m_AlbumImageDataSize;
		GLuint m_AlbumImageTex;


	};


	typedef std::vector<s32*> ProcessAlbumImageQueue;

	ProcessAlbumImageQueue* GetProcessAlbumImageQueue();

}

#endif // !MP_AUDIO_OBJECT_H
