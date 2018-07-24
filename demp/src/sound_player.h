#pragma once

#include <SDL.h>
#include <SDL_mixer.h>

#include "md_types.h"
#include "md_util.h"

namespace mdEngine
{
namespace SoundPlayer
{
	struct Song
	{
		const char* path;
		Mix_Music* m_Music;

		Song() : m_Music(NULL)
		{

		}

		~Song()
		{
			//Mix_FreeMusic(m_Music);
		}

		void load(const char *path)
		{
			this->path = path;
			m_Music = Mix_LoadMUS(path);
			if (m_Music == NULL)
			{
				MD_SDL_ERROR(path);
				return;
			}
		}

		Mix_Music* get()
		{
			return m_Music;
		}

	};

	

}

}
