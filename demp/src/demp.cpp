#include "demp.h"

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
	mdEngine::MP::RealtimeApplication theApp(demp);

	theApp.Open();
	theApp.Run();

	return 0;
}
