#include "graphics.h"

#include "music_player_graphics.h"

namespace mdEngine
{
	void Graphics::Start()
	{
		MP::StartMainWindow();
	}

	void Graphics::Update()
	{
		MP::UpdateMainWindow();
	}

	void Graphics::Render()
	{
		MP::RenderMainWindow();
	}
	void Graphics::Close()
	{
		MP::CloseMainWindow();
	}

}