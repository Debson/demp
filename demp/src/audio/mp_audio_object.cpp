#include "mp_audio_object.h"

#include <thread>

#include <boost/range/iterator_range.hpp>
#include <boost/filesystem.hpp>

#include <taglib/toolkit/tbytevector.h>
#include <taglib/fileref.h>
#include <taglib/mpeg/mpegfile.h>
#include <taglib/mpeg/id3v2/id3v2tag.h>
#include <taglib/mpeg/id3v2/id3v2frame.h>
#include <taglib/mpeg/id3v2/frames/attachedpictureframe.h>


#include "../utility/md_load_texture.h"
#include "../utility/utf8_to_utf16.h"
#include "../utility/md_util.h"

namespace fs = boost::filesystem;

namespace Audio
{

	static ProcessAlbumImageQueue m_ProcessAlbumImageQueue;

	struct AlbumTexInfo
	{
		unsigned char* data = NULL;
		s32 width;
		s32 height;
		s32 nrComponents;
	} m_AlbumTexInfo;
}

Audio::AudioObject::AudioObject() 
{ 
	m_AlbumImageTex = 0;
}

Audio::AudioObject::~AudioObject()
{
	glDeleteTextures(1, &m_AlbumImageTex);
	m_AlbumImageTex = 0;
	folder = "";
	delete path;
	delete info;
	info = NULL;
	path = NULL;
}

void Audio::AudioObject::Init()
{
	m_AlbumImageTex = 0;
	InitItem();
}

s32& Audio::AudioObject::GetID()
{
	return m_ItemID;
}

s32* Audio::AudioObject::GetIDP()
{
	return &m_ItemID;
}

void Audio::AudioObject::DecrementID()
{
	m_ItemID > 0 ? m_ItemID-- : 0;
}

void Audio::AudioObject::IncrementID()
{
	m_ItemID++;
}

std::string& Audio::AudioObject::GetPath() const
{
	return *path;
}

#ifdef _WIN32_
std::wstring Audio::AudioObject::GetPathUTF8()	const
{
	return utf8_to_utf16(*path);
}
#else

#endif

const std::string Audio::AudioObject::GetFolderPath() const
{
	return Info::GetFolderPath(*path);
}

const std::string& Audio::AudioObject::GetArtist() const
{
	return info->artist;
}

const std::string Audio::AudioObject::GetTitle()
{
	return info->title;
}

const std::string Audio::AudioObject::GetNameToPlaylist() const
{
	if (info->title.compare("") == 0 || info->artist.compare("") == 0)
		return std::to_string(m_ItemID + 1) + ". " + Info::GetCompleteTitle(*path);

	return std::to_string(m_ItemID + 1) + ". " + info->artist + " - " + info->title;
}

const std::string Audio::AudioObject::GetCompleteName() const
{
	if (info->title.compare("") == 0 || info->artist.compare("") == 0)
		return Info::GetCompleteTitle(*path);

	return info->artist + " - " + info->title;
}

const s32& Audio::AudioObject::GetTrackNum() const
{
	/*if (info->track_num.compare(L"0") == 0)
		return L"";*/

	return info->track_num;
}

const std::string& Audio::AudioObject::GetAlbum() const
{
	return info->album;
}

const s32& Audio::AudioObject::GetYear() const
{
	/*if (info->year.compare(L"0") == 0)
		return L"";*/

	return info->year;
}

const std::string& Audio::AudioObject::GetComment() const
{
	/*if (info->comment.compare(L"0") == 0)
		return L"";*/

	return info->comment;
}

const std::string& Audio::AudioObject::GetGenre() const
{
	return info->genre;
}

const std::string& Audio::AudioObject::GetFormat() const
{
	return info->format;
}

void Audio::AudioObject::SetID(s32 id)
{
	this->m_ItemID = id;
}

void Audio::AudioObject::SetPath(std::string& path)
{
	this->path = &path;
}

void Audio::AudioObject::SetFolderPath(std::string& path)
{
	this->folder = path;
}

void Audio::AudioObject::ReloadTextTexture()
{
	DeleteTexture();

	m_TextString = GetNameToPlaylist();
	TTF_SizeUTF8(m_Font, m_TextString.c_str(), &m_TextSize.x, &m_TextSize.y);
	m_TextTexture = LoadText(m_Font, m_TextString, m_TextColorSDL);
}


f32 Audio::AudioObject::GetFrequency() const
{
	return info->freq;
}

f32 Audio::AudioObject::GetBitrate() const
{
	return info->bitrate;
}

s32 Audio::AudioObject::GetObjectSize() const
{
	return info->size;
}

f64 Audio::AudioObject::GetLength() const
{
	return info->length;
}

Audio::Info::ID3* Audio::AudioObject::GetID3Struct()
{
	return info;
}

void Audio::AudioObject::SetID3Struct(Info::ID3* id3)
{
	info = id3;
}

