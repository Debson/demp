#include "mp_channel_attrib.h"

#include <algorithm>
#include <mutex>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>
#include <bass.h>
//#include <bassenc_mp3.h>
#include <taglib/tag.h>
#include <taglib/fileref.h>


#ifdef _WIN32_

#else

#endif

#include "mp_audio.h"
#include "../settings/music_player_settings.h"
#include "../player/music_player_state.h"
#include "../playlist/music_player_playlist.h"
#include "../graphics/music_player_graphics_playlist.h"
#include "../utility/utf8_to_utf16.h"
#include "../utility/md_util.h"
#include "../utility/md_load_texture.h"
#include "../utility/md_string.h"


namespace fs = boost::filesystem;

namespace Audio
{
	namespace Info
	{
		s32 LoadedItemsInfoCount = 0;
		b8 ItemBeingProcessed;
		std::mutex mutex;

		std::vector<std::string> m_LoadedPaths;

		void DeleteSpecialCharacters(std::string& str);
		void GetID3Info(Info::ID3* info, std::string& path);
		b8 IsSupported(std::string& type);
	}

	void Info::Update()
	{
		if (State::CheckState(State::SortPathsOnNewFileLoad) == true)
		{
			m_LoadedPaths.clear();
			for (auto & i : *Audio::Object::GetAudioObjectContainer())
			{
				m_LoadedPaths.push_back(i->GetPath());
			}

			std::sort(m_LoadedPaths.begin(), m_LoadedPaths.end());

			State::SetState(State::PathContainerSorted);
			State::ResetState(State::SortPathsOnNewFileLoad);
		}
	}

	std::vector<std::string>* Info::GetLoadedPathsContainer()
	{
		return &m_LoadedPaths;
	}

	b8 Info::CheckIfAudio(std::string& path)
	{
		std::string ext = fs::extension(path);
		boost::algorithm::to_lower(ext);

		u32 size = mdEngine::MP::Data::SupportedAudioFormats.size();
		for (u8 i = 0; i < size; i++)
		{
			if (ext.compare(mdEngine::MP::Data::SupportedAudioFormats[i]) == 0)
				return true;
		}

		return false;
	}

	b8 Info::CheckIfImage(const std::string& path)
	{
		for (auto & i : MP::Data::SupportedImageFormats)
		{
			if (fs::extension(path).compare(i) == 0)
				return true;
		}

		return false;
	}

	b8 Info::CheckIfHasItems(std::wstring& path)
	{
		for (auto & i : fs::directory_iterator(path))
		{
			if (fs::is_regular_file(path))
				return true;
		}

		return false;
	}

	b8 Info::IsPathLoaded(std::string& path)
	{
		if (std::binary_search(m_LoadedPaths.begin(), m_LoadedPaths.end(), path) == true)
			return true;

		return false;
	}

	std::string Info::GetFolderPath(std::string& path)
	{
		fs::path p(path);

		return p.remove_filename().string();
	}

	std::string Info::GetFolder(std::string& path)
	{
		// Shouldn't be hardcoded!!! change it
		s16 pos = path.find_last_of('\\');

		return path.substr(pos + 1, path.length());
	}

	// make it faster
	std::string Info::GetCompleteTitle(std::string& path)
	{
		s16 pos = path.find_last_of(L'\\');
		std::string title = path.substr(pos + 1, path.length());
		pos = title.find_last_of(L'.');
		title = title.substr(0, pos);

		return title;
	}

	std::string Info::GetArtist(std::string& path)
	{
		fs::path p(path);

		std::string artist(p.filename().string());
		wchar_t minus = '-';
		s32 pos = artist.find(artist, minus);
		artist = artist.substr(0, pos - 1);

		return artist;
	}

	std::string Info::GetExt(std::string& path)
	{
		fs::path p(path);

		p = fs::extension(p);
		p = p.string().substr(1, p.string().length());
		std::string up(p.string());
		boost::to_upper(up);
		p = up;

		return p.string();
	}

	void Info::GetInfo(std::shared_ptr<Audio::AudioObject> audioObj)
	{
		// Request for a info from main thread has been send, make other threads wait till main thread finishes its work
		while(State::CheckState(State::RequestForInfoLoad) == true && std::this_thread::get_id() != MAIN_THREAD_ID) { }

		ItemBeingProcessed = true;

		std::lock_guard<std::mutex> lockGuard(mutex);

		if (audioObj == nullptr)
			return;

		auto info = audioObj->GetID3Struct();
		*info = ID3();


		HSTREAM stream;
		stream = BASS_StreamCreateFile(FALSE, utf8_to_utf16(audioObj->GetPath()).c_str(), 0, 0, BASS_STREAM_DECODE);
		char *buffer;

		//u32 start = SDL_GetTicks();
		BASS_CHANNELINFO chinf;
		BASS_ChannelGetInfo(stream, &chinf);
		info->ctype = chinf.ctype;
		info->format = Info::GetExt(audioObj->GetPath());


		BASS_ChannelGetAttribute(stream, BASS_ATTRIB_FREQ, &info->freq);
		BASS_ChannelGetAttribute(stream, BASS_ATTRIB_BITRATE, &info->bitrate);
		//BASS_ChannelGetAttribute(stream, BASS_ATTRIB_MUSIC_ACTIVE, &(float)info->channels);
		f32 size = BASS_ChannelGetLength(stream, BASS_POS_BYTE);
		info->length = BASS_ChannelBytes2Seconds(stream, size);
		info->size = fs::file_size(utf8_to_utf16(audioObj->GetPath()));
		BASS_StreamFree(stream);

		/*	Some of the queries to retrieve info can be send from the main thread while
			info is loading, don't add the length and size to overall because other threads
			will process the same object which was in that query second time.
		*/
		if (std::this_thread::get_id() != MAIN_THREAD_ID)
		{
			Graphics::MP::GetPlaylistObject()->AddToItemsDuration(info->length);
			Graphics::MP::GetPlaylistObject()->AddToItemsSize(info->size);
		}

#ifdef _EXTRACT_ID3_TAGS_
		Info::GetID3Info(info, audioObj->GetPath());
#endif

		if (std::this_thread::get_id() != MAIN_THREAD_ID)
		{
			info->loaded = true;
			LoadedItemsInfoCount++;
		}
		
		ItemBeingProcessed = false;
	}

	void Info::GetID3Info(Info::ID3* info, std::string& path)
	{
		if (IsSupported(info->format) == false)
			return;

		TagLib::FileRef file(utf8_to_utf16(path).c_str());
#if 1
		if (file.tag()->isEmpty() == false)
		{
			info->channels = file.audioProperties()->channels();
			TagLib::String buff;
			s32 buffInt = 0;

			buff = file.tag()->title();
			if (buff != TagLib::String::null)
				info->title = utf16_to_utf8(buff.toWString());
			DeleteSpecialCharacters(info->title);

			buff = file.tag()->artist();
			if (buff != TagLib::String::null)
				info->artist = utf16_to_utf8(buff.toWString());
			DeleteSpecialCharacters(info->artist);

			buffInt = file.tag()->track();
			if (buffInt != 0)
				info->track_num = buffInt;

			buff = file.tag()->album();
			if (buff != TagLib::String::null)
				info->album = utf16_to_utf8(buff.toWString());
			DeleteSpecialCharacters(info->album);


			buffInt = file.tag()->year();
			if (buffInt != 0)
				info->year = buffInt;

			/*buff = file.tag()->comment().toCString();;
			if (buff != TagLib::String::null)
				info->comment = buff.toCString();
			DeleteSpecialCharacters(info->comment);*/

			buff = file.tag()->genre().toCString();;
			if (buff != TagLib::String::null)
				info->genre = buff.toCString();
		}
#endif
	}

	b8 Info::IsSupported(std::string& type)
	{
		std::string ext = '.' + tolower(type);
		for (auto & i : MP::Data::NotSupportedByTaglib)
		{
			if (ext.compare(i) == 0)
				return false;
		}

		return true;
	}

	std::string Info::GetProcessedItemsCountStr()
	{
		std::string str = " ";
		if (Audio::Object::GetSize() > 0 && LoadedItemsInfoCount > 0)
		{
			f32 perc = (f32)LoadedItemsInfoCount / (f32)Audio::Object::GetSize();
			str = std::to_string(s32(perc * 100));
			str += "%";
		}

		return str;
	}

	s32 Info::GetProcessedItemsCount()
	{
		return LoadedItemsInfoCount;
	}

	void Info::DeleteSpecialCharacters(std::string& str)
	{
		for (auto & i : str)
		{
			if (i == '\r' || i == '\n')
				i = ' ';
		}
	}
}