#ifndef MD_CONVERTER_H
#define MD_CONVERTER_H

#include <iostream>
#include "md_types.h"

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

		std::wstring FrequencyToProperFormat(f32 freq);
	}
}
#endif // !MD_CONVERTER_H

