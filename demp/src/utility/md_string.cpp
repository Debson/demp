#include "md_string.h"


std::string mdEngine::tolower(std::string& str)
{
	std::string newStr;
	for (auto & i : str)
		newStr += i | 32;
	return newStr;
}


void mdEngine::tolowerP(std::string& str)
{
	for (auto & i : str)
		i |= 32;
}