void Audio::AudioObject::LoadAlbumImageThreadFun()
{
	// Load image data to AlbumTexInfo struct and then push this item id to queue
	m_AlbumTexInfo.data = stbi_load_from_memory((unsigned char*)m_AlbumImageData,
												m_AlbumImageDataSize,
												&m_AlbumTexInfo.width,
												&m_AlbumTexInfo.height,
												&m_AlbumTexInfo.nrComponents,
												4);

	if (m_AlbumTexInfo.data)
	{
		m_ProcessAlbumImageQueue.push_back(&m_ItemID);
	}
}

void Audio::AudioObject::LoadAlbumImage()
{
	if (this->m_AlbumImageTex != 0)
	{
		md_log("Album cover already loaded");
		return;
	}
	// Currently can load album images only of mpeg mp3 files
	
	if (this->info->ctype != BASS_CTYPE_STREAM_MP3)
	{
		if (LoadAlbumImageFromFolder() == true)
		{
			if (m_AlbumImageTex > 0)
				md_log("Album cover image loaded from folder succesfully");
		}
		else
			MD_ERROR("Could not find an album image in the folder!");

		return;
	}

	static const char *IdPicture = "APIC";
	TagLib::MPEG::File mpegFile(GetPathUTF8().c_str(), false);

	TagLib::ID3v2::Tag *id3v2tag = mpegFile.ID3v2Tag();
	TagLib::ID3v2::FrameList Frame;
	TagLib::ID3v2::AttachedPictureFrame *PicFrame;

	m_AlbumImageData = NULL;
	if (id3v2tag != NULL)
	{
		Frame = id3v2tag->frameListMap()[IdPicture];
		if (!Frame.isEmpty())
		{
			for (TagLib::ID3v2::FrameList::ConstIterator it = Frame.begin(); it != Frame.end(); ++it)
			{
				PicFrame = (TagLib::ID3v2::AttachedPictureFrame *)(*it);

				m_AlbumImageDataSize = PicFrame->picture().size();
				m_AlbumImageData = malloc(m_AlbumImageDataSize);
				memcpy(m_AlbumImageData, PicFrame->picture().data(), m_AlbumImageDataSize);
			}
		}
	}
	else
	{
		MD_ERROR("Could not find an album image!");
	}

	if (m_AlbumImageData != NULL)
	{
		// Size of image is too large, process it on different thread
		if (m_AlbumImageDataSize > MAX_ALBUM_IMAGE_SIZE)
		{
			md_log("Large album image size... Opening it on different thread!");
			std::thread tt(&AudioObject::LoadAlbumImageThreadFun, this);
			tt.detach();
			
			return; // Skip other part of the function
		}
		
		m_AlbumImageTex = mdLoadTexture(m_AlbumImageData, m_AlbumImageDataSize);
		free(m_AlbumImageData);
		m_AlbumImageData = NULL;
	}
	else if(LoadAlbumImageFromFolder() == true)
	{
		if (m_AlbumImageTex > 0)
			md_log("Album cover image loaded from folder succesfully");
		return;
	}
	else
	{
		MD_ERROR("Could not find an album image in the folder!");
		return;
	}

	// data is freed in mdLoadTexture function
	if (m_AlbumImageTex > 0)
		md_log("Album cover image loaded succesfully");

}

void Audio::AudioObject::LoadAlbumImageLargeSize()
{
	// Load actual texture to opengl on main thread
	if (m_AlbumTexInfo.data)
	{
		m_AlbumImageTex = mdLoadTexture(m_AlbumTexInfo.data, m_AlbumTexInfo.width, m_AlbumTexInfo.height);
		stbi_image_free(m_AlbumTexInfo.data);
		m_AlbumTexInfo.data = NULL;

		free(m_AlbumImageData);
		m_AlbumImageData = NULL;
	}

	if (m_AlbumImageTex > 0)
		md_log("Large album cover image loaded succesfully");
}

void Audio::AudioObject::DeleteAlbumImageTexture()
{
	// Delete only album image textures with small size(they are fast to load)
	if (m_AlbumImageTex > 0 && 
		m_AlbumImageDataSize < MAX_ALBUM_IMAGE_SIZE)
	{
		glDeleteTextures(1, &m_AlbumImageTex);
		m_AlbumImageTex = 0;

		md_log("Album cover image deleted succesfully");
	}
}

GLuint Audio::AudioObject::GetAlbumPictureTexture()
{
	return m_AlbumImageTex;
}


b8 Audio::AudioObject::LoadAlbumImageFromFolder()
{
	// Iterate through all the items in audio file folder and search for the first image, if found, load it
	for (auto & i : fs::directory_iterator(utf8_to_utf16(this->GetFolderPath())))
	{
		if (fs::is_regular_file(i.path()) == true && 
			Info::CheckIfImage(i.path().string()) == true)
		{
			m_AlbumImageTex = mdLoadTexture(i.path().string());
			return true;
		}
	}

	return false;
}

Audio::ProcessAlbumImageQueue* Audio::GetProcessAlbumImageQueue()
{
	return &m_ProcessAlbumImageQueue;
}