#include "md_text.h"

#include <sstream>

#include <gtc/matrix_transform.hpp>

#include "md_shader.h"
#include "md_shape.h"
#include "../app/realtime_system_application.h"
#include "../graphics/graphics.h"
#include "../utility/utf8_to_utf16.h"
#include "../utility/md_shader.h"


namespace mdEngine
{
	namespace Text
	{

	}

	Text::TextObject::TextObject()
	{
		m_TextString = L"";
		m_Font = NULL;
		m_TextScale = 1.f;
		m_TextOffset = glm::vec2();
	}

	Text::TextObject::TextObject(TTF_Font* font, glm::vec3 col)
	{
		SDL_Color color = { static_cast<Uint8>(255 * col.x),
							static_cast<Uint8>(255 * col.y),
							static_cast<Uint8>(255 * col.z)
						   };

		m_Font = font;
		m_TextColor = color;
		m_TextString = L"";
		m_TextScale = 1.f;
		m_TextOffset = glm::vec2();
		TTF_SizeUTF8(m_Font, utf16_to_utf8(m_TextString).c_str(), &m_TextSize.x, &m_TextSize.y);
	}

	Text::TextObject::TextObject(TTF_Font* font, glm::vec3 col, std::wstring text)
	{
		SDL_Color color = { static_cast<Uint8>(255 * col.x),
							static_cast<Uint8>(255 * col.y),
							static_cast<Uint8>(255 * col.z)
						  };

		m_Font = font;
		m_TextColor = color;
		m_TextString = text;
		m_TextScale = 1.f;
		m_TextOffset = glm::vec2();
		TTF_SizeUTF8(m_Font, utf16_to_utf8(m_TextString).c_str(), &m_TextSize.x, &m_TextSize.y);
	}

	Text::TextObject::~TextObject() { }

	void Text::TextObject::InitTextTexture()
	{
		TTF_SizeUTF8(m_Font, utf16_to_utf8(m_TextString).c_str(), &m_TextSize.x, &m_TextSize.y);
		m_TextTexture = LoadText(m_Font, m_TextString, m_TextColor);
	}

	void Text::TextObject::DrawString() const
	{
		Graphics::Shader::shaderDefault->use();
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(m_TextPos, 0.9));
		model = glm::scale(model, glm::vec3((glm::vec2)m_TextSize * m_TextScale, 1.0));
		Graphics::Shader::shaderDefault->setMat4("model", model);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_TextTexture);
		Graphics::Shader::Draw(Graphics::Shader::shaderDefault);
	}

	void Text::TextObject::DrawString(GLuint tex) const
	{
		Graphics::Shader::shaderDefault->use();
		glm::mat4 model;
		model = glm::translate(model, 
							   glm::vec3(glm::vec2(m_TextPos.x + m_TextOffset.x, 
													m_TextPos.y + m_TextOffset.y), 
								         0.9)
							  );
		model = glm::scale(model, glm::vec3((glm::vec2)m_TextSize * m_TextScale, 1.0));
		Graphics::Shader::shaderDefault->setMat4("model", model);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);
		Graphics::Shader::Draw(Graphics::Shader::shaderDefault);
	}

	void Text::TextObject::SetTextScale(f32 scale)
	{
		m_TextScale = scale;
	}

	void Text::TextObject::SetTextColor(glm::vec3 col)
	{
		SDL_Color color = { static_cast<Uint8>(255 * col.x),
							static_cast<Uint8>(255 * col.y),
							static_cast<Uint8>(255 * col.z)
						  };

		m_TextColor = color;
	}

	void Text::TextObject::SetTextString(std::wstring str)
	{
		m_TextString = str;
	}

	void Text::TextObject::SetTextPos(glm::vec2 pos)
	{
		m_TextPos = pos;
	}

	void Text::TextObject::SetTextSize(glm::vec2 size)
	{
		m_TextSize = size;
	}

	void Text::TextObject::SetTextOffset(glm::vec2 offset)
	{
		m_TextOffset = offset;
	}

	GLuint Text::TextObject::GetLoadedTexture()
	{
		GLuint tempText = LoadText(m_Font, m_TextString, m_TextColor);

		return tempText;
	}

	TTF_Font* Text::TextObject::GetTextFont() const
	{
		return m_Font;
	}
	
	f32 Text::TextObject::GetTextScale() const
	{
		return m_TextScale;
	}

	glm::vec2& Text::TextObject::GetTextPos()
	{
		return m_TextPos;
	}

	glm::vec2 Text::TextObject::GetTextSize() const
	{
		return m_TextSize;
	}

	SDL_Color Text::TextObject::GetTextColor() const
	{
		return m_TextColor;
	}


	void Text::InitializeText()
	{
		
	}

	GLuint Text::LoadText(TTF_Font* font, std::wstring string, SDL_Color color)
	{
		const Uint16* text = NULL;

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