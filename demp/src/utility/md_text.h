#ifndef MD_TEXT_H
#define MD_TEXT_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <GL/gl3w.h>
#include <glm.hpp>
#include <string>

#include "../settings/music_player_settings.h"
#include "../settings/music_player_string.h"
#include "../utility/md_types.h"
#include "md_shader.h"

namespace mdEngine
{
	namespace Text
	{
		class TextObject
		{
		public:
			TextObject();
			explicit TextObject(glm::vec3 col, s32 fontSize = 14);
			TextObject(glm::vec3 col, std::string text, s32 fontSize = 14);
			virtual ~TextObject();

			virtual void InitTextTexture();
			virtual void ReloadTextTexture();
			// Awlays call it to initialize rendere
			virtual void DeleteTexture();
			virtual void DrawString(b8 drawOverAnything = false) const;

			// Draw text string with specific texture 
			virtual void DrawString(GLuint tex) const;
			virtual void DrawString(mdShader* shader) const;

			virtual void SetTextScale(f32 scale);
			virtual void SetTextColor(glm::vec3 col);
			virtual void SetTextString(std::string str);
			virtual void SetTextPos(glm::vec2 pos);
			virtual void SetTextSize(glm::vec2 size);
			virtual void SetTextOffset(glm::vec2 offset);

			b8 HasTexture() const;
			/* Initializes and returns a texture(does not initialize any class members, memory safe)*/
			GLuint		GetLoadedTexture();
			f32			GetTextScale() const;
			glm::vec2&	GetTextPos();
			glm::vec2	GetTextSize() const;
			SDL_Color	GetTextColor() const;

		protected:
			f32			  m_TextScale;
			s32			  m_FontSize;
			GLuint		  m_TextTexture;
			SDL_Color	  m_TextColorSDL;
			glm::vec2	  m_TextOffset;
			glm::vec2	  m_TextPos;
			glm::vec3	  m_TextColorVec;
			glm::ivec2	  m_TextSize;
			std::string	  m_TextString;

		private:
		};

		void InitializeText();

		GLuint			LoadText(std::string string, SDL_Color color, std::string fontPath = Strings::_FONT_PATH, s32 fontSize = 14);
		GLuint			LoadText(TTF_Font* font, std::string string, SDL_Color color);
		SDL_Texture*	LoadTextSDL(SDL_Renderer* renderer, TTF_Font* font, std::string string, SDL_Color color);
		
		void CloseText();
	}
}
#endif // !MD_TEXT_H
