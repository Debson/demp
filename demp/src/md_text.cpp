#include "md_text.h"

#include <sstream>

#include "realtime_system_application.h"

#include "md_shader.h"
#include "md_shape.h"
#include "graphics.h"

namespace mdEngine
{

	struct Character {
		GLuint TextureID;   // ID handle of the glyph texture
		glm::ivec2 Size;    // Size of glyph
		glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
		GLuint Advance;    // Horizontal offset to advance to next glyph
	};

	std::map<GLchar, Character> Characters;

	namespace Text
	{
		typedef std::basic_string<Uint16, std::char_traits<Uint16>, std::allocator<Uint16> > u16string;

	}

	void Text::InitializeText()
	{
		
	}

	GLuint Text::LoadText(TTF_Font* font, std::wstring string, SDL_Color color)
	{
		const Uint16* text = 0;

		assert(sizeof(Uint16) == sizeof(wchar_t));
		text = reinterpret_cast<const Uint16*>(string.c_str());

		GLuint colors;
		GLuint textTexture;
		SDL_Surface* textSurface;
		GLenum format;

		textSurface = TTF_RenderUNICODE_Blended(font, text, color);
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

	void Text::RenderText()
	{
		/*Graphics::Shader::shaderDefault->use();
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(100.f, 50.f, 1.f));
		model = glm::scale(model, glm::vec3(100, 20, 1.f));
		glm::vec3 color(1.f);
		Graphics::Shader::shaderDefault->setMat4("model", model);
		Graphics::Shader::shaderDefault->setVec3("color", color);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textTexture);
		Graphics::Shader::Draw();*/

	}

	void Text::CloseText()
	{
		
	}
		
}