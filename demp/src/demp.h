#pragma once
#ifndef DEMP_H
#define DEMP_H

#include "app/application_handler_interface.h"
#include "app/system_application_input.h"
#include "app/realtime_application.h"
#include "player/music_player.h"

class Demp : public mdEngine::App::ApplicationHandlerInterface
{
public:
	virtual ~Demp();

	virtual void OnWindowOpen();
	virtual void OnRealtimeUpdate(void);
	virtual void OnRealtimeRender(void);
	virtual void OnWindowClose(void);

private:
	mdEngine::MP::MusicPlayer mMusicPlayer;

};

#endif // !DEMP_H