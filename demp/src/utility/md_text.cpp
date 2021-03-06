#include "md_text.h"

#include <sstream>
#include <mutex>

#include <gtc/matrix_transform.hpp>

#include "md_shader.h"
#include "md_shape.h"
#include "../app/realtime_system_application.h"
#include "../graphics/graphics.h"
#include "../settings/music_player_string.h"
#include "../utility/utf8_to_utf16.h"
#include "../utility/md_shader.h"


namespace mdEngine
{
	namespace Text
	{
		static std::mutex mutex;
	}

	Text::TextObject::TextObject() : m_Transparency(1.f)
	{
		m_FontPath = Strings::_FONT_PATH;
		m_TextString = "";
		m_TextScale = 1.f;
		m_TextOffset = glm::vec2();
		m_TextTexture = 0;
		m_FontSize = 14;
	}

	Text::TextObject::TextObject(glm::vec3 col, s32 fontSize) : m_Transparency(1.f)
	{
		SDL_Color color = { static_cast<Uint8>(255 * col.x),
							static_cast<Uint8>(255 * col.y),
							static_cast<Uint8>(255 * col.z)
						   };

		m_FontSize = fontSize;
		SetTextColor(col);
		m_FontPath = Strings::_FONT_PATH;
		m_TextString = "";
		m_TextScale = 1.f;
		m_TextOffset = glm::vec2();
		m_TextTexture = 0;
		//TTF_SizeUTF8(m_Font, m_TextString.c_str(), &m_TextSize.x, &m_TextSize.y);
	}

	Text::TextObject::TextObject(glm::vec3 col, std::string text, s32 fontSize) : m_Transparency(1.f)
	{
		SDL_Color color = { static_cast<Uint8>(255 * col.x),
							static_cast<Uint8>(255 * col.y),
							static_cast<Uint8>(255 * col.z)
						  };

		SetTextColor(col);
		m_FontPath = Strings::_FONT_PATH;
		m_TextString = text;
		m_TextScale = 1.f;
		m_TextOffset = glm::vec2();
		m_TextTexture = 0;
	}

	Text::TextObject::TextObject(glm::vec3 col, s32 fontSize, std::string fontPath, f32 transparency) : m_FontSize(fontSize), m_Transparency(transparency)
	{
		SDL_Color color = { static_cast<Uint8>(255 * col.x),
							static_cast<Uint8>(255 * col.y),
							static_cast<Uint8>(255 * col.z)
		};

		SetTextColor(col);
		m_FontPath = fontPath;
		m_TextScale = 1.f;
		m_TextOffset = glm::vec2();
		m_TextTexture = 0;
	}

	Text::TextObject::~TextObject() 
	{ 
		DeleteTexture();
	}

	void Text::TextObject::InitTextTexture()
	{
		if (m_TextTexture == 0)
		{
			TTF_Font* font = TTF_OpenFont(m_FontPath.c_str(), m_FontSize);
			TTF_SizeUTF8(font, m_TextString.c_str(), &m_TextSize.x, &m_TextSize.y);
			m_TextTexture = LoadText(font, m_TextString, m_TextColorSDL);
			TTF_CloseFont(font);
		}
	}

	void Text::TextObject::ReloadTextTexture()
	{
		DeleteTexture();

		TTF_Font* font = TTF_OpenFont(m_FontPath.c_str(), m_FontSize);
		assert(font != NULL);
		TTF_SizeUTF8(font, m_TextString.c_str(), &m_TextSize.x, &m_TextSize.y);

		m_TextTexture = LoadText(font, m_TextString, m_TextColorSDL);
		TTF_CloseFont(font);

	}

	void Text::TextObject::DeleteTexture()
	{
		if (m_TextTexture > 0)
		{
			glDeleteTextures(1, &m_TextTexture);
			m_TextTexture = 0;
		}
	}

