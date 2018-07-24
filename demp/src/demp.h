#pragma once
#ifndef SDL_BACKEND_H
#define SDL_BACKEND_h


#include "application_handler_interface.h"
#include "system_application_input.h"
#include "realtime_application.h"
#include "music_player.h"

class Demp : public mdEngine::Application::ApplicationHandlerInterface
{
public:
	Demp();
	virtual ~Demp();

	virtual void OnWindowOpen();
	virtual void OnRealtimeUpdate(void);

private:
	mdEngine::Application::MusicPlayer mMusicPlayer;

};

#endif // !SDL_BACKEND_H
