#include "md_interface.h"

#include <gtc/matrix_transform.hpp>
#include <algorithm>
#include <random>
#include <iomanip>

#include "../app/application_window.h"
#include "../settings/music_player_settings.h"
#include "../playlist/music_player_playlist.h"
#include "../player/music_player_state.h"
#include "../graphics/graphics.h"
#include "../graphics/music_player_graphics_playlist.h"
#include "../ui/music_player_ui.h"
#include "../audio/mp_audio.h"
#include "../utility/utf8_to_utf16.h"
#include "../utility/md_text.h"
#include "../utility/md_load_texture.h"

using namespace mdEngine::MP::UI;
using namespace mdEngine::Graphics;
using namespace mdEngine::MP;

namespace mdEngine
{
	namespace Interface
	{
		static PlaylistSeparatorContainer	m_PlaylistSeparatorContainer;
		static PlaylistButtonContainer		m_PlaylistButtonsContainer;
		InterfaceButtonContainer		m_InterfaceButtonContainer;

	}

	void Interface::CloseInterface()
	{
		for (s32 i = 0; i < m_PlaylistSeparatorContainer.size(); i++)
		{
			delete m_PlaylistSeparatorContainer[i].second;
			m_PlaylistSeparatorContainer[i].second = nullptr;
		}

		for (s32 i = 0; i < m_PlaylistButtonsContainer.size(); i++)
		{
			//delete m_PlaylistButtonsContainer[i].second;
			m_PlaylistButtonsContainer[i].second = nullptr;
		}

		m_PlaylistSeparatorContainer.clear();
		m_PlaylistButtonsContainer.clear();
	}

	void Interface::PrintSeparatorAndItsSubFiles()
	{
		for (auto & i : m_PlaylistSeparatorContainer)
		{
			std::cout << utf16_to_utf8(i.second->GetSeparatorName()) << std::endl;
			for (auto & k : *i.second->GetSubFilesContainer())
			{
				std::cout << "  ";
				std::cout << "ID: " << *k.first  << "   Path: " << utf16_to_utf8(k.second) << std::endl;
			}
		}
	}

	/* *************************************************** */
	Interface::Movable::Movable(glm::vec2 size, glm::vec2 pos) : m_Size(size), m_Pos(pos)
	{
		mdMovableContainer.push_back(this);
	}

	/* *************************************************** */
	Interface::Resizable::Resizable(glm::vec2 size, glm::vec2 pos) : m_Size(size), m_Pos(pos)
	{
		mdResizableContainer.push_back(this);
	}

	/* *************************************************** */
	Interface::Button::Button() { }

	Interface::Button::~Button() { }

	Interface::Button::Button(glm::vec2 size, glm::vec2 pos) : m_ButtonSize(size), m_ButtonPos(pos) { }

	Interface::Button::Button(Input::ButtonType type, glm::vec2 size, glm::vec2 pos) : m_ButtonSize(size), m_ButtonPos(pos)
	{
		mdButtonsContainer.push_back(std::make_pair(type, this));
	}


	void Interface::Button::SetButtonPos(glm::vec2 pos)
	{
		m_ButtonPos = pos;
	}

	void Interface::Button::SetButtonPosY(f32 posY)
	{
		m_ButtonPos = glm::vec2(m_ButtonPos.x, posY);
	}

	void Interface::Button::SetButtonSize(glm::vec2 size)
	{
		m_ButtonSize = size;
	}

	void Interface::Button::ResetState()
	{
		isPressed			= false;
		isReleased			= false;
		isDown				= false;
		topHasFocus			= false;
		bottomHasFocus		= false;
		hasFocus			= false;
		hasFocusTillRelease = false;
		wasDown				= false;
	}

	glm::vec2& Interface::Button::GetButtonPos()
	{
		return m_ButtonPos;
	}

	glm::vec2& Interface::Button::GetButtonSize()
	{
		return m_ButtonSize;
	}

	glm::vec2& Interface::Button::GetInButtonMousePos()
	{
		return m_MousePos;
	}

	


	/* *************************************************** */

	Interface::PlaylistItem::PlaylistItem()
	{
		m_FolderRep = false;
		m_ClickCount = 0;
	}

	Interface::PlaylistItem::~PlaylistItem() { }

	void Interface::PlaylistItem::InitItem(s32* id)
	{
		m_ItemColor = Color::White;
		m_ButtonSize = Data::_PLAYLIST_ITEM_SIZE;
		m_ClickCount = 0;
		m_PlaylistItemHidden = false;
		m_ItemColor = Color::White;
		
		m_ItemID = id;

		// Button position is predefined in playlist items rendering algorithm anyway, but leave it for now
		m_StartPos = Data::_PLAYLIST_ITEMS_SURFACE_POS;
		m_ButtonPos = glm::vec2(m_StartPos.x, m_StartPos.y + *m_ItemID * (m_ButtonSize.y));
		m_TextPos = m_ButtonPos;

		m_Font = MP::Data::_MUSIC_PLAYER_FONT;
		m_TextScale = 1.f;
		SetTextColor(Color::White);

		m_TextString = Audio::Object::GetAudioObject(*m_ItemID)->GetTitle();

		assert(Audio::Object::GetAudioObject(*m_ItemID) != nullptr);

		m_TitleC = utf16_to_utf8(m_TextString);
		TTF_SizeUTF8(m_Font, m_TitleC.c_str(), &m_TextSize.x, &m_TextSize.y);

		m_PlaylistButtonsContainer[*id] = std::make_pair(id, this);
	}