	void Text::TextObject::DrawString(b8 drawOverAnything) const
	{
		/*if (m_TextTexture == 0)
			return;*/

		Graphics::Shader::shaderDefault->use();
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(glm::vec2(s32(m_TextPos.x + m_TextOffset.x),
														  s32(m_TextPos.y + m_TextOffset.y)), drawOverAnything ? 1.f : 0.9f));
		model = glm::scale(model, glm::vec3((glm::vec2)m_TextSize * m_TextScale, 1.0));
		Graphics::Shader::shaderDefault->setMat4("model", model);
		Graphics::Shader::shaderDefault->setVec3("color", m_TextColorVec);
		Graphics::Shader::shaderDefault->setFloat("transVal", m_Transparency);
		glBindTexture(GL_TEXTURE_2D, m_TextTexture);
		Graphics::Shader::Draw(Graphics::Shader::shaderDefault);
		Graphics::Shader::shaderDefault->setFloat("transVal", 1.f);
		Graphics::Shader::shaderDefault->setVec3("color", Color::White);
	}

	void Text::TextObject::DrawString(GLuint tex) const
	{
		Graphics::Shader::shaderDefault->use();
		glm::mat4 model;
		model = glm::translate(model, 
							   glm::vec3(glm::vec2(s32(m_TextPos.x + m_TextOffset.x), 
												   s32(m_TextPos.y + m_TextOffset.y)), 
								   0.9f));
		model = glm::scale(model, glm::vec3((glm::vec2)m_TextSize * m_TextScale, 1.f));
		Graphics::Shader::shaderDefault->setMat4("model", model);
		Graphics::Shader::shaderDefault->setVec3("color", m_TextColorVec);
		glBindTexture(GL_TEXTURE_2D, tex);
		Graphics::Shader::Draw(Graphics::Shader::shaderDefault);
		Graphics::Shader::shaderDefault->setVec3("color", Color::White);
	}

	void Text::TextObject::DrawString(mdShader* shader) const
	{
		shader->use();


		glm::mat4 model;
		model = glm::translate(model, glm::vec3(glm::vec2(s32(m_TextPos.x + m_TextOffset.x),
														  s32(m_TextPos.y + m_TextOffset.y)), 0.9f));
		model = glm::scale(model, glm::vec3((glm::vec2)m_TextSize * m_TextScale, 1.0));
		shader->setMat4("model", model);
		shader->setVec3("color", m_TextColorVec);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_TextTexture);
		Graphics::Shader::Draw(shader);
		shader->setVec3("color", Color::White);
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

		m_TextColorSDL = color;
		m_TextColorVec = col;
	}

	void Text::TextObject::SetTextString(std::string str)
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

	b8 Text::TextObject::HasTexture() const
	{
		if (m_TextTexture > 0)
			return true;

		return false;
	}

	GLuint Text::TextObject::GetLoadedTexture()
	{
		GLuint tempText = LoadText(m_TextString, m_TextColorSDL);


		return tempText;
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
		return m_TextColorSDL;
	}

	void Text::TextObject::SetFontSize(s32 fontSize)
	{
		m_FontSize = fontSize;
	}

	void Text::InitializeText()
	{
		
	}

	GLuint Text::LoadText(std::string string, SDL_Color color, std::string fontPath, s32 fontSize)
	{
		TTF_Font* font = TTF_OpenFont(Strings::_FONT_PATH.c_str(), fontSize);

		GLuint colors;
		GLuint textTexture = 0;
		SDL_Surface* textSurface;
		GLenum format;

		textSurface = TTF_RenderUTF8_Blended(font, string.c_str(), color);
		if (textSurface == NULL)
		{
			std::cout << "Font address: " << font << "     String name: " << string.c_str() << "     Color: " << std::to_string(color.r) << "  " << std::to_string(color.g) << "  " << std::to_string(color.b) << std::endl;
			return 0;
		}
		assert(textSurface != NULL);

		colors = textSurface->format->BytesPerPixel;

		format = GL_RGB;

		if (colors == 4)
			format = GL_RGBA;
		
		/*if (bgra == true)
			format = GL_BGRA;*/

		glGenTextures(1, &textTexture);
		glBindTexture(GL_TEXTURE_2D, textTexture);
		GLenum error = glGetError();
		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textSurface->w, textSurface->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, textSurface->pixels);

		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		SDL_FreeSurface(textSurface);

		TTF_CloseFont(font);

		return textTexture;
	}

	GLuint Text::LoadText(TTF_Font* font, std::string string, SDL_Color color)
	{
		GLuint colors;
		GLuint textTexture = 0;
		SDL_Surface* textSurface;
		GLenum format;

		textSurface = TTF_RenderUTF8_Blended(font, string.c_str(), color);
		if (textSurface == NULL)
		{
			MD_ERROR("String name: " + string + "     Color: " + std::to_string(color.r) + "  " + std::to_string(color.g) + "  " + std::to_string(color.b));
			//std::cout << "Font address: " << font << "     String name: " << string.c_str() << "     Color: " << std::to_string(color.r) << "  " << std::to_string(color.g) << "  " << std::to_string(color.b) << std::endl;
			return 0;
		}
		assert(textSurface != NULL);

		colors = textSurface->format->BytesPerPixel;

		format = GL_RGB;

		if (colors == 4)
			format = GL_RGBA;

		/*if (bgra == true)
			format = GL_BGRA;*/

		glGenTextures(1, &textTexture);
		glBindTexture(GL_TEXTURE_2D, textTexture);
		GLenum error = glGetError();

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textSurface->w, textSurface->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, textSurface->pixels);

		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		SDL_FreeSurface(textSurface);

		return textTexture;
	}


	SDL_Texture* Text::LoadTextSDL(SDL_Renderer* renderer, TTF_Font* font, std::string string, SDL_Color color)
	{
		SDL_Surface* textSurface = TTF_RenderUTF8_Blended(font, string.c_str(), color);

		SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, textSurface);

		SDL_FreeSurface(textSurface);

		return tex;
	}


	void Text::CloseText()
	{
		
	}
		
}