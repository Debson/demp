#ifndef MD_TEXT_H
#define MD_TEXT_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <GL/gl3w.h>
#include <glm.hpp>
#include <map>

#include <string>

namespace mdEngine
{
	namespace Text
	{
		
		void InitializeText();

		GLuint LoadText(TTF_Font* font, std::wstring string, SDL_Color color);

		void RenderText();
		
		void CloseText();
	}

}
#endif // !MD_TEXT_H
