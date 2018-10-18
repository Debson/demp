#include "md_windows.h"

#include <iostream>
#include <iostream>
#include <sstream>
#include <string>
#include <Windows.h>
#include <shlobj.h>

#include "../app/realtime_system_application.h"
#include "../settings/music_player_settings.h"


namespace mdEngine
{
namespace WindowsPlatform
{
	namespace FileBrowser
	{
		std::string convertedFilenames;
		char filterC[] = "All Files\0*.*\0MP3 Files\0*.mp3\0WAV Files\0*.wav\0WMA Files\0*.wma\0";

	}

	void FileBrowser::OpenFileBrowser()
	{
		\wersyyehfdghfgjhfgh
			//TODO: check this function, somehow it causes some errors to SDL_TTF



		std::string filenameBuffer;
		OPENFILENAME ofn;

		convertedFilenames.erase();

		filenameBuffer.resize(MAX_FILEPATH_BUFFER_SIZE);

		ZeroMemory(&filenameBuffer[0], filenameBuffer.size());
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = Window::GetHWNDWindow();
		ofn.lpstrFilter = filterC;
		ofn.lpstrFile = &filenameBuffer[0];
		ofn.nMaxFile = MAX_FILEPATH_BUFFER_SIZE;
		ofn.lpstrTitle = "Open";
		ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER;


		if (GetOpenFileName(&ofn) == FALSE)
			convertedFilenames = "";

		s8 nulls = 0;
		for (size_t i = 0; i < filenameBuffer.length(); i++)
		{
			if (filenameBuffer[i] == '\0')
			{
				nulls++;
				if (nulls > 1)
					break;
				convertedFilenames += '\n';
				i++;
			}
			else
			{
				nulls = 0;
			}

			convertedFilenames += filenameBuffer[i];
		}
		
		filenameBuffer.erase();
	}

	static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
	{

		if (uMsg == BFFM_INITIALIZED)
		{
			std::string tmp = (const char *)lpData;
			std::cout << "path: " << tmp << std::endl;
			SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
		}

		return 0;
	}

	void FileBrowser::OpenFolderBrowser(std::string savedPath)
	{
		convertedFilenames.erase();

		CHAR path[MAX_FOLDERPATH_BUFFER_SIZE];

		std::string wsaved_path(savedPath.begin(), savedPath.end());
		const char * path_param = wsaved_path.c_str();

		BROWSEINFO bi = { 0 };
		bi.lpszTitle = ("Browse for folder...");
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
		bi.lpfn = BrowseCallbackProc;
		bi.lParam = (LPARAM)path_param;

		LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

		if (pidl != 0)
		{
			//get the name of the folder and put it in path
			SHGetPathFromIDList(pidl, path);

			//free memory used
			IMalloc * imalloc = 0;
			if (SUCCEEDED(SHGetMalloc(&imalloc)))
			{
				imalloc->Free(pidl);
				imalloc->Release();
			}

			convertedFilenames = path;
		}
	}

	std::string FileBrowser::GetFileNames()
	{
		return convertedFilenames;
	}
	
}
}