	void Interface::PlaylistItem::DrawDottedBorder() const
	{
		glm::mat4 model;
		Shader::shaderBorder->use();
		Shader::shaderBorder->setVec3("color", Color::Grey);
		f32 dotXOffset = 0.01;
		f32 dotYOffset = 0.1;
		Shader::shaderBorder->setFloat("xOffset", dotXOffset);
		Shader::shaderBorder->setFloat("yOffset", dotYOffset);
		model = glm::translate(model, glm::vec3(m_ButtonPos, 0.9));
		model = glm::scale(model, glm::vec3(m_ButtonSize, 1.f));
		Shader::shaderBorder->setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, 0);
		Shader::DrawDot();
	}

	void Interface::PlaylistItem::DrawItem(GLuint texture)
	{
		glm::mat4 model;
		glm::vec3 originalColor = m_ItemColor;
		glm::vec3 halvesColor = Color::DarkGrey;

		if (Graphics::MP::GetPlaylistObject()->GetPlayingID() == *m_ItemID &&
			State::CheckState(State::CurrentlyPlayingDeleted) == false)
		{
			m_ItemColor *= Color::Red;
		}

		Shader::shaderDefault->use();
		halvesColor *= m_ItemColor;
		if (topHasFocus == true)
		{
			model = glm::translate(model, glm::vec3(m_ButtonPos, 0.5f));
			model = glm::scale(model, glm::vec3(m_ButtonSize.x, m_ButtonSize.y / 2.f, 1.0f));
			Shader::shaderDefault->setMat4("model", model);
			Shader::shaderDefault->setVec3("color", halvesColor);
			glBindTexture(GL_TEXTURE_2D, texture);
			Shader::Draw(Shader::shaderDefault);
		}


		if (bottomHasFocus == true)
		{
			model = glm::translate(model, glm::vec3(m_ButtonPos.x, m_ButtonPos.y + m_ButtonSize.y / 2.f, 0.5f));
			model = glm::scale(model, glm::vec3(m_ButtonSize.x, m_ButtonSize.y / 2.f, 1.0f));
			Shader::shaderDefault->setMat4("model", model);
			Shader::shaderDefault->setVec3("color", halvesColor);
			glBindTexture(GL_TEXTURE_2D, texture);
			Shader::Draw(Shader::shaderDefault);
		}


		model = glm::mat4();
		model = glm::translate(model, glm::vec3(m_ButtonPos, 0.5f));
		model = glm::scale(model, glm::vec3(m_ButtonSize, 1.0f));
		Shader::shaderDefault->setMat4("model", model);
		Shader::shaderDefault->setVec3("color", m_ItemColor);
		glBindTexture(GL_TEXTURE_2D, texture);
		Shader::Draw(Shader::shaderDefault);

		// Draw String
		s32 offsetY = (m_ButtonSize.y - m_TextSize.y) / 2;
		SetTextOffset(glm::vec2(5.f, offsetY));
		DrawString();
		m_ItemColor = originalColor;
	}

	void Interface::PlaylistItem::SetButtonPos(glm::vec2 pos)
	{
		m_ButtonPos = pos;
		m_TextPos = pos;
	}

	void Interface::PlaylistItem::Click()
	{
		m_ClickCount++;
	}

	void Interface::PlaylistItem::HidePlaylistItem(b8 val)
	{
		m_PlaylistItemHidden = val;
	}

	void Interface::PlaylistItem::Visible(b8 val)
	{
		m_Visible = val;
	}

	void Interface::PlaylistItem::SetAsFolderRep()
	{
		m_FolderRep = true;
	}

	void Interface::PlaylistItem::TakeFolderRep()
	{
		m_FolderRep = false;
	}

	void Interface::PlaylistItem::SetClickCount(s8 count)
	{
		m_ClickCount = count;
	}

	void Interface::PlaylistItem::SetItemColor(glm::vec3 color)
	{
		m_ItemColor = color;
	}

	b8 Interface::PlaylistItem::IsVisible() const
	{
		return m_Visible;
	}

	b8 Interface::PlaylistItem::IsPlaying() const
	{
		return MP::Playlist::RamLoadedMusic.m_ID == *m_ItemID;
	}

	b8 Interface::PlaylistItem::IsFolderRep() const
	{
		return m_FolderRep;
	}

	b8 Interface::PlaylistItem::IsPlaylistItemHidden() const
	{
		return m_PlaylistItemHidden;
	}

	u8 Interface::PlaylistItem::GetClickCount() const
	{
		return m_ClickCount;
	}

	glm::vec3 Interface::PlaylistItem::GetItemColor() const
	{
		return m_ItemColor;
	}

	std::wstring Interface::PlaylistItem::GetShortenTextString()
	{
		s16 len = wcslen(m_TextString.c_str());
		f32 textSize = m_TextSize.x * m_TextScale;

		if (textSize > this->m_ButtonSize.x)
		{
			float charSize = textSize / (float)len;
			u16 i = 0;
			u16 pos = 0;
			while (i * charSize < this->m_ButtonSize.x)
				i++;

			m_TextString = m_TextString.substr(0, i - 4);
			m_TextString += L"...";

			len = m_TextString.length();
			m_TitleC.resize(len + 1);
			m_TitleC = utf16_to_utf8(m_TextString);
			TTF_SizeText(m_Font, m_TitleC.c_str(), &m_TextSize.x, &m_TextSize.y);;
		}

		return m_TextString;
	}

	s32 Interface::PlaylistItem::OffsetIndex = 0;

	/* *************************************************** */
	Interface::PlaylistSeparator::PlaylistSeparator() { }

	Interface::PlaylistSeparator::PlaylistSeparator(std::wstring name)
	{ 
		m_Path = name;
		m_TextString = Audio::Info::GetFolder(name);
		m_Visible = false;
		m_ButtonPos = glm::vec2(POS_INVALID);
		m_TextPos = glm::vec2(POS_INVALID);
		m_SepItemCount = 0;
		SepItemDuration = 0;
		m_ItemColor = Color::Blue;
		m_ButtonSize = Data::_PLAYLIST_SEPARATOR_SIZE;
	}

	void Interface::PlaylistSeparator::InitItem(s32 posOfFirstFile)
	{
		m_Font = MP::Data::_MUSIC_PLAYER_FONT;
		m_TextScale = 1.f;
		m_ItemColor = Color::Pink;
		SetTextColor(Color::Grey);

		m_SeparatorHidden = false;
		m_SeparatorSelected = false;

		u16 len = m_TextString.length();
		m_TitleC.resize(len + 1);
		m_TitleC = utf16_to_utf8(m_TextString);

		TTF_SizeUTF8(m_Font, m_TitleC.c_str(), &m_TextSize.x, &m_TextSize.y);

		m_PlaylistSeparatorContainer.push_back(std::make_pair(m_Path, this));
	}

	void Interface::PlaylistSeparator::DrawItem(GLuint texture)
	{
		Shader::shaderDefault->use();
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(m_ButtonPos, 0.5f));
		model = glm::scale(model, glm::vec3(m_ButtonSize, 1.0f));
		Shader::shaderDefault->setMat4("model", model);
		Shader::shaderDefault->setVec3("color", m_ItemColor);
		glBindTexture(GL_TEXTURE_2D, texture);
		Shader::Draw(Shader::shaderDefault);

		// Draw String
		s32 offsetY = (m_ButtonSize.y - m_TextSize.y) / 2;
		SetTextOffset(glm::vec2(5.f, offsetY));
		DrawString();
	}

	void Interface::PlaylistSeparator::SetSeperatorPath(std::wstring path)
	{
		m_Path = path;
	}

	void Interface::PlaylistSeparator::SeparatorSubFilePushBack(s32* fileIndex, std::wstring path)
	{
		m_SubFilesPaths.push_back(std::make_pair(fileIndex, path));
		m_SepItemCount++;
	}

	void Interface::PlaylistSeparator::SeparatorSubFileInsert(s32* fileIndex, const std::wstring path, s32 pos)
	{
		assert(pos < m_SubFilesPaths.size());
		m_SubFilesPaths.insert(m_SubFilesPaths.begin() + pos, std::make_pair(fileIndex, path));
		m_SepItemCount++;
	}

	void Interface::PlaylistSeparator::SeparatorSubFileErased()
	{
		m_SepItemCount--;
	}
	
	b8 Interface::PlaylistSeparator::IsSeparatorHidden() const
	{
		return m_SeparatorHidden;
	}

	b8 Interface::PlaylistSeparator::IsSelected() const
	{
		return m_SeparatorSelected;
	}

	void Interface::PlaylistSeparator::HideSeparator(b8 val)
	{
		auto audioCon = Audio::Object::GetAudioObjectContainer();
		if (val == true)
		{
			for (auto i : m_SubFilesPaths)
			{
				audioCon[*i.first]->HidePlaylistItem(true);
			}
		}
		else
		{
			for (auto i : m_SubFilesPaths)
			{
				audioCon[*i.first]->HidePlaylistItem(false);
			}
		}
		m_SeparatorHidden = val;
	}

	void Interface::PlaylistSeparator::Select(b8 val)
	{
		auto playlist = Graphics::MP::GetPlaylistObject();
		assert(playlist != NULL);

		if (val == true)
		{
			for (auto i : m_SubFilesPaths)
			{
				playlist->multipleSelect.push_back(i.first);
			}
		}
		else
		{
			playlist->multipleSelect.clear();
		}

		m_SeparatorSelected = val;
	}

	std::wstring Interface::PlaylistSeparator::GetSeparatorPath() const
	{
		return m_Path;
	}

	std::wstring Interface::PlaylistSeparator::GetSeparatorName() const
	{
		return m_TextString;
	}


	Interface::SeparatorSubContainer* Interface::PlaylistSeparator::GetSubFilesContainer()
	{
		return &m_SubFilesPaths;
	}

	/* *************************************************** */
	Interface::TextBoxItem::TextBoxItem(const std::wstring name, glm::vec2 itemSize, glm::vec2 itemPos,
																 glm::vec2 textSize, glm::vec2 textPos,
										GLuint tex)
	{
		m_ButtonPos = itemPos;
		m_ButtonSize = itemSize;
		m_TextPos = textPos;
		m_TextSize = textSize;
		m_TextTexture = tex;
		m_InterfaceButtonContainer.push_back(make_pair(name, this));
	}

	void Interface::TextBoxItem::UpdateTextBoxItemPos(glm::vec2 pos)
	{
		m_ButtonPos = glm::vec2(pos.x, pos.y + m_Index * Data::_TEXT_BOX_ITEM_HEIGHT);
		m_TextPos = glm::vec2(m_ButtonPos.x + 40.f, m_ButtonPos.y);
	}

	/* *************************************************** */
	Interface::TextBox::TextBox() { }

	Interface::TextBox::TextBox(MP::UI::Input::ButtonType code, glm::vec2 size, glm::vec2 pos, mdShader* shader) : 
								m_Shader(shader)
	{ 
		m_Pos = pos;
		m_Size = size;
		m_Type = code;
		m_ItemsCount = 0;
		new Button(code, size, pos);
	}

	void Interface::TextBox::Render()
	{
		assert(m_Shader != NULL);

		glm::mat4 model;
		model = glm::translate(model, glm::vec3(m_Pos, 0.7));
		model = glm::scale(model, glm::vec3(m_Size, 1.0));
		m_Shader->setBool("plain", true);
		m_Shader->setVec3("color", m_Color);
		m_Shader->setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, 0);
		Graphics::Shader::Draw(m_Shader);
		m_Shader->setBool("plain", false);
		m_Shader->setVec3("color", 1.f, 1.f, 1.f);

		for (s32 i = 0; i < m_Items.size(); i++)
		{
			TextBoxItem* item = m_Items[i];
			//render item rect
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(item->m_ButtonPos, 0.7));
			model = glm::scale(model, glm::vec3(item->m_ButtonSize, 1.0));;
			m_Shader->setMat4("model", model);
			glBindTexture(GL_TEXTURE_2D, 0);
			Graphics::Shader::Draw(m_Shader);

			//render text on the item
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(item->m_TextPos, 0.8));
			model = glm::scale(model, glm::vec3(item->m_TextSize, 1.0));;
			m_Shader->setMat4("model", model);
			glBindTexture(GL_TEXTURE_2D, item->m_TextTexture);
			Graphics::Shader::Draw(m_Shader);
		}

	}

	void Interface::TextBox::UpdateItemPos()
	{
		for (u16 i = 0; i < m_Items.size(); i++)
		{
			m_Items[i]->UpdateTextBoxItemPos(m_Pos);
		}
	}

	void Interface::TextBox::SetPos(glm::vec2 pos)
	{
		m_Pos = pos;

		auto item = std::find_if(mdButtonsContainer.begin(), mdButtonsContainer.end(),
			[&](std::pair<Input::ButtonType, Interface::Button*> const& ref) { return ref.first == m_Type; });

		if (item != mdButtonsContainer.end())
		{
			item->second->SetButtonPos(m_Pos);
		}

	}

	void Interface::TextBox::SetSize(glm::vec2 size)
	{
		m_Size = size;

		auto item = std::find_if(mdButtonsContainer.begin(), mdButtonsContainer.end(),
			[&](std::pair<Input::ButtonType, Interface::Button*> const& ref) { return ref.first == m_Type; });

		if (item != mdButtonsContainer.end())
		{
			item->second->SetButtonSize(m_Size);
		}
	}

	void Interface::TextBox::SetColor(glm::vec3 color)
	{
		m_Color = color;
	}

	void Interface::TextBox::SetItemScale(f32 scale)
	{
		m_ItemScale = scale;
	}

	void Interface::TextBox::SetItemSize(glm::vec2 itemSize)
	{
		m_ItemSize = itemSize;
	}

	void Interface::TextBox::AddItem(const std::wstring itemName)
	{
		SDL_Color color = { 0, 0, 0 };
		GLuint tex = Text::LoadText(Data::_MUSIC_PLAYER_FONT, itemName, color);

		glm::ivec2 textSize;
		std::string name = utf16_to_utf8(itemName);
		TTF_SizeText(Data::_MUSIC_PLAYER_FONT, name.c_str(), &textSize.x, &textSize.y);

		TextBoxItem* item = new TextBoxItem(itemName, glm::vec2(m_Size.x, Data::_TEXT_BOX_ITEM_HEIGHT), glm::vec2(m_Pos.x, m_Pos.y + m_ItemsCount * Data::_TEXT_BOX_ITEM_HEIGHT),
													   glm::vec2(textSize) * m_ItemScale, glm::vec2(m_Pos.x + 40.f, m_Pos.y + m_ItemsCount * textSize.y * m_ItemScale),
											 tex);
		item->m_Index = m_ItemsCount;
		m_Items.push_back(item);
		m_ItemsCount++;
	}

	glm::vec2 Interface::TextBox::GetPos() const
	{
		return m_Pos;
	}

	glm::vec2 Interface::TextBox::GetSize() const
	{
		return m_Size;
	}

	b8 Interface::TextBox::hasItemFocus(const std::wstring name) const
	{
		auto item = std::find_if(m_InterfaceButtonContainer.begin(), m_InterfaceButtonContainer.end(),
			[&](std::pair<const std::wstring, Button*> const & ref) { return ref.first.compare(name) == 0; });

		return item == m_InterfaceButtonContainer.end() ? false : item->second->hasFocus;
	}

	b8 Interface::TextBox::isItemPressed(const std::wstring name) const
	{
		auto item = std::find_if(m_InterfaceButtonContainer.begin(), m_InterfaceButtonContainer.end(),
			[&](std::pair<const std::wstring, Button*> const & ref) { return ref.first.compare(name) == 0; });

		return item == m_InterfaceButtonContainer.end() ? false : item->second->isPressed;
	}

	
	Interface::ButtonSlider::ButtonSlider() { }

	Interface::ButtonSlider::ButtonSlider(std::wstring labelName, glm::ivec2 pos, f32* value, f32 step, f32 min, f32 max, glm::vec2 size) :	m_SliderSize(size),
																																			m_ValueF(value),
																																			m_Step(step),
																																			m_MinValue(min),
																																			m_MaxValue(max)
	{
		m_Value = NULL;
		m_LabelText = labelName;
		m_WindowSize = Data::_OPTIONS_WINDOW_SIZE;
		m_SliderPos = pos;
		m_SliderSize = size;
		m_ButtonSize = { 15, 15 };

	}

	Interface::ButtonSlider::ButtonSlider(std::wstring labelName, glm::ivec2 pos, s32* value, s32 step, s32 min, s32 max, glm::vec2 size) :	m_SliderSize(size),
																																			m_Value(value),
																																			m_Step(step),
																																			m_MinValue(min),
																																			m_MaxValue(max)
	{
		m_ValueF = NULL;
		m_LabelText = labelName;
		m_WindowSize = Data::_OPTIONS_WINDOW_SIZE;
		m_SliderPos = pos;
		m_SliderSize = size;

	}

	void Interface::ButtonSlider::Init(SDL_Renderer* renderer)
	{
		m_Renderer = renderer;
		m_LeftTexture = mdLoadTextureSDL(m_Renderer, "E:\\SDL Projects\\demp\\demp\\assets\\switch.png");
		m_RightTexture = m_LeftTexture;

		if (m_LeftTexture == NULL)
		{
			MD_SDL_ERROR("SDL Texture");
		}

		glm::ivec2 buttonTexSize(12, 12);
		s32 offsetY = 5;

		s32 labelTextXOffset = 3;
		m_LabelTextObject = TextObject(Data::_MUSIC_PLAYER_FONT, Color::Black);
		m_LabelTextObject.SetTextString(m_LabelText);
		m_LabelTextObject.SetTextPos(glm::vec2(m_SliderPos.x - labelTextXOffset, m_SliderPos.y));
		// Init sdl text
		m_LabelTextObject.InitTextTextureSDL(m_Renderer);
		

		s32 outlineSizeOffsetX = 4;
		m_SliderOutline = { m_SliderPos.x - outlineSizeOffsetX, m_SliderPos.y + (s32)m_LabelTextObject.GetTextSize().y + offsetY - (m_SliderSize.y - buttonTexSize.y) / 2,
							m_SliderSize.x + 2 * outlineSizeOffsetX , m_SliderSize.y };

		m_ValueTextObject = TextObject(Data::_MUSIC_PLAYER_FONT, Color::Black);
		if (m_Value == NULL)
		{
			std::wstringstream ss;
			std::wstring val;
			ss << std::setprecision(3) << *m_ValueF;
			ss >> val;
			m_ValueTextObject.SetTextString(val);
		}
		else
		{
			m_ValueTextObject.SetTextString(std::to_wstring(*m_Value));
		}


		m_ValueTextObject.InitTextTextureSDL(m_Renderer);


		s32 offsetX = (m_SliderSize.x - 2 * buttonTexSize.x - m_ValueTextObject.GetTextSize().x) / 2;

		m_ValueTextObject.SetTextPos(glm::vec2(m_SliderPos.x + buttonTexSize.x + offsetX,
											   m_SliderPos.y + m_LabelTextObject.GetTextSize().y + offsetY - (m_SliderSize.y - m_ValueTextObject.GetTextSize().y) / 2));

		s32 texW, texH;
		SDL_QueryTexture(m_LeftTexture, NULL, NULL, &texW, &texH);	


		m_LeftSrc	= { 0, 0, texW, texH };
		m_LeftDest	= { m_SliderPos.x, m_SliderPos.y + (s32)m_LabelTextObject.GetTextSize().y + offsetY,
						buttonTexSize.x, buttonTexSize.y };
		m_RightSrc	= { 0, 0, texW, texH };
		m_RightDest	= { m_SliderPos.x + buttonTexSize.x + (s32)m_ValueTextObject.GetTextSize().x + 2 * offsetX,
						m_SliderPos.y + (s32)m_LabelTextObject.GetTextSize().y + offsetY, 
						buttonTexSize.x, buttonTexSize.y};


		m_ButtonSize = glm::vec2(m_SliderSize.y, m_SliderSize.y);
		offsetX = (m_SliderSize.x - 2 * m_ButtonSize.x - m_ValueTextObject.GetTextSize().x) / 2;
		m_LeftButton	= new Button();
		m_RightButton = new Button();
		m_LeftButton->SetButtonPos(glm::vec2(m_SliderPos.x - outlineSizeOffsetX,
									   m_SliderPos.y + m_LabelTextObject.GetTextSize().y + offsetY - (m_SliderSize.y - buttonTexSize.y) / 2));
		m_LeftButton->SetButtonSize(m_ButtonSize);
		m_RightButton->SetButtonPos(glm::vec2(m_SliderPos.x + m_ButtonSize.x + m_ValueTextObject.GetTextSize().x + 2 * offsetX + outlineSizeOffsetX,
										m_SliderPos.y + m_LabelTextObject.GetTextSize().y + offsetY - (m_SliderSize.y - buttonTexSize.y) / 2));
		m_RightButton->SetButtonSize(m_ButtonSize);
		m_ClickTimer = Time::Timer(650);

		m_RightBackground = { (s32)m_RightButton->GetButtonPos().x,  (s32)m_RightButton->GetButtonPos().y,
							  (s32)m_RightButton->GetButtonSize().x, (s32)m_RightButton->GetButtonSize().y };


		m_LeftBackground = { (s32)m_LeftButton->GetButtonPos().x,  (s32)m_LeftButton->GetButtonPos().y,
							  (s32)m_LeftButton->GetButtonSize().x, (s32)m_LeftButton->GetButtonSize().y };

		m_RightBackgroundAlpha	= 0;
		m_LeftBackgroundAlpha	= 0;

		m_ButtonsBackgroundColor = SDLColor::Orange;

		s32 fadeTimeValue	= 200;
		m_FadeTimerRight	= Time::Timer(fadeTimeValue);
		m_FadeTimerLeft		= Time::Timer(fadeTimeValue);

		m_SliderBackground = m_SliderOutline;
		m_SliderBackgroundColor = SDLColor::Grey;



		m_DefaultRect = { (s32)m_SliderPos.x + (s32)m_SliderSize.x + 10, m_SliderOutline.y,
						  70, (s32)m_SliderSize.y };

		m_DefaultButton = new Button();
		m_DefaultButton->SetButtonPos(glm::vec2(m_DefaultRect.x, m_DefaultRect.y));
		m_DefaultButton->SetButtonSize(glm::vec2(m_DefaultRect.w, m_DefaultRect.h));

		m_DefaultTextObject = TextObject(Data::_MUSIC_PLAYER_FONT, Color::Black);

		m_DefaultTextObject.SetTextString(L"Default");
		// Init sdl text
		m_DefaultTextObject.InitTextTextureSDL(m_Renderer);

		offsetX = (m_DefaultRect.w - m_DefaultTextObject.GetTextSize().x) / 2;
		offsetY = (m_DefaultRect.h - m_DefaultTextObject.GetTextSize().y) / 2;
		m_DefaultTextObject.SetTextPos(glm::vec2(m_DefaultRect.x + offsetX,
												 m_SliderOutline.y + offsetY));


	}

	void Interface::ButtonSlider::Update()
	{
		u8 startAlpha = 100;
		if (m_RightButton->hasFocus == true)
		{
			m_FadeTimerRight.Start();
			m_RightBackgroundAlpha = startAlpha;
		}
		else if(m_FadeTimerRight.started == true)
		{
			m_RightBackgroundAlpha = startAlpha * (1.f - m_FadeTimerRight.GetProgress());
		}
		else if (m_FadeTimerRight.finished == true)
		{
			m_RightBackgroundAlpha = 0;
		}


		if (m_RightButton->isDown == true)
		{
			if (m_ClickTimer.finished == true && m_RightButton->isPressed == false)
			{
				if (m_Value == NULL)
				{
					*m_ValueF += m_Step;
					if (*m_ValueF > m_MaxValue)
						*m_ValueF = m_MaxValue;
				}
				else
				{
					*m_Value += m_Step;
					if (*m_Value > m_MaxValue)
						*m_Value = m_MaxValue;
				}

				ReloadSliderInfo();
			}

			if (m_RightButton->isPressed == true)
			{
				if (m_Value == NULL)
				{
					*m_ValueF += m_Step;
					if (*m_ValueF > m_MaxValue)
						*m_ValueF = m_MaxValue;
				}
				else
				{
					*m_Value += m_Step;
					if (*m_Value > m_MaxValue)
						*m_Value = m_MaxValue;
				}

				ReloadSliderInfo();
				m_ClickTimer.Start();
			}
		}
		else
		{
			m_ClickTimer.Stop();
		}

		if (m_LeftButton->hasFocus == true)
		{
			m_FadeTimerLeft.Start();
			m_LeftBackgroundAlpha = startAlpha;
		}
		else if (m_FadeTimerLeft.started == true)
		{
			m_LeftBackgroundAlpha = startAlpha * (1.f - m_FadeTimerLeft.GetProgress());
		}
		else if (m_FadeTimerLeft.finished == true)
		{
			m_LeftBackgroundAlpha = 0;
		}


		if (m_LeftButton->isDown == true)
		{
			if (m_ClickTimer.finished == true && m_LeftButton->isPressed == false)
			{
				if (m_Value == NULL)
				{
					*m_ValueF -= m_Step;
					if (*m_ValueF < m_MinValue)
						*m_ValueF = m_MinValue;
				}
				else
				{
					*m_Value -= m_Step;
					if (*m_Value < m_MinValue)
						*m_Value = m_MinValue;
				}

				ReloadSliderInfo();
			}

			if (m_LeftButton->isPressed == true)
			{
				if (m_Value == NULL)
				{
					*m_ValueF -= m_Step;
					if (*m_ValueF < m_MinValue)
						*m_ValueF = m_MinValue;
				}
				else
				{
					*m_Value -= m_Step;
					if (*m_Value < m_MinValue)
						*m_Value = m_MinValue;
				}

				ReloadSliderInfo();
				m_ClickTimer.Start();
			}
		}
		else
		{
			m_ClickTimer.Stop();
		}

		if (m_DefaultButton->hasFocus == true)
		{
			m_DefaultButtonColor = SDLColor::Silver;
		}
		else
		{
			m_DefaultButtonColor = SDLColor::DarkGrey;
		}

		
		m_FadeTimerRight.Update();
		m_FadeTimerLeft.Update();
		m_ClickTimer.Update();
	}

	void Interface::ButtonSlider::ProcessInput()
	{
		App::ProcessButton(m_LeftButton);
		App::ProcessButton(m_RightButton);
		App::ProcessButton(m_DefaultButton);
	}

	b8 Interface::ButtonSlider::IsDefaultPressed()
	{
		return m_DefaultButton->isPressed;
	}

	void Interface::ButtonSlider::ResetButtons()
	{
		m_LeftButton->ResetState();
		m_RightButton->ResetState();
		m_DefaultButton->ResetState();
	}

	void Interface::ButtonSlider::Render()
	{
		SDL_SetRenderDrawColor(m_Renderer, m_SliderBackgroundColor.r, m_SliderBackgroundColor.g, m_SliderBackgroundColor.b, 0xFF);
		SDL_RenderFillRect(m_Renderer, &m_SliderBackground);

		SDL_SetRenderDrawBlendMode(m_Renderer, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(m_Renderer, m_ButtonsBackgroundColor.r, m_ButtonsBackgroundColor.g, m_ButtonsBackgroundColor.b, m_RightBackgroundAlpha);
		SDL_RenderFillRect(m_Renderer, &m_RightBackground);

		SDL_SetRenderDrawColor(m_Renderer, m_ButtonsBackgroundColor.r, m_ButtonsBackgroundColor.g, m_ButtonsBackgroundColor.b, m_LeftBackgroundAlpha);
		SDL_RenderFillRect(m_Renderer, &m_LeftBackground);
		SDL_SetRenderDrawBlendMode(m_Renderer, SDL_BLENDMODE_NONE);


		SDL_SetRenderDrawColor(m_Renderer, m_DefaultButtonColor.r, m_DefaultButtonColor.g, m_DefaultButtonColor.b, 0xFF);
		SDL_RenderFillRect(m_Renderer, &m_DefaultRect);
		m_DefaultTextObject.DrawStringSDL(m_Renderer);


		SDL_SetRenderDrawColor(m_Renderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderDrawRect(m_Renderer, &m_SliderOutline);

		m_LabelTextObject.DrawStringSDL(m_Renderer);

		SDL_RenderCopyEx(m_Renderer, m_LeftTexture, NULL, &m_LeftDest, 0, NULL, SDL_FLIP_HORIZONTAL);

		m_ValueTextObject.DrawStringSDL(m_Renderer);

		SDL_RenderCopy(m_Renderer, m_RightTexture, &m_RightSrc, &m_RightDest);

	}

	void Interface::ButtonSlider::Free()
	{
		SDL_DestroyTexture(m_LeftTexture);
		SDL_DestroyTexture(m_RightTexture);
		m_LeftTexture	= NULL;
		m_RightTexture	= NULL;

		m_RightButton	= NULL;
		m_LeftButton	= NULL;
		m_DefaultButton = NULL;
	}

	void Interface::ButtonSlider::ReloadSliderInfo()
	{
		if (m_Value == NULL)
		{
			std::wstringstream ss;
			std::wstring val;
			ss << std::setprecision(3) << *m_ValueF;
			ss >> val;
			m_ValueTextObject.SetTextString(val);
		}
		else
		{
			m_ValueTextObject.SetTextString(std::to_wstring(*m_Value));
		}
		s32 textSizeBefore = m_ValueTextObject.GetTextSize().x;
		m_ValueTextObject.ReloadTextTextureSDL();
		s32 textSizeAfter = m_ValueTextObject.GetTextSize().x;;
		s32 diff = (textSizeAfter - textSizeBefore) / 2.f;

		m_ValueTextObject.SetTextPos(glm::vec2(m_ValueTextObject.GetTextPos().x - diff, 
											   m_ValueTextObject.GetTextPos().y));

		//m_Right->SetButtonPos(glm::vec2(m_Right->GetButtonPos().x + diff, m_Right->GetButtonPos().y));
		//m_RightDest = { m_RightDest.x + diff, m_RightDest.y, m_RightDest.w, m_RightDest.h };

		//m_Left->SetButtonPos(glm::vec2(m_Left->GetButtonPos().x - diff, m_Left->GetButtonPos().y));
		//m_LeftDest = { m_LeftDest.x - diff, m_LeftDest.y, m_LeftDest.w, m_LeftDest.h };
	}


	Interface::CheckBox::CheckBox() { }

	Interface::CheckBox::CheckBox(std::wstring labelName, glm::vec2 pos, b8* val) : m_Value(val)
	{
		s32 textOffsetX = 5;
		m_TextString = labelName;
		m_TextPos = glm::vec2(pos.x - textOffsetX, pos.y);
		m_Font = Data::_MUSIC_PLAYER_FONT;
		m_TextColorSDL = SDLColor::Black;
	}

	void Interface::CheckBox::Init(SDL_Renderer* renderer)
	{
		m_Renderer = renderer;
		InitTextTextureSDL(m_Renderer);

		s32 offsetY = 5;
		m_ButtonPos = glm::vec2(m_TextPos.x, m_TextPos.y + m_TextSize.y + offsetY);
		m_ButtonSize = glm::vec2(15, 15);

		m_CheckBoxOutline = { (s32)m_ButtonPos.x, (s32)m_ButtonPos.y,
							  (s32)m_ButtonSize.x, (s32)m_ButtonSize.y };

		m_CheckBoxColor = SDLColor::Orange;

	}

	void Interface::CheckBox::Update()
	{
		if (this->isPressed == true)
		{
			*m_Value = !(*m_Value);
		}
	}

	void Interface::CheckBox::ProcessInput()
	{
		App::ProcessButton(this);
	}

	void Interface::CheckBox::Render()
	{
		DrawStringSDL(m_Renderer);

		*m_Value == true ? SDL_SetRenderDrawColor(m_Renderer, SDLColor::Orange.r, SDLColor::Orange.g, SDLColor::Orange.b, 0xFF) :
						   SDL_SetRenderDrawColor(m_Renderer, SDLColor::Azure.r, SDLColor::Azure.g, SDLColor::Azure.b, 0xFF);

		SDL_RenderFillRect(m_Renderer, &m_CheckBoxOutline);


		SDL_SetRenderDrawColor(m_Renderer, SDLColor::Black.r, SDLColor::Black.g, SDLColor::Black.b, 0xFF);
		SDL_RenderDrawRect(m_Renderer, &m_CheckBoxOutline);
	}

	void Interface::CheckBox::Free()
	{
		m_Renderer = NULL;
		m_Value = NULL;
	}

	
	Interface::PlaylistSeparatorContainer*  Interface::Separator::GetContainer()
	{
		return &m_PlaylistSeparatorContainer;
	}

	Interface::PlaylistSeparator* Interface::Separator::GetSeparator(std::wstring text)
	{
		if (m_PlaylistSeparatorContainer.empty() == true)
			return nullptr;

		auto it = find_if(m_PlaylistSeparatorContainer.begin(), m_PlaylistSeparatorContainer.end(),
			[&](std::pair<std::wstring, Interface::PlaylistSeparator*> const & ref) { return ref.first.compare(text) == 0; });

		if (it == m_PlaylistSeparatorContainer.end())
			return nullptr;

		return it->second;
	}

	Interface::PlaylistSeparator* Interface::Separator::GetSeparatorByID(s32 id)
	{
		if (m_PlaylistSeparatorContainer.empty() == true)
			return nullptr;

		for (auto & i : m_PlaylistSeparatorContainer)
		{
			auto subCon = i.second->GetSubFilesContainer();
			if (*subCon->at(0).first <= id &&
				*subCon->back().first >= id)
			{
				return i.second;
			}
		}

		return nullptr;
	}

	s32 Interface::Separator::GetSize()
	{
		return m_PlaylistSeparatorContainer.size();
	}

#ifdef _DEBUG_
	void Interface::Separator::PrintSeparatorInfo()
	{
		s32 maxNameLen = 40;
		for (auto i : m_PlaylistSeparatorContainer)
		{
			std::cout << "Name: ";
			std::cout << utf16_to_utf8(i.second->GetSeparatorName());
			for(s32 k = utf16_to_utf8(i.second->GetSeparatorName()).length(); k < maxNameLen; k++)
				std::cout << " ";
			std::cout << " || ";
			std::cout << "Visible: ";
			std::cout << i.second->IsVisible();
			std::cout << " || ";;
			std::cout << "Hidden: ";
			std::cout << i.second->IsSeparatorHidden();
			std::cout << " || ";
			std::cout << "Sub items: ";
			std::cout << i.second->GetSubFilesContainer()->size();
			std::cout << " || ";
			std::cout << "Pos X: ";
			std::cout << i.second->GetButtonPos().x;
			std::cout << " || ";
			std::cout << "Pos Y: ";
			std::cout << i.second->GetButtonPos().y;

			std::cout << std::endl;
		}

	}
#endif

	Interface::PlaylistButtonContainer* Interface::PlaylistButton::GetContainer()
	{
		return &m_PlaylistButtonsContainer;
	}

	Interface::Button* Interface::PlaylistButton::GetButton(s32 id)
	{
		if (m_PlaylistButtonsContainer.empty() == true		||
			id > m_PlaylistButtonsContainer.size() - 1		||
			id < 0)
			return nullptr;

		if (id < m_PlaylistButtonsContainer.size() ||
			id >= 0)
		{
			if (m_PlaylistButtonsContainer[id].second == NULL)
				return nullptr;
		}

		return m_PlaylistButtonsContainer[id].second;
	}

	s32 Interface::PlaylistButton::GetSize()
	{
		return m_PlaylistButtonsContainer.size();
	}



}