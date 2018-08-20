#ifndef MD_INTERFACE_H
#define MD_INTERFACE_H

#include <glm.hpp>
#include <SDL_ttf.h>
#include <vector>

#include "../utility/md_shader.h"
#include "../ui/music_player_ui_input.h"

using namespace mdEngine::MP::UI;

namespace mdEngine
{
	namespace Interface
	{
		struct Movable
		{
			Movable(glm::vec2 size, glm::vec2 pos);;

			glm::vec2 mSize;
			glm::vec2 mPos;

		};

		struct Resizable
		{
			Resizable(glm::vec2 size, glm::vec2 pos);;

			glm::vec2 size;
			glm::vec2 pos;
		};

		struct Button
		{
			Button();
			virtual ~Button();
			Button(glm::vec2 size, glm::vec2 pos);
			Button(Input::ButtonType type, glm::vec2 size, glm::vec2 pos);

			glm::vec2 mSize;
			glm::vec2 mPos;

			glm::vec2 mMousePos;

			b8 isPressed;
			b8 isReleased;
			b8 isDown;
			b8 hasFocus;
			b8 hasFocusTillRelease;

			b8 wasDown;
		};

		struct TextBoxItem : public Button
		{
			TextBoxItem(const std::wstring name, glm::vec2 itemSize, glm::vec2 itemPos, 
												 glm::vec2 textSize, glm::vec2 textPos,
						GLuint tex);
			virtual ~TextBoxItem();

			glm::vec2 m_Pos;
			glm::vec2 m_Size;
			glm::vec2 m_TextPos;
			glm::vec2 m_TextSize;
			GLuint m_Tex;
			s8 m_Index;

		};

		class PlaylistItem : public Button
		{
		public:
			virtual ~PlaylistItem();
			glm::vec3 mColor;
			glm::vec2 mStartPos;
			SDL_Color mTextColor;

			static s32 mOffsetY;
			static s32 mOffsetIndex;

			void InitFont();
			void InitItem(s32* id);
			void SetColor(glm::vec3 color);
			void DrawDottedBorder(s16 playpos);

			b8 IsPlaying();

			std::wstring GetTitle();

			TTF_Font * mFont;
			glm::ivec2 mTextSize;
			f32 mTextScale;
			u8 clickCount;
		private:
			s32 mID;
			std::string mTitleC;
			GLuint mTitleTexture;
			std::wstring mTitle;
			std::string mInfo;
			std::wstring mPath;
			std::string mLength;
			u16 mFontSize;

		};

		class TextBox
		{
		public:
			TextBox();
			TextBox(MP::UI::Input::ButtonType code, glm::vec2 size, glm::vec2 pos, mdShader* shader);

			void Render();
			void UpdateItemPos();
			void SetPos(glm::vec2 pos);
			void SetSize(glm::vec2 size);
			void SetColor(glm::vec3 color);
			void SetItemScale(f32 scale);
			void SetItemSize(glm::vec2 itemSize);
			void AddItem(const std::wstring itemName);
			glm::vec2 GetPos() const;
			glm::vec2 GetSize() const;
			b8 hasItemFocus(const std::wstring name) const;
			b8 isItemPressed(const std::wstring name) const;

		private:
			mdShader* m_Shader;
			std::vector<TextBoxItem*> m_Items;
			MP::UI::Input::ButtonType m_Type;
			glm::vec2 m_Pos;
			glm::vec2 m_Size;
			glm::vec3 m_Color;
			f32 m_ItemScale;
			glm::vec2 m_ItemSize;
			u16 m_ItemsCount;
			b8 m_Active;

		};


		extern std::vector<std::pair<const std::wstring, Button*>> mdInterfaceButtonContainer;
	}
}
#endif // !MD_INTERFACE_H