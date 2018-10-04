#include "mp_audio_object.h"

#include <taglib/toolkit/tbytevector.h>
#include <taglib/mpeg/mpegfile.h>
#include <taglib/mpeg/id3v2/id3v2tag.h>
#include <taglib/mpeg/id3v2/id3v2frame.h>
#include <taglib/mpeg/id3v2/frames/attachedpictureframe.h>

#include "../utility/md_load_texture.h"
#include "../utility/utf8_to_utf16.h"
#include "../utility/md_util.h"

namespace Audio
{

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

void Audio::AudioObject::LoadAlbumImage()
{
	static const char *IdPicture = "APIC";
	TagLib::MPEG::File mpegFile(GetPathUTF8().c_str());
	TagLib::ID3v2::Tag *id3v2tag = mpegFile.ID3v2Tag();
	TagLib::ID3v2::FrameList Frame;
	TagLib::ID3v2::AttachedPictureFrame *PicFrame;

	u32 size;

	void* data = NULL;
	if (id3v2tag)
	{
		Frame = id3v2tag->frameListMap()[IdPicture];
		if (!Frame.isEmpty())
		{
			for (TagLib::ID3v2::FrameList::ConstIterator it = Frame.begin(); it != Frame.end(); ++it)
			{
				PicFrame = (TagLib::ID3v2::AttachedPictureFrame *)(*it);

				size = PicFrame->picture().size();
				data = malloc(size);
				memcpy(data, PicFrame->picture().data(), size);
			}
		}
	}
	else
	{
		MD_ERROR("Could not find an album image!");
	}

	if (data != NULL)
	{
		m_AlbumImageTex = mdLoadTexture(data, size);
		free(data);
		data = NULL;
	}

	// data is freed in mdLoadTexture function

	if (m_AlbumImageTex > 0)
		md_log("Album cover image loaded succesfully");
}

void Audio::AudioObject::DeleteAlbumImageTexture()
{
	if (m_AlbumImageTex > 0)
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

