#pragma once
#ifndef DEMP_H
#define DEMP_H

#include "application_handler_interface.h"
#include "system_application_input.h"
#include "realtime_application.h"
#include "music_player.h"

class Demp : public mdEngine::App::ApplicationHandlerInterface
{
public:
	Demp();
	virtual ~Demp();

	virtual void OnWindowOpen();
	virtual void OnRealtimeUpdate(void);
	virtual void OnRealtimeRender(void);

private:
	mdEngine::MP::MusicPlayer mMusicPlayer;

};

#endif // !DEMP_H
