#pragma once
#ifndef SETTINGS_H
#define SETTINGS_H

#include <iostream>
#include <fstream>

#define MAX_EXTENSION_LENGTH 4

namespace mdEngine
{
namespace Data
{
#ifdef _WIN32_
	std::vector<std::wstring> SupportedFormats =
	{
		L".mp3", 
		L".MP3",
		L".wav"
	};
#else
	std::vector<const char*> SupportedFormats =
	{
		".mp3",
		".MP3",
		".wav"
	};
#endif

}
}


#endif // !SETTINGS_H
