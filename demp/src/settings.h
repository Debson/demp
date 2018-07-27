#pragma once
#ifndef SETTINGS_H
#define SETTINGS_H

#include <iostream>
#include <fstream>

#define MAX_PATH_LENGTH 4

namespace mdEngine
{
namespace Data
{
	std::vector<std::wstring> SupportedFormats =
	{
		L".mp3", 
		L".MP3",
		L".wav"
	};

}
}


#endif // !SETTINGS_H
