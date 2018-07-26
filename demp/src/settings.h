#pragma once
#ifndef SETTINGS_H
#define SETTINGS_H

#include <iostream>

#define MAX_PATH_LENGTH 4

namespace mdEngine
{
namespace Data
{
	std::vector<std::string> SupportedFormats =
	{
		".mp3", 
		".MP3",
		".wav"
	};

}
}


#endif // !SETTINGS_H
