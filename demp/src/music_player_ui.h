#pragma once
#ifndef MUSIC_PLAYER_UI_H
#define MUSIC_PLAYER_UI_H

#include <glm.hpp>
#include <SDL_ttf.h>
#include <GL/gl3w.h>
#include "../external/imgui/imgui.h"

#include "music_player_ui_input.h"

namespace mdEngine
{
namespace MP
{
	namespace UI
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

		class PlaylistItem : public Button
		{
		public:
			virtual ~PlaylistItem();
			glm::vec3 mColor;
			glm::vec2 mStartPos;
			SDL_Color mTextColor;

			static s32 mOffsetY;
			static s32 mCount;
			static s32 mOffsetIndex;

			void InitFont();
			void InitItem();
			void UpdateItem();
			void SetColor(glm::vec3 color);
			void DrawDottedBorder(s16 playpos);

			b8 IsPlaying();

			std::wstring GetTitle();

			s32 mID;
			TTF_Font * mFont;
			glm::ivec2 mTextSize;
			f32 mTextScale;
			u8 clickCount;
		private:
			std::string mTitleC;
			GLuint mTitleTexture;
			std::wstring mTitle;
			std::string mInfo;
			std::wstring mPath;
			std::string mLength;
			u16 mFontSize;

		};

		extern std::vector<std::pair<Input::ButtonType, Button*>> mdPlaylistButtonsContainer;
		extern std::vector<std::pair<Input::ButtonType, Button*>> mdButtonsContainer;
		extern std::vector<PlaylistItem*> mdItemContainer;

		/* useless */
		extern ImVec4 ClearColor;

		void Start();

		void Update();

		void Render();

	}
}
}


#endif // !MUSIc_PLAYER_UI_H
