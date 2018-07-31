#pragma once
#ifndef SETTINGS_H
#define SETTINGS_H

#include <iostream>
#include <fstream>
#include <glm.hpp>

#define MAX_EXTENSION_LENGTH 4

namespace mdEngine
{
	namespace Data
	{
#ifdef _WIN32_
		std::vector<std::wstring> SupportedFormats =
		{
			L".mp3",
			L".wav"
		};
#else
		std::vector<const char*> SupportedFormats =
		{
			".mp3",
			".wav"
		};
#endif

	}
}

#endif // !SETTINGS_H
