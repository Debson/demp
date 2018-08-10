#ifndef MD_WINDOWS_H
#define MD_WINDOWS_H

#include "md_types.h"
#include "music_player_settings.h"

namespace mdEngine
{
namespace WindowsPlatform
{
	namespace FileBrowser
	{
		void OpenFileBrowser();

		void OpenFolderBrowser(std::string savedPath);

		std::wstring GetFileNames();
	}

}
}

namespace mdWindowsFile = mdEngine::WindowsPlatform::FileBrowser;



#endif // !MD_WINDOWS_H

