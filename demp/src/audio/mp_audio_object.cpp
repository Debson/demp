#include "mp_audio_object.h"


namespace Audio
{

}

Audio::AudioObject::AudioObject() { }

Audio::AudioObject::~AudioObject()
{
	folder = "";
	delete path;
	delete info;
	info = NULL;
	path = NULL;
}

void Audio::AudioObject::Init()
{
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
	if (info->title.compare("") == 0)
		return Info::GetCompleteTitle(*path);
		
	return info->title;
}


const std::string Audio::AudioObject::GetCompleteTitle() const
{
	if (info->title.compare("") == 0)
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

