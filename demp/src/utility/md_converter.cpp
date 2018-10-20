#include "md_converter.h"

#include <string>
#include <sstream>
#include <iomanip>


namespace mdEngine
{
	// DD::HH::MM::SS
	std::string Converter::SecToProperTimeFormat(f64 secs)
	{
		s32 minutes = secs / 60;
		s32 seconds = secs - minutes * 60;
		s32 hours = minutes / 60;
		s32 days = hours / 24;

		minutes -= hours * 60;
		hours -= days * 24;
		
		std::string time;

		// Days
		if (days == 0)
			time += "00";
		else if (days > 0 && days < 10)
		{
			time += "0";
			time += std::to_string(days);
		}
		else
		{
			time += std::to_string(days);
		}

		time += ":";

		// Hours
		if (hours == 0)
			time += "00";
		else if (hours > 0 && hours < 10)
		{
			time += "0";
			time += std::to_string(hours);
		}
		else
		{
			time += std::to_string(hours);
		}

		time += ":";

		// minutes
		if (minutes == 0)
			time += "00";
		else if (minutes > 0 && minutes < 10)
		{
			time += "0";
			time += std::to_string(minutes);
		}
		else
		{
			time += std::to_string(minutes);
		}

		time += ":";

		// seconds
		if (seconds == 0)
		time += "00";
		else if (seconds > 0 && seconds < 10)
		{
			time += "0";
			time += std::to_string(seconds);
		}
		else
		{
			time += std::to_string(seconds);
		}

		return time;
	}

	// Min length 5 chars MM:SS
	std::string Converter::SecToProperTimeFormatShort(f64 secs)
	{
		std::string time = SecToProperTimeFormat(secs);

		while (time[0] == '0' &&
			   time[1] == '0' &&
			   time.length() > 5)
		{
			time = time.substr(3, time.length());
		}


		return time;
	}

	// GB or MB
	std::string Converter::BytesToProperSizeFormat(f64 bytes)
	{
		std::stringstream ss;
		f64 mb = bytes * BYTE_TO_MB_BINARY;
		s32 gb = mb * MB_TO_GB_BINARY;

		mb -= gb / MB_TO_GB_BINARY;

		std::string size;

		if (gb > 0)
		{
			size += std::to_string(gb);
			size += ".";
			if (mb > 0 && mb < 100)
				size += "0";
			mb /= 10;
			if (mb > 100)
				size += "99";
			else
				size += std::to_string((s32)mb);
			size += " GB";
		}
		else
		{
			s32 precision = 0;
			if (mb > 0 && mb < 10)
				precision = 3;
			else if (mb > 10 && mb < 100)
				precision = 4;
			else
				precision = 5;

			ss << std::setprecision(precision) << mb;
			ss >> size;

			size += " MB";
		}

		return size;
	}

	std::string Converter::FrequencyToProperFormat(f32 freq)
	{
		std::string str;

		str = std::to_string((s32)freq / 1000);
		str += " kHz";

		return str;
	}

	std::string Converter::GetShortenString(std::string& in, s32 maxWidth, s32 fontSize, std::string fontPath)
	{
		s32 w, h;
		TTF_Font* font = TTF_OpenFont(fontPath.c_str(), fontSize);

		TTF_SizeUTF8(font, in.c_str(), &w, &h);
		s32 i = 0;
		while (w > maxWidth)
		{
			in = in.substr(0, in.length() - 1);
			TTF_SizeUTF8(font, in.c_str(), &w, &h);
			i++;
		}

		if (i != 0)
		{
			in = in.substr(0, in.length() - 3);
			// delete all spaces
			while(in[in.length() - 1] == ' ')
				in = in.substr(0, in.length() - 1);
			in += "...";
		}

		TTF_CloseFont(font);

		return in;
	}

	std::string Converter::GetShortenString(std::string& in, s32 maxWidth, TTF_Font* font)
	{
		s32 w, h;

		TTF_SizeUTF8(font, in.c_str(), &w, &h);
		s32 i = 0;
		while (w > maxWidth)
		{
			in = in.substr(0, in.length() - 1);
			TTF_SizeUTF8(font, in.c_str(), &w, &h);
			i++;
		}

		if (i != 0)
		{
			in = in.substr(0, in.length() - 3);
			// delete all spaces
			while (in[in.length() - 1] == ' ')
				in = in.substr(0, in.length() - 1);
			in += "...";
		}

		return in;
	}
}