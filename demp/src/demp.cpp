#include "demp.h"

#include <thread>

#include "app/application_window.h"
#include "settings/music_player_string.h"
#include "audio/mp_audio.h"


Demp::~Demp() { }

void Demp::OnWindowOpen()
{
	mMusicPlayer.Start();
}

void Demp::OnRealtimeUpdate()
{
	mMusicPlayer.Update();
}

void Demp::OnRealtimeRender()
{
	mMusicPlayer.Render();
}

void Demp::OnWindowClose()
{
	mMusicPlayer.Close();
}


int main(int argc, char** argv)
{
	mdEngine::Strings::SetProgramPath(argv[0]);

	if (argc == 2)
	{
		Audio::SavePathFromCommandLine(argv[1]);
		/*SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
			"App as default",
			argv[1],
			NULL);*/
	}

	Demp demp;
	mdEngine::App::RealtimeApplication theApp(demp);

	mdEngine::App::WindowProperties windowProperties;
	theApp.SetWindowProperties(windowProperties);
	theApp.Open();

	theApp.Run();

	theApp.Close();

	return 0;
}
