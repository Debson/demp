#ifndef MD_INTERFACE_H
#define MD_INTERFACE_H

#include <glm.hpp>
#include <SDL_ttf.h>
#include <vector>

#include "../utility/md_shader.h"
#include "../ui/music_player_ui_input.h"
#include "../settings/music_player_settings.h"
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
			b8 isPressedRight;
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
			std::string GetShortenTextString();

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
			std::string m_Path;
			static f32*	 m_PlaylistOffsetY;
		};

		typedef std::vector<std::pair<s32*, std::string*>> SeparatorSubContainer;
		class PlaylistSeparator : public PlaylistItem
		{
		public:
			PlaylistSeparator();
			~PlaylistSeparator();
			PlaylistSeparator(std::string& name);

			virtual void InitItem();
			virtual void DrawItem(GLuint texture);

			void			SetSeperatorPath(std::string& path);
			void			SeparatorSubFilePushBack(s32* fileIndex, std::string& path);
			b8				IsSeparatorHidden() const;
			b8				IsSelected()		const;
			//void			Visible(b8 val);
			void			HideSeparator(b8 val);
			void			Select(b8 val);
			std::string	GetSeparatorPath() const;;
			std::string	GetSeparatorName() const;
			SeparatorSubContainer* GetSubFilesContainer();

			f64 SepItemDuration;
		private:
			b8 m_SeparatorSelected;
			b8 m_SeparatorHidden;
			b8 m_Visible;
			s32 m_SepItemCount;
			std::string m_Path;
			SeparatorSubContainer m_SubFilesPaths;

		};

		class TextBoxItem : public Button, public TextObject
		{
			friend class TextBox;
			public:
			TextBoxItem(const std::string name, glm::vec2 itemSize, glm::vec2 itemPos, 
												 glm::vec2 textSize, glm::vec2 textPos,
						GLuint textTexture, GLuint iconTexture = 0);

			
		private:
			s8			m_Index;
			GLuint		m_IconTexture;
			void UpdateTextBoxItemPos(glm::vec2 pos, glm::vec2 offset);

		};

		class TextBox : public TextObject
		{
		public:
			TextBox();
			~TextBox();
			TextBox(MP::UI::Input::ButtonType code, glm::vec2 size, glm::vec2 pos, mdShader* shader);

			void Render();
			void UpdateItemsPos();
			void UpdateItemsPos(glm::vec2 pos);
			void SetBackgroundTexture(GLuint tex);
			void SetSelectTexture(GLuint tex);
			void SetPos(glm::vec2 pos);
			void SetSize(glm::vec2 size);
			void SetColor(glm::vec3 color);
			void SetItemScale(f32 scale);
			void SetItemSize(glm::vec2 itemSize);
			void AddItem(const std::string itemName, GLuint iconTexture = 0);
			void SetItemsOffset(glm::vec2 offset);

			b8			hasItemFocus(const std::string name) const;
			b8			isItemPressed(const std::string name) const;
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
			glm::vec2					m_ItemsOffset;
			MP::UI::Input::ButtonType	m_Type;
			std::vector<std::shared_ptr<TextBoxItem>>	m_Items;

		};

		class PlaylistItemTextBox : public TextBox
		{
		public:
			using TextBox::TextBox;

			void SetSelectedItemID(u32 id);
			u32 GetSelectedItemID();

		private:
			u32 m_SelectedID;
		};

		class ButtonSlider
		{
		public:

			ButtonSlider();
			ButtonSlider(std::string labelName, glm::ivec2 pos, f32* value, f32 step, f32 min = 0, f32 max = 100, glm::vec2 size = glm::vec2(100, 20));
			ButtonSlider(std::string labelName, glm::ivec2 pos, s32* value, s32 step, s32 min = 0, s32 max = 100, glm::vec2 size = glm::vec2(100, 20));
			~ButtonSlider();


			void Init(mdShader* shader);
			void Update();
			void ProcessInput();
			b8 IsDefaultPressed();
			void ReloadSliderInfo();
			void ResetButtons();
			void Render();
			void Free();

		private:

			f32 m_RightBackgroundAlpha;
			f32 m_LeftBackgroundAlpha;
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
			GLuint m_LeftTexture;
			GLuint m_RightTexture;
			mdShader* m_Shader;
			glm::vec4 m_LeftDest;
			glm::vec4 m_RightDest;
			glm::vec4 m_SliderOutline;
			glm::vec4 m_LeftBackground;
			glm::vec4 m_RightBackground;
			glm::vec4 m_SliderBackground;
			glm::vec4 m_DefaultRect;
			glm::vec3 m_ButtonsBackgroundColor;
			glm::vec3 m_SliderBackgroundColor;
			glm::vec3 m_DefaultButtonColor;
			glm::ivec2 m_ButtonSize;
			Time::Timer m_ClickTimer;
			Time::Timer m_FadeTimerRight;
			Time::Timer m_FadeTimerLeft;
			glm::ivec2 m_SliderPos;
			glm::vec2 m_WindowSize;
			glm::ivec2 m_SliderSize;
			std::string m_LabelText;

		};

		class CheckBox : public Button, private TextObject
		{
		public:
			CheckBox();
			CheckBox(std::string labelName, glm::vec2 pos, b8* val);
			~CheckBox();

			void Init(mdShader* renderer);
			void Update();
			void ProcessInput();
			void Render();
			void Free();

		private:
			b8* m_Value;
			glm::vec4 m_CheckBoxOutline;
			glm::vec3 m_CheckBoxColor;
			mdShader* m_Shader;
		};


		typedef std::vector<std::pair<std::string*, std::shared_ptr<PlaylistSeparator>>>	PlaylistSeparatorContainer;
		typedef std::vector<std::pair<s32*, Interface::Button*>>			PlaylistButtonContainer;
		typedef std::vector<std::pair<const std::string, Button*>>			InterfaceButtonContainer;
		

		// TODO: It should not be an extern var...
		extern InterfaceButtonContainer m_InterfaceButtonContainer;


		namespace Separator
		{
			PlaylistSeparatorContainer* GetContainer();
			std::shared_ptr<PlaylistSeparator> GetSeparator(std::string text);
			std::shared_ptr<PlaylistSeparator> GetSeparatorByID(s32 id);
			s32 GetSize();

#ifdef _DEBUG_
			void PrintSeparatorInfo();
#endif
		}
	}
}
#endif // !MD_INTERFACE_H