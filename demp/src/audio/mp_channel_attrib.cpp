#include "mp_channel_attrib.h"

#include <algorithm>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <bass.h>
#include <taglib/tag.h>
#include <taglib/fileref.h>


#include "../music_player_settings.h"
#include "mp_audio.h"
#include "../utf8_to_utf16.h"
#include "id3/tag.h"


namespace fs = boost::filesystem;

namespace Audio
{
	namespace Info
	{
		

		std::wstring GetNewString(ID3_FrameID fid, const std::wstring& path);
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

	
	b8 Info::CheckIfAlreadyLoaded(const std::vector<std::wstring*>* v, std::wstring path)
	{
		for (u32 i = 0; i < v->size(); i++)
		{
			if ((*v->at(i)).compare(path) == 0)
			{
				return true;
			}
		}

		return false;
	}

	std::wstring Info::GetFolder(std::wstring path)
	{
		fs::path p(path);

		return p.remove_filename().wstring();
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
		HSTREAM stream;;

		stream = BASS_StreamCreateFile(FALSE, path.c_str(), 0, 0, BASS_STREAM_DECODE | BASS_UNICODE);

		BASS_ChannelGetAttribute(stream, BASS_ATTRIB_FREQ, &info->freq);
		BASS_ChannelGetAttribute(stream, BASS_ATTRIB_BITRATE, &info->bitrate);
		info->size = BASS_ChannelGetLength(stream, BASS_POS_BYTE);
		info->length = BASS_ChannelBytes2Seconds(stream, info->size);

		BASS_StreamFree(stream);
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

	std::wstring Info::GetNewString(ID3_FrameID fid, const std::wstring& path)
	{
		ID3_Tag f;
		f.Link(utf16_to_utf8(path).c_str(), ID3TT_ID3);

		char data[32];
		ID3_Frame* frame = NULL;;
		ID3_Field* field = NULL;

		frame = f.Find(fid);
		if (frame == 0)
		{
			return L"";
		}

		if (!frame->Contains(ID3FN_TEXTENC))
		{
			delete frame;
			return L"";
		}

		if (frame->Contains(ID3FN_TEXT))
		{
			field = frame->GetField(ID3FN_TEXT);
		}

		if (field == 0)
		{
			delete frame;
			return L"";
		}

		field->SetEncoding(ID3TE_ISO8859_1);

		std::string rawText(field->GetRawText());
		std::wstring rawTextW = utf8_to_utf16(rawText); // TODO: GetRawUnicodeText

		return rawTextW;
	}

}