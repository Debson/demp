#ifndef MD_TEXT_H
#define MD_TEXT_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <GL/gl3w.h>
#include <glm.hpp>
#include <string>

#include "../utility/md_types.h"

namespace mdEngine
{
	namespace Text
	{
		class TextObject
		{
		public:
			TextObject();
			TextObject(TTF_Font* font, glm::vec3 col);
			TextObject(TTF_Font* font, glm::vec3 col, std::wstring text);
			virtual ~TextObject();


			void InitTextTexture();
			void DrawString() const;

			// Draw text string with specific texture 
			void DrawString(GLuint tex) const;

			void SetTextScale(f32 scale);
			void SetTextColor(glm::vec3 col);
			void SetTextString(std::wstring str);
			void SetTextPos(glm::vec2 pos);
			void SetTextSize(glm::vec2 size);
			void SetTextOffset(glm::vec2 offset);

			/* Initializes and returns a texture(does not initialize any class members, memory safe)*/
			GLuint		GetLoadedTexture();
			TTF_Font*	GetTextFont()  const;
			f32			GetTextScale() const;
			glm::vec2&	GetTextPos();
			glm::vec2	GetTextSize() const;
			SDL_Color	GetTextColor() const;

		protected:
			f32			 m_TextScale;
			GLuint		 m_TextTexture;
			TTF_Font*	 m_Font;
			SDL_Color	 m_TextColor;
			glm::vec2	 m_TextOffset;
			glm::vec2	 m_TextPos;
			glm::ivec2	 m_TextSize;
			std::wstring m_TextString;

		private:
		};

		void InitializeText();

		GLuint LoadText(TTF_Font* font, std::wstring string, SDL_Color color);
		
		void CloseText();
	}

}
#endif // !MD_TEXT_H
