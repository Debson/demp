#pragma once
#ifndef MUSIC_PLAYER_GRAPHICS_H
#define MUSIC_PLAYER_GRAPHICS_H
#include <glm.hpp>

namespace mdEngine
{
namespace Graphics
{
	namespace MP
	{
		struct PlaylistItem
		{
			PlaylistItem();

			//void Render();
			static int count;
			glm::vec3 color;
			glm::vec2 pos;
			static glm::vec2 size;
		};

		void StartMainWindow();

		void UpdateMainWindow();

		void RenderMainWindow();

		void RenderTest();

		void CloseMainWindow();

	}
}
}

#endif // !MUSIC_PLAYER_GRAPHICS_H
