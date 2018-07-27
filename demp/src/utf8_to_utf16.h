#pragma once
#ifndef UTF8_TO_UTF16_H
#define UTF8_TO_UTF16_h

#include <iostream>
#include <vector>
#include <fstream>

std::wstring utf8_to_utf16(const std::string& utf8);

#endif // !UTF8_TO_UTF16_H