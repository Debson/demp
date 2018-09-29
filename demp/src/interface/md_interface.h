#ifndef MD_INTERFACE_H
#define MD_INTERFACE_H

#include <glm.hpp>
#include <SDL_ttf.h>
#include <vector>

#include "../utility/md_shader.h"
#include "../ui/music_player_ui_input.h"
#include "../utility/md_text.h"
#include "../utility/md_time.h"

using namespace mdEngine::MP::UI;
using namespace mdEngine::Text;

namespace mdEngine
{
	namespace Interface
	{
		void CloseInterface();

		void PrintSeparatorAndItsSubFiles();

		struct Movable
		{
			Movable();
			Movable(glm::vec2 size, glm::vec2 pos);;

			glm::vec2 m_Size;
			glm::vec2 m_Pos;
			b8 hasFocus;

		};

		struct Resizable
		{
			Resizable();
			Resizable(glm::vec2 size, glm::vec2 pos);

			glm::vec2 m_Size;
			glm::vec2 m_Pos;
			b8 hasFocus;
		};

		class Button
		{
		public:
			Button();
			Button(glm::vec2 size, glm::vec2 pos);
			Button(Input::ButtonType type, glm::vec2 size, glm::vec2 pos);
			virtual ~Button();


			virtual void SetButtonPos(glm::vec2 pos);
			void SetButtonPosY(f32 posY);
			void SetButtonSize(glm::vec2 size);
			void ResetState();

			glm::vec2& GetButtonPos();
			glm::vec2& GetButtonSize();
			glm::vec2& GetInButtonMousePos();

			b8 isPressed;
			b8 isReleased;
			b8 isDown;
			b8 topHasFocus;
			b8 bottomHasFocus;
			b8 hasFocus;
			b8 hasFocusTillRelease;
			b8 wasDown;
		protected:
			glm::vec2 m_ButtonSize;
			glm::vec2 m_ButtonPos;
			glm::vec2 m_MousePos;
		};


		class PlaylistItem : public Button, public TextObject
		{
		public:
			PlaylistItem();
			virtual ~PlaylistItem();
			static s32 OffsetIndex;

			void DrawDottedBorder();
			virtual void DrawItem(GLuint texture);
			virtual void InitItem();
			virtual void SetButtonPos(glm::vec2 pos);
			glm::vec2& GetPlaylistItemPos();

			void Click();
			void HidePlaylistItem(b8 val);
			void Visible(b8 val);
			void SetFolderRep(b8 val);
			void SetClickCount(s8 count);
			void SetItemColor(glm::vec3 color);
			b8 IsSelected();
			static void SetPlaylistOffsetY(f32* offsetY);

			b8			 IsVisible() const;
			b8			 IsPlaying() const;
			b8			 IsFolderRep() const;
			b8			 IsPlaylistItemHidden() const;
			u8			 GetClickCount() const;
			glm::vec3	 GetItemColor() const;
			std::wstring GetShortenTextString();

		protected:
			b8			 m_Visible;
			b8			 m_PlaylistItemHidden;
			b8			 m_FolderRep;
			u8			 m_ClickCount;
			s32			 m_ItemID;
			glm::vec3	 m_ItemColor;
			glm::vec2	 m_StartPos;
			glm::vec2	 m_PlaylistItemPos;
			std::string  m_TitleC;
			std::wstring m_Path;
			static f32*	 m_PlaylistOffsetY;
		};

		typedef std::vector<std::pair<s32*, std::wstring*>> SeparatorSubContainer;
		class PlaylistSeparator : public PlaylistItem
		{
		public:
			PlaylistSeparator();
			~PlaylistSeparator();
			PlaylistSeparator(std::wstring& name);

			virtual void InitItem();
			virtual void DrawItem(GLuint texture);

			void			SetSeperatorPath(std::wstring& path);
			void			SeparatorSubFilePushBack(s32* fileIndex, std::wstring& const path);
			b8				IsSeparatorHidden() const;
			b8				IsSelected()		const;
			//void			Visible(b8 val);
			void			HideSeparator(b8 val);
			void			Select(b8 val);
			std::wstring	GetSeparatorPath() const;;
			std::wstring	GetSeparatorName() const;
			SeparatorSubContainer* GetSubFilesContainer();

			f64 SepItemDuration;
		private:
			b8 m_SeparatorSelected;
			b8 m_SeparatorHidden;
			b8 m_Visible;
			s32 m_SepItemCount;
			std::wstring m_Path;
			SeparatorSubContainer m_SubFilesPaths;

		};

		class TextBoxItem : public Button, public TextObject
		{
			friend class TextBox;
			public:
			TextBoxItem(const std::wstring name, glm::vec2 itemSize, glm::vec2 itemPos, 
												 glm::vec2 textSize, glm::vec2 textPos,
						GLuint textTexture, GLuint iconTexture = 0);

			
		private:
			s8			m_Index;
			GLuint		m_IconTexture;
			void UpdateTextBoxItemPos(glm::vec2 pos);

		};

