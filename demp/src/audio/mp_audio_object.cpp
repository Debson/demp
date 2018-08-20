#include "mp_audio_object.h"


namespace Audio
{

}

Audio::AudioObject::AudioObject() { }

Audio::AudioObject::AudioObject(AudioProperties* ap) : m_AudioProperties(ap) { }

Audio::AudioObject::~AudioObject()
{
	m_AudioProperties->path = L"";
	m_AudioProperties->folder = L"";
	delete m_AudioProperties;
	delete m_PlaylistItem;
}

void Audio::AudioObject::Init()
{

	m_PlaylistItem = new Interface::PlaylistItem();

	m_PlaylistItem->InitFont();

	m_PlaylistItem->InitItem(&m_AudioProperties->id);
}

s32& Audio::AudioObject::GetID() const
{
	assert(m_AudioProperties != NULL);
	return m_AudioProperties->id;
}

void Audio::AudioObject::DecrementID()
{
	assert(m_AudioProperties != NULL);
	if(m_AudioProperties > 0)
		m_AudioProperties->id--;
}

std::wstring Audio::AudioObject::GetPath() const
{
	assert(m_AudioProperties != NULL);
	return m_AudioProperties->path;
}

std::wstring Audio::AudioObject::GetFolderPath() const
{
	assert(m_AudioProperties != NULL);
	return m_AudioProperties->folder;
}

std::wstring Audio::AudioObject::GetArtist() const
{
	assert(m_AudioProperties != NULL);
	return m_AudioProperties->info.artist;
}

std::wstring Audio::AudioObject::GetTitle() 
{
	assert(m_AudioProperties != NULL);
	if (m_AudioProperties->info.title.compare(L"") == 0)
		return Info::GetCompleteTitle(m_AudioProperties->path);
		
	return m_AudioProperties->info.title;
}

std::wstring Audio::AudioObject::GetTrackNum() const
{
	assert(m_AudioProperties != NULL);
	if (m_AudioProperties->info.track_num.compare(L"0") == 0)
		return L"";

	return m_AudioProperties->info.track_num;
}

std::wstring Audio::AudioObject::GetAlbum() const
{
	assert(m_AudioProperties != NULL);
	return m_AudioProperties->info.album;
}

std::wstring Audio::AudioObject::GetYear() const
{
	assert(m_AudioProperties != NULL);
	if (m_AudioProperties->info.year.compare(L"0") == 0)
		return L"";

	return m_AudioProperties->info.year;
}

std::wstring Audio::AudioObject::GetComment() const
{
	assert(m_AudioProperties != NULL);
	if (m_AudioProperties->info.comment.compare(L"0") == 0)
		return L"";

	return m_AudioProperties->info.comment;
}

std::wstring Audio::AudioObject::GetGenre() const
{
	assert(m_AudioProperties != NULL);
	return m_AudioProperties->info.genre;
}

std::wstring Audio::AudioObject::GetFormat() const
{
	assert(m_AudioProperties != NULL);
	return m_AudioProperties->info.format;
}

f32 Audio::AudioObject::GetFrequency() const
{
	assert(m_AudioProperties != NULL);
	return m_AudioProperties->info.freq;
}

f32 Audio::AudioObject::GetBitrate() const
{
	assert(m_AudioProperties != NULL);
	return m_AudioProperties->info.bitrate;
}

s32 Audio::AudioObject::GetObjectSize() const
{
	assert(m_AudioProperties != NULL);
	return m_AudioProperties->info.size;
}

f64 Audio::AudioObject::GetLength() const
{
	assert(m_AudioProperties != NULL);
	return m_AudioProperties->info.length;
}

Audio::AudioProperties* Audio::AudioObject::GetAudioProperty() const
{
	assert(m_AudioProperties != NULL);
	return m_AudioProperties;
}

Interface::PlaylistItem* Audio::AudioObject::GetPlaylistItem() const
{
	if (this == NULL)
		return NULL;

	return m_PlaylistItem;
}
