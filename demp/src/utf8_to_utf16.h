#pragma once
#ifndef UTF8_TO_UTF16_H
#define UTF8_TO_UTF16_h

#include <iostream>
#include <vector>
#include <fstream>

#include <Windows.h>

std::wstring* utf8_to_utf16(const std::string& utf8);

std::string utf16_to_utf8(const wchar_t* buffer, int len);

std::string utf16_to_utf8(const std::wstring& str);

#endif // !UTF8_TO_UTF16_H