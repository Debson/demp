#include "mp_channel_attrib.h"

#include <algorithm>
#include <mutex>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>
#include <bass.h>
#include <taglib/tag.h>
#include <taglib/fileref.h>

#include "mp_audio.h"
#include "../settings/music_player_settings.h"
#include "../player/music_player_state.h"
#include "../graphics/music_player_graphics_playlist.h"
#include "../utility/utf8_to_utf16.h"
#include "../utility/md_util.h"

namespace fs = boost::filesystem;

namespace Audio
{
	namespace Info
	{
		s32 LoadedItemsInfoCount = 0;
		b8 ItemBeingProcessed;
		std::mutex mutex;
	}


	b8 Info::CheckIfAudio(std::wstring& path)
	{
		fs::path p(path);

		std::string ext = fs::extension(path);
		boost::algorithm::to_lower(ext);

		for (u8 i = 0; i < mdEngine::MP::Data::SupportedFormats.size(); i++)
		{
			if(ext.compare(mdEngine::MP::Data::SupportedFormats[i]) == 0)
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

	
	b8 Info::IsPathLoaded(std::wstring& path)
	{
		for (auto & i : *Audio::Object::GetAudioObjectContainer())
		{
			if (path.compare(i->GetPath()) == 0)
			{
				return true;
			}
		}

		return false;
	}

	std::wstring Info::GetFolderPath(std::wstring& path)
	{
		fs::path p(path);

		return p.remove_filename().wstring();
	}

	std::wstring Info::GetFolder(std::wstring& path)
	{
		// Shouldn't be hardcoded!!! change it
		s16 pos = path.find_last_of('\\');

		return path.substr(pos + 1, path.length());
	}

	// make it faster
	std::wstring Info::GetCompleteTitle(std::wstring& path)
	{
		s16 pos = path.find_last_of(L'\\');
		std::wstring title = path.substr(pos + 1, path.length());
		pos = title.find_last_of(L'.');
		title = title.substr(0, pos);

		return title;
	}

	std::wstring Info::GetArtist(std::wstring& path)
	{
		fs::path p(path);

		std::wstring artist(p.filename().wstring());
		wchar_t minus = '-';
		s32 pos = artist.find(artist, minus);
		artist = artist.substr(0, pos - 1);

		return artist;
	}

	std::wstring Info::GetExt(std::wstring& path)
	{
		fs::path p(path);

		p = fs::extension(p);
		p = p.string().substr(1, p.string().length());
		std::string up(p.string());
		boost::to_upper(up);
		p = up;

		return p.wstring();
	}

	void Info::GetInfo(std::shared_ptr<Audio::AudioObject> audioObj)
	{
		ItemBeingProcessed = true;
		std::lock_guard<std::mutex> lockGuard(mutex);

		if(audioObj == nullptr)
			return;

		auto info = audioObj->GetID3Struct();

		HSTREAM stream;;
		stream = BASS_StreamCreateFile(FALSE, audioObj->GetPath().c_str(), 0, 0, BASS_STREAM_DECODE);

		boost::intmax_t fileSize = boost::filesystem::file_size(audioObj->GetPath());

		BASS_ChannelGetAttribute(stream, BASS_ATTRIB_FREQ, &info->freq);
		BASS_ChannelGetAttribute(stream, BASS_ATTRIB_BITRATE, &info->bitrate);
		BASS_ChannelGetAttribute(stream, BASS_ATTRIB_MUSIC_ACTIVE, &info->channels);
		f32 size = BASS_ChannelGetLength(stream, BASS_POS_BYTE);
		info->length = BASS_ChannelBytes2Seconds(stream, size);
		info->size = fileSize;
		BASS_StreamFree(stream);

		Graphics::MP::GetPlaylistObject()->AddToItemsDuration(info->length);
		Graphics::MP::GetPlaylistObject()->AddToItemsSize(info->size);

		info->loaded = true;
		LoadedItemsInfoCount++;
		ItemBeingProcessed = false;
	}

	void Info::GetID3Info(Info::ID3* info, std::wstring& path)
	{
		TagLib::FileRef file(path.c_str());
		TagLib::String buff = file.tag()->title();
		s32 buffInt = 0;
		if (buff != TagLib::String::null)
			info->title = buff.toWString();
		buff = file.tag()->artist();
		if (buff != TagLib::String::null)
			info->artist = buff.toWString();

		info->track_num = std::to_wstring(file.tag()->track());
		buff = file.tag()->album();
		if (buff != TagLib::String::null)
			info->album = buff.toWString();

		info->year = std::to_wstring(file.tag()->year());
		info->comment = file.tag()->comment().toWString();
		info->genre = file.tag()->genre().toWString();
	}

	std::wstring Info::GetProcessedItemsCountStr()
	{
		std::wstring str = L" ";
		if (Audio::Object::GetSize() > 0 && LoadedItemsInfoCount > 0)
		{
			f32 perc = (f32)LoadedItemsInfoCount / (f32)Audio::Object::GetSize();
			str = std::to_wstring(s32(perc * 100));
			str += L"%";
		}

		return str;
	}

	s32 Info::GetProcessedItemsCount()
	{
		return LoadedItemsInfoCount;
	}

	void Info::WaitTillFileInfoLoaded()
	{

		md_log(LoadedItemsInfoCount);
	}
}