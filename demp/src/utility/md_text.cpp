#include "md_text.h"

#include <sstream>

#include "md_shader.h"
#include "md_shape.h"
#include "../app/realtime_system_application.h"
#include "../graphics/graphics.h"


namespace mdEngine
{
	namespace Text
	{

	}

	void Text::InitializeText()
	{
		
	}
	static int count = 0;
	GLuint Text::LoadText(TTF_Font* font, std::wstring string, SDL_Color color)
	{
		const Uint16* text = NULL;

		//md_log(count);
		count++;

		//assert(sizeof(Uint16) == sizeof(wchar_t));
		//text = reinterpret_cast<const Uint16*>(string.c_str());

		GLuint colors;
		GLuint textTexture;
		SDL_Surface* textSurface;
		GLenum format;

		textSurface = TTF_RenderUTF8_Blended(font, utf16_to_utf8(string).c_str(), color);
		assert(textSurface != NULL);

		colors = textSurface->format->BytesPerPixel;

		if (colors == 4)
		{
			if (textSurface->format->Rmask == 0x000000FF)
				format = GL_RGBA;
			else
				format = GL_BGRA;
		}
		else
		{
			if (textSurface->format->Rmask == 0x000000FF)
				format = GL_RGB;
			else
				format = GL_BGR;
		}

		glGenTextures(1, &textTexture);
		glBindTexture(GL_TEXTURE_2D, textTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, colors, textSurface->w, textSurface->h, 0, format, GL_UNSIGNED_BYTE, textSurface->pixels);

		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		SDL_FreeSurface(textSurface);

		return textTexture;
	}


	void Text::CloseText()
	{
		
	}
		
}