#include "mp_channel_attrib.h"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <bass.h>

#include "../music_player_settings.h"



namespace fs = boost::filesystem;

namespace Audio
{

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

	void Info::GetInfo(Info::ChannelInfo* info, std::wstring path)
	{
		HSTREAM stream;
		//BASS_CHANNELINFO inf;
		
		stream = BASS_StreamCreateFile(FALSE, path.c_str(), 0, 0, 0);
		//BASS_ChannelGetInfo(stream, &inf);
		/*switch (inf.ctype)
		{
		case(BASS_CTYPE_STREAM_MP1):
			info->ext = L"MP1";
			break;
		case(BASS_CTYPE_STREAM_MP2):
			info->ext = L"MP2";
			break;
		case(BASS_CTYPE_STREAM_MP3):
			info->ext = L"MP3";
			break;
		case(BASS_CTYPE_STREAM_WAV):
		case(BASS_CTYPE_STREAM_WAV_FLOAT):
		case(BASS_CTYPE_STREAM_WAV_PCM):
			info->ext = L"WAV";
			break;
		case(BASS_CTYPE_STREAM_AIFF):
			info->ext = L"AIFF";
			break;
		case(BASS_CTYPE_STREAM_OGG):
			info->ext = L"OGG";
			break;
		}*/

		BASS_ChannelGetAttribute(stream, BASS_ATTRIB_FREQ, &info->freq);
		BASS_ChannelGetAttribute(stream, BASS_ATTRIB_BITRATE, &info->bitrate);
		info->size = BASS_ChannelGetLength(stream, BASS_POS_BYTE);
		info->length = BASS_ChannelBytes2Seconds(stream, info->size);


		BASS_StreamFree(stream);
	}

}