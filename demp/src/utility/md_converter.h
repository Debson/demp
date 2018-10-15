#ifndef MD_CONVERTER_H
#define MD_CONVERTER_H

#include <iostream>

#include <SDL_ttf.h>

#include "md_types.h"
#include "../settings/music_player_settings.h"
#include "../settings/music_player_string.h"

#define BYTE_TO_MB_BINARY 0.00000095367432
#define MB_TO_GB_BINARY 0.0009765625

namespace mdEngine
{
	namespace Converter
	{
		// DD:HH:MM:SS
		std::string SecToProperTimeFormat(f64 secs);

		// Shortest possible representation
		std::string SecToProperTimeFormatShort(f64 secs);

		// GB or MB
		std::string BytesToProperSizeFormat(f64 bytes);

		std::string FrequencyToProperFormat(f32 freq);

		std::string GetShortenString(std::string& in, s32 maxWidth, s32 fontSize = 14, std::string fontPath = Strings::_FONT_PATH);

		std::string GetShortenString(std::string& in, s32 maxWidth, TTF_Font* font);
	}
}
#endif // !MD_CONVERTER_H

