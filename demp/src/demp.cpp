#include <iostream>

#include "demp.h"

using namespace mdEngine;
using namespace Application;

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
	Demp demp;
	Application::RealtimeApplication theApp(demp);

	theApp.Open();
	theApp.Run();

	return 0;
}
