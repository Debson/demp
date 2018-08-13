#include "mp_audio_object.h"


namespace Audio
{

}
	Audio::AudioObject::AudioObject() { }

	Audio::AudioObject::AudioObject(AudioProperties* ap){
		m_AudioProperties = ap;

		//m_PlaylistItem = new Interface::PlaylistItem();
		//m_PlaylistItem->InitFont();
		//m_PlaylistItem->InitItem();
	}

	u32 Audio::AudioObject::GetID(){

		return m_AudioProperties->id;
	}

	std::wstring Audio::AudioObject::GetPath(){

		return m_AudioProperties->path;
	}

	std::wstring Audio::AudioObject::GetFolderPath() {

		return m_AudioProperties->folder;
	}

	std::wstring Audio::AudioObject::GetArtist() {

		return m_AudioProperties->info.artist;
	}

	std::wstring Audio::AudioObject::GetTitle() {

		if (m_AudioProperties->info.title.compare(L"") == 0)
			return Info::GetCompleteTitle(m_AudioProperties->path);
			
		return m_AudioProperties->info.title;
	}

	std::wstring Audio::AudioObject::GetTrackNum() {

		if (m_AudioProperties->info.track_num.compare(L"0") == 0)
			return L"";

		return m_AudioProperties->info.track_num;
	}

	std::wstring Audio::AudioObject::GetAlbum() {

		return m_AudioProperties->info.album;
	}

	std::wstring Audio::AudioObject::GetYear() {

		if (m_AudioProperties->info.year.compare(L"0") == 0)
			return L"";

		return m_AudioProperties->info.year;
	}

	std::wstring Audio::AudioObject::GetComment() {

		if (m_AudioProperties->info.comment.compare(L"0") == 0)
			return L"";

		return m_AudioProperties->info.comment;
	}

	std::wstring Audio::AudioObject::GetGenre() {
	
		return m_AudioProperties->info.genre;
	}

	std::wstring Audio::AudioObject::GetFormat() {

		return m_AudioProperties->info.format;
	}

	f32 Audio::AudioObject::GetFrequency() {

		return m_AudioProperties->info.freq;
	}

	f32 Audio::AudioObject::GetBitrate() {

		return m_AudioProperties->info.bitrate;
	}

	s32 Audio::AudioObject::GetObjectSize() {

		return m_AudioProperties->info.size;
	}

	f64 Audio::AudioObject::GetLength() {

		return m_AudioProperties->info.length;
	}

	Audio::AudioProperties* Audio::AudioObject::GetAudioProperty() {

		return m_AudioProperties;
	}

	Interface::PlaylistItem* Audio::AudioObject::GetPlaylistItem() {

		return m_PlaylistItem;
	}
