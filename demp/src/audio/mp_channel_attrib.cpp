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
#include "../utility/utf8_to_utf16.h"
#include "../utility/md_util.h"

namespace fs = boost::filesystem;

namespace Audio
{
	namespace Info
	{
		s32 LoadedItemsInfoCount = 0;
		b8 SingleItemInfoLoaded(false);
		std::mutex mutex;
	}

	b8 Info::CheckIfAudio(std::wstring path)
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

	b8 Info::CheckIfHasItems(std::wstring path)
	{
		fs::path p(path);

		for (auto & i : fs::directory_iterator(path))
		{
			if (fs::is_regular_file(i.path()))
				return true;
		}

		return false;
	}

	
	b8 Info::CheckIfAlreadyLoaded(std::vector<std::wstring>* v, std::wstring path)
	{
		auto it = std::find(v->begin(), v->end(), path);
		if (it != v->end())
			return true;

		return false;
	}

	std::wstring Info::GetFolderPath(std::wstring path)
	{
		fs::path p(path);

		return p.remove_filename().wstring();
	}

	std::wstring Info::GetFolder(std::wstring path)
	{
		// Shouldn't be hardcoded!!! change it
		s32 pos = path.find_last_of('\\');

		return path.substr(pos + 1, path.length());
	}

	std::wstring Info::GetCompleteTitle(std::wstring path)
	{
		fs::path p(path);

		s16 len = p.filename().string().length();
		s16 extLen = p.extension().string().length();
		std::wstring title = p.filename().wstring();
		title = title.substr(0, len - extLen);

		return title;
	}

	std::wstring Info::GetArtist(std::wstring path)
	{
		fs::path p(path);

		std::wstring artist(p.filename().wstring());
		wchar_t minus = '-';
		s32 pos = artist.find(artist, minus);
		artist = artist.substr(0, pos - 1);

		return artist;
	}

	std::wstring Info::GetExt(std::wstring path)
	{
		fs::path p(path);

		p = fs::extension(p);
		p = p.string().substr(1, p.string().length());
		std::string up(p.string());
		boost::to_upper(up);
		p = up;

		return p.wstring();
	}

	
	void Info::GetInfo(Info::ID3* info, std::wstring path)
	{
		std::lock_guard<std::mutex> lockGuard(mutex);

		HSTREAM stream;;
		stream = BASS_StreamCreateFile(FALSE, path.c_str(), 0, 0, BASS_STREAM_DECODE);

		boost::intmax_t fileSize = boost::filesystem::file_size(path);

		BASS_ChannelGetAttribute(stream, BASS_ATTRIB_FREQ, &info->freq);
		BASS_ChannelGetAttribute(stream, BASS_ATTRIB_BITRATE, &info->bitrate);
		BASS_ChannelGetAttribute(stream, BASS_ATTRIB_MUSIC_ACTIVE, &info->channels);
		f32 size = BASS_ChannelGetLength(stream, BASS_POS_BYTE);
		info->length = BASS_ChannelBytes2Seconds(stream, size);
		info->size = fileSize;

		LoadedItemsInfoCount++;

		BASS_StreamFree(stream);

		SingleItemInfoLoaded = true;
	}

	void Info::GetID3Info(Info::ID3* info, std::wstring path)
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