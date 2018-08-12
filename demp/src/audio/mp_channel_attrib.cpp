#include "mp_channel_attrib.h"

#include <algorithm>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <bass.h>

#include "../music_player_settings.h"
#include "mp_audio.h"
#include "../utf8_to_utf16.h"

namespace fs = boost::filesystem;

namespace Audio
{
	namespace Info
	{
		
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

		return it != v->end() ? true : false;

		return false;
	}

	std::wstring Info::GetFolder(std::wstring path)
	{
		fs::path p(path);

		return p.parent_path().leaf().wstring();
	}

	std::wstring Info::GetName(std::wstring path)
	{
		fs::path p(path);

		return p.filename().wstring();
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

	void Info::GetInfo(Info::ChannelInfo* info, std::wstring path)
	{
		HSTREAM stream;
		
		stream = BASS_StreamCreateFile(FALSE, path.c_str(), 0, 0, 0);

		BASS_ChannelGetAttribute(stream, BASS_ATTRIB_FREQ, &info->freq);
		BASS_ChannelGetAttribute(stream, BASS_ATTRIB_BITRATE, &info->bitrate);
		info->size = BASS_ChannelGetLength(stream, BASS_POS_BYTE);
		info->length = BASS_ChannelBytes2Seconds(stream, info->size);


		BASS_StreamFree(stream);
	}

}