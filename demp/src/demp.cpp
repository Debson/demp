#include "demp.h"
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


int main(int argc, char** argv)
{
	setlocale(LC_ALL, "");
	Demp demp;
	mdEngine::App::RealtimeApplication theApp(demp);

	mdEngine::App::WindowProperties windowProperties;
	theApp.SetWindowProperties(windowProperties);
	theApp.Open();
	theApp.Run();

	return 0;
}