		class TextBox : public TextObject
		{
		public:
			TextBox();
			~TextBox();
			TextBox(MP::UI::Input::ButtonType code, glm::vec2 size, glm::vec2 pos, mdShader* shader);

			void Render();
			void UpdateItemPos();
			void SetBackgroundTexture(GLuint tex);
			void SetSelectTexture(GLuint tex);
			void SetPos(glm::vec2 pos);
			void SetSize(glm::vec2 size);
			void SetColor(glm::vec3 color);
			void SetItemScale(f32 scale);
			void SetItemSize(glm::vec2 itemSize);
			void AddItem(const std::wstring itemName, GLuint iconTexture = 0);

			b8			hasItemFocus(const std::wstring name) const;
			b8			isItemPressed(const std::wstring name) const;
			glm::vec2	GetPos() const;
			glm::vec2	GetSize() const;

		private:
			b8							m_Active;
			u16							m_ItemsCount;
			f32							m_ItemScale;
			GLuint						m_TextBoxBackgroundTexture;
			GLuint						m_TextBoxSelectTexture;
			mdShader*					m_Shader;
			glm::vec2					m_Pos;
			glm::vec2					m_ItemSize;
			glm::vec3					m_Color;
			glm::vec2					m_Size;
			MP::UI::Input::ButtonType	m_Type;
			std::vector<std::shared_ptr<TextBoxItem>>	m_Items;

		};

		class ButtonSlider
		{
		public:

			ButtonSlider();
			ButtonSlider(std::wstring labelName, glm::ivec2 pos, f32* value, f32 step, f32 min = 0, f32 max = 100, glm::vec2 size = glm::vec2(100, 20));
			ButtonSlider(std::wstring labelName, glm::ivec2 pos, s32* value, s32 step, s32 min = 0, s32 max = 100, glm::vec2 size = glm::vec2(100, 20));

			void Init(SDL_Renderer* renderer);
			void Update();
			void ProcessInput();
			b8 IsDefaultPressed();
			void ReloadSliderInfo();
			void ResetButtons();
			void Render();
			void Free();

		private:

			s32 m_RightBackgroundAlpha;
			u8 m_LeftBackgroundAlpha;
			f32* m_ValueF;
			s32* m_Value;
			f32 m_Step;
			f32 m_MinValue;
			f32 m_MaxValue;
			Button* m_LeftButton;
			Button* m_RightButton;
			Button* m_DefaultButton;
			TextObject m_ValueTextObject;
			TextObject m_LabelTextObject;
			TextObject m_DefaultTextObject;
			SDL_Texture* m_LeftTexture;
			SDL_Texture* m_RightTexture;
			SDL_Renderer* m_Renderer;
			SDL_Rect m_LeftSrc;
			SDL_Rect m_LeftDest;
			SDL_Rect m_RightSrc;
			SDL_Rect m_RightDest;
			SDL_Rect m_SliderOutline;
			SDL_Rect m_LeftBackground;
			SDL_Rect m_RightBackground;
			SDL_Rect m_SliderBackground;
			SDL_Rect m_DefaultRect;
			SDL_Color m_ButtonsBackgroundColor;
			SDL_Color m_SliderBackgroundColor;
			SDL_Color m_DefaultButtonColor;
			glm::ivec2 m_ButtonSize;
			Time::Timer m_ClickTimer;
			Time::Timer m_FadeTimerRight;
			Time::Timer m_FadeTimerLeft;
			glm::ivec2 m_SliderPos;
			glm::vec2 m_WindowSize;
			glm::ivec2 m_SliderSize;
			std::wstring m_LabelText;

		};

		class CheckBox : public Button, private TextObject
		{
		public:
			CheckBox();
			CheckBox(std::wstring labelName, glm::vec2 pos, b8* val);

			void Init(SDL_Renderer* renderer);
			void Update();
			void ProcessInput();
			void Render();
			void Free();

		private:
			b8* m_Value;
			SDL_Rect m_CheckBoxOutline;
			SDL_Color m_CheckBoxColor;
			SDL_Renderer* m_Renderer;
		};


		typedef std::vector<std::pair<std::wstring*, std::shared_ptr<PlaylistSeparator>>>	PlaylistSeparatorContainer;
		typedef std::vector<std::pair<s32*, Interface::Button*>>			PlaylistButtonContainer;
		typedef std::vector<std::pair<const std::wstring, Button*>>			InterfaceButtonContainer;
		

		// TODO: It should not be an extern var...
		extern InterfaceButtonContainer m_InterfaceButtonContainer;


		namespace Separator
		{
			PlaylistSeparatorContainer* GetContainer();
			std::shared_ptr<PlaylistSeparator> GetSeparator(std::wstring text);
			std::shared_ptr<PlaylistSeparator> GetSeparatorByID(s32 id);
			s32 GetSize();

#ifdef _DEBUG_
			void PrintSeparatorInfo();
#endif
		}

		namespace PlaylistButton
		{
			PlaylistButtonContainer* GetContainer();
			Interface::Button* GetButton(s32 id);
			s32 GetSize();
		}




	}
}
#endif // !MD_INTERFACE_H