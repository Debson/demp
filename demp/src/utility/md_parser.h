#ifndef MD_PARSER_H
#define MD_PARSER_H

#include <vector>
#include <iostream>
#include <string>
#include "md_types.h"



namespace mdEngine
{
	namespace Parser
	{
		b8 InitialzieParser();

		b8 SavePathsToFile(const std::string& fileName);
		b8 ReadPathsFromFile(const std::string& fileName);

		b8 SaveSettingsToFile(const std::string& fileName);
		b8 ReadSettingsFromFile(const std::string& fileName);
;

		std::string GetString(const std::string& fileName, const std::string& valName);
		s32 GetInt(const std::string& fileName, const std::string& valName);
		f32 GetFloat(const std::string& fileName, const std::string& valName);
		std::wstring GetStringUTF8(const std::string& fileName, const std::string& valName);

	}
}


#endif // !MD_PARSER_H
