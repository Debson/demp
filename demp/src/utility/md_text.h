#ifndef MD_TEXT_H
#define MD_TEXT_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <GL/gl3w.h>
#include <glm.hpp>
#include <string>

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
			TextObject(TTF_Font* font, glm::vec3 col);
			TextObject(TTF_Font* font, glm::vec3 col, std::wstring text);
			virtual ~TextObject();


			virtual void InitTextTexture();
			virtual void ReloadTextTexture();
			virtual void DeleteTexture();
			virtual void DrawString() const;

			// Draw text string with specific texture 
			virtual void DrawString(GLuint tex) const;
			virtual void DrawString(mdShader* shader) const;

			virtual void SetTextScale(f32 scale);
			virtual void SetTextColor(glm::vec3 col);
			virtual void SetTextString(std::wstring str);
			virtual void SetTextPos(glm::vec2 pos);
			virtual void SetTextSize(glm::vec2 size);
			virtual void SetTextOffset(glm::vec2 offset);

			b8 HasTexture() const;
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
			SDL_Color	 m_TextColorSDL;
			glm::vec2	 m_TextOffset;
			glm::vec2	 m_TextPos;
			glm::vec3	 m_TextColorVec;
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
