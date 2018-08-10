#include "demp.h"

#include <thread>

#include "application_window.h"

Demp::Demp()
{

}

Demp::~Demp()
{
	
}

void Demp::OnWindowOpen()
{
	mMusicPlayer.Start();
	// MusciPlayer open
}

void Demp::OnRealtimeUpdate()
{
	// Music player update
	mMusicPlayer.Update();
}

void Demp::OnRealtimeRender()
{
	/* Render music player */
	mMusicPlayer.Render();
}

void Demp::OnWindowClose()
{
	mMusicPlayer.Close();
}


int main(int argc, char** argv)
{
	Demp demp;
	mdEngine::App::RealtimeApplication theApp(demp);

	mdEngine::App::WindowProperties windowProperties;
	theApp.SetWindowProperties(windowProperties);
	theApp.Open();

	theApp.Run();

	return 0;
}
