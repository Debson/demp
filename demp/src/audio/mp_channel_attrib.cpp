#include "mp_channel_attrib.h"

#include <filesystem>
#include <bass.h>

#include "../music_player_settings.h"

namespace fs = std::experimental::filesystem::v1;

namespace Audio
{

	b8 Info::CheckIfAudio(std::wstring path)
	{
		fs::path p(path);

		if (p.extension().has_extension() && p.extension().string().length() < MAX_EXTENSION_LENGTH + 1)
			return true;

		return false;
	}

	std::wstring Info::GetFolder(std::wstring path)
	{
		std::wstring folder(L"");
		char slash = '\\';
		s32 pos_first;
		s32 pos_second;
		
		pos_first = path.find_last_of(slash);
		folder = path.substr(0, pos_first);
		pos_second = folder.find_last_of(slash);
		folder = folder.substr(pos_second + 1, pos_first);


		return folder;
	}

	std::wstring Info::GetName(std::wstring path)
	{
		fs::path ph(path);
		std::wstring name(L"");
		char slash = '\\';
		s32 pos_first;

		s8 extL = ph.extension().string().length() + 1;

		pos_first = path.find_last_of(slash);
		name = path.substr(pos_first + 1, path.length() - pos_first - extL);

		return name;
	}

	void Info::GetInfo(Info::ChannelInfo* info, std::wstring path)
	{
		HSTREAM stream;
		BASS_CHANNELINFO inf;
		
		stream = BASS_StreamCreateFile(FALSE, path.c_str(), 0, 0, 0);
		BASS_ChannelGetInfo(stream, &inf);
		switch (inf.ctype)
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
		}

		BASS_ChannelGetAttribute(stream, BASS_ATTRIB_FREQ, &info->freq);
		BASS_ChannelGetAttribute(stream, BASS_ATTRIB_BITRATE, &info->bitrate);
		info->size = BASS_ChannelGetLength(stream, BASS_POS_BYTE);
		info->length = BASS_ChannelBytes2Seconds(stream, info->size);


		BASS_StreamFree(stream);
	}

}