#include "mp_audio_object.h"


namespace Audio
{

}

Audio::AudioObject::AudioObject() { }

Audio::AudioObject::~AudioObject()
{
	path = L"";
	folder = L"";;
}

void Audio::AudioObject::Init()
{
	InitItem(&id);
}

s32& Audio::AudioObject::GetID()
{
	return id;
}

void Audio::AudioObject::DecrementID()
{
	id > 0 ? id-- : 0;
}

void Audio::AudioObject::IncrementID()
{
	id++;
}

std::wstring Audio::AudioObject::GetPath() const
{
	return path;
}

std::wstring Audio::AudioObject::GetFolderPath() const
{
	return folder;
}

std::wstring Audio::AudioObject::GetArtist() const
{
	return info.artist;
}

std::wstring Audio::AudioObject::GetTitle() 
{
	if (info.title.compare(L"") == 0)
		return Info::GetCompleteTitle(path);
		
	return info.title;
}

std::wstring Audio::AudioObject::GetTrackNum() const
{
	if (info.track_num.compare(L"0") == 0)
		return L"";

	return info.track_num;
}

std::wstring Audio::AudioObject::GetAlbum() const
{
	return info.album;
}

std::wstring Audio::AudioObject::GetYear() const
{
	if (info.year.compare(L"0") == 0)
		return L"";

	return info.year;
}

std::wstring Audio::AudioObject::GetComment() const
{
	if (info.comment.compare(L"0") == 0)
		return L"";

	return info.comment;
}

std::wstring Audio::AudioObject::GetGenre() const
{
	return info.genre;
}

std::wstring Audio::AudioObject::GetFormat() const
{
	return info.format;
}

void Audio::AudioObject::SetID(s32 id)
{
	this->id = id;
}

void Audio::AudioObject::SetPath(std::wstring path)
{
	this->path = path;
}

void Audio::AudioObject::SetFolderPath(std::wstring path)
{
	this->folder = path;
}

f32 Audio::AudioObject::GetFrequency() const
{
	return info.freq;
}

f32 Audio::AudioObject::GetBitrate() const
{
	return info.bitrate;
}

s32 Audio::AudioObject::GetObjectSize() const
{
	return info.size;
}

f64 Audio::AudioObject::GetLength() const
{
	return info.length;
}

Audio::Info::ID3& Audio::AudioObject::GetID3Struct()
{
	return info;
}

