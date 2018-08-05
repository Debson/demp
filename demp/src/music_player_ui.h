#pragma once
#ifndef MUSIC_PLAYER_UI_H
#define MUSIC_PLAYER_UI_H

#include <glm.hpp>
#include <SDL_ttf.h>
#include <GL/gl3w.h>
#include "../external/imgui/imgui.h"
#include "md_types.h"

#include "music_player_ui_input.h"

namespace mdEngine
{
namespace MP
{
	namespace UI
	{
		namespace Data
		{
			extern u16 _PLAYLIST_CHOOSE_ITEM_DELAY;

			extern glm::vec2 _MIN_PLAYER_SIZE;

			extern glm::vec2 _DEFAULT_PLAYER_POS;
			extern glm::vec2 _DEFAULT_PLAYER_SIZE;

			extern glm::vec2 _DEFAULT_WINDOW_POS;
			extern glm::vec2 _DEFAULT_WINDOW_SIZE;

			extern glm::vec2 _MAIN_BACKGROUND_POS;
			extern glm::vec2 _MAIN_BACKGROUND_SIZE;

			extern glm::vec2 _MAIN_FOREGROUND_POS;
			extern glm::vec2 _MAIN_FOREGROUND_SIZE;

			extern glm::vec2 _PLAYLIST_FOREGROUND_POS;
			extern glm::vec2 _PLAYLIST_FOREGROUND_SIZE;

			extern glm::vec2 _VOLUME_BAR_POS;
			extern glm::vec2 _VOLUME_BAR_SIZE;

			extern glm::vec2 _MUSIC_PROGRESS_BAR_POS;
			extern glm::vec2 _MUSIC_PROGRESS_BAR_SIZE;

			extern glm::vec2 _VOLUME_SPEAKER_POS;
			extern glm::vec2 _VOLUME_SPEAKER_SIZE;

			extern glm::vec2 _MUSIC_PROGRESS_BAR_DOT_POS;
			extern glm::vec2 _VOLUME_BAR_DOT_POS;
			extern glm::vec2 _SLIDER_DOT_SIZE;

			extern glm::vec2 _UI_WINDOW_BAR_POS;
			extern glm::vec2 _UI_WINDOW_BAR_SIZE;

			extern glm::vec2 _EXIT_BUTTON_POS;
			extern glm::vec2 _EXIT_BUTTON_SIZE;

			extern glm::vec2 _MINIMIZE_BUTTON_POS;
			extern glm::vec2 _MINIMIZE_BUTTON_SIZE;

			extern glm::vec2 _STAY_ON_TOP_BUTTON_POS;
			extern glm::vec2 _STAY_ON_TOP_BUTTON_SIZE;

			extern glm::vec2 _PLAY_BUTTON_POS;
			extern glm::vec2 _PLAY_BUTTON_SIZE;

			extern glm::vec2 _NEXT_BUTTON_POS;
			extern glm::vec2 _NEXT_BUTTON_SIZE;

			extern glm::vec2 _PREVIOUS_BUTTON_POS;
			extern glm::vec2 _PREVIOUS_BUTTON_SIZE;

			extern glm::vec2 _SHUFFLE_BUTTON_POS;
			extern glm::vec2 _SHUFFLE_BUTTON_SIZE;

			extern glm::vec2 _REPEAT_BUTTON_POS;
			extern glm::vec2 _REPEAT_BUTTON_SIZE;

			extern glm::vec2 _DOT_BUTTON_STATE_SIZE;

			extern glm::vec2 _PLAYLIST_BUTTON_POS;
			extern glm::vec2 _PLAYLIST_BUTTON_SIZE;

			extern glm::vec2 _PLAYLIST_ITEMS_SURFACE_POS;
			extern glm::vec2 _PLAYLIST_ITEMS_SURFACE_SIZE;
		
			extern glm::vec2 _PLAYLIST_ITEM_SIZE;

			extern glm::vec2 _PLAYLIST_SCROLL_BAR_POS;
			extern glm::vec2 _PLAYLIST_SCROLL_BAR_SIZE;
		}

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
			char* mTitleC;
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
