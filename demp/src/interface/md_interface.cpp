#include "md_interface.h"

#include <gtc/matrix_transform.hpp>
#include <algorithm>

#include "../graphics.h"
#include "../music_player_ui.h"
#include "../music_player_settings.h"
#include "../music_player_playlist.h"
#include "../utf8_to_utf16.h"
#include "../md_text.h"

using namespace mdEngine::MP::UI;
using namespace mdEngine::Graphics;
using namespace mdEngine::MP;

namespace mdEngine
{
	namespace Interface
	{
		std::vector<std::pair<const std::wstring, Button*>> mdInterfaceButtonContainer;



	}
	/* *************************************************** */
	Interface::Movable::Movable(glm::vec2 size, glm::vec2 pos) : mSize(size), mPos(pos)
	{
		mdMovableContainer.push_back(this);

	
	}

	/* *************************************************** */
	Interface::Resizable::Resizable(glm::vec2 size, glm::vec2 pos) : size(size), pos(pos)
	{
		mdResizableContainer.push_back(this);
	}

	/* *************************************************** */
	Interface::Button::Button() { }

	Interface::Button::~Button() { }

	Interface::Button::Button(glm::vec2 size, glm::vec2 pos) : mSize(size), mPos(pos) { }

	Interface::Button::Button(Input::ButtonType type, glm::vec2 size, glm::vec2 pos) : mSize(size), mPos(pos)
	{
		mdButtonsContainer.push_back(std::make_pair(type, this));
	}


	/* *************************************************** */
	Interface::TextBoxItem::TextBoxItem(const std::wstring name, glm::vec2 itemSize, glm::vec2 itemPos,
																 glm::vec2 textSize, glm::vec2 textPos,
										GLuint tex) : m_Tex(tex)
	{
		m_Pos = itemPos;
		m_Size = itemSize;
		m_TextPos = textPos;
		m_TextSize = textSize;
		mPos = m_Pos;
		mSize = m_Size;
		mdInterfaceButtonContainer.push_back(make_pair(name, this));
	}


	/* *************************************************** */
	Interface::PlaylistItem::~PlaylistItem() { }

	void Interface::PlaylistItem::InitFont()
	{
		mFont = MP::Data::_MUSIC_PLAYER_FONT;
		if (mFont == NULL)
		{
			std::cout << "ERROR: initfont!\n";
		}

		mTextScale = 1.0f;

	}

	void Interface::PlaylistItem::InitItem()
	{
		mColor = glm::vec3(1.f);

		mStartPos = Data::_PLAYLIST_ITEMS_SURFACE_POS;
		mSize = Data::_PLAYLIST_ITEM_SIZE;

		mPos = glm::vec2(mStartPos.x, mStartPos.y + mCount * (mSize.y + mOffsetY));

		mTextColor = { 255, 255, 255 };

		mID = mCount;
		mTitle = Playlist::GetTitle(mID);

		u16 len = mTitle.length();
		mTitleC.resize(len + 1);
		mTitleC = utf16_to_utf8(mTitle);

		TTF_SizeText(mFont, mTitleC.c_str(), &mTextSize.x, &mTextSize.y);

		mdItemContainer.push_back(this);
		mdPlaylistButtonsContainer.push_back(std::make_pair(Input::ButtonType::None, this));

		mCount++;
	}	

	void Interface::PlaylistItem::UpdateItem()
	{
		/*mTitle = Playlist::GetTitle(mID);
		u16 len = wcslen(mTitle.c_str());
		mTitleC = new char[len + 1];
		mTitleC[len] = '\0';
		wcstombs(mTitleC, mTitle.c_str(), len);

		TTF_SizeText(mFont, mTitleC, &mTextSize.x, &mTextSize.y);*/

	}

	void Interface::PlaylistItem::SetColor(glm::vec3 color)
	{
		mTextColor = { u8(color.x * 255.f), u8(color.y * 255.f), u8(color.z * 255.f) };
	}

	void Interface::PlaylistItem::DrawDottedBorder(s16 playPos)
	{
		if (this->mPos == glm::vec2(INVALID))
			return;

		glm::mat4 model;
		Shader::shaderBorder->use();
		Shader::shaderBorder->setVec3("color", Color::Grey);
		f32 dotXOffset = 0.01;
		f32 dotYOffset = 0.1;
		Shader::shaderBorder->setFloat("xOffset", dotXOffset);
		Shader::shaderBorder->setFloat("yOffset", dotYOffset);
		model = glm::translate(model, glm::vec3(this->mPos, 0.9));
		model = glm::scale(model, glm::vec3(this->mSize, 1.f));
		Shader::shaderBorder->setMat4("model", model);
		Shader::DrawDot();
	}

	std::wstring Interface::PlaylistItem::GetTitle()
	{
		s16 len = wcslen(mTitle.c_str());
		f32 textSize = mTextSize.x * mTextScale;

		if (textSize > this->mSize.x)
		{
			float charSize = textSize / (float)len;
			u16 i = 0;
			u16 pos = 0;
			while (i * charSize < this->mSize.x)
				i++;

			mTitle = mTitle.substr(0, i - 4);
			mTitle += L"...";

			len = mTitle.length();
			mTitleC.resize(len + 1);
			mTitleC = utf16_to_utf8(mTitle);
			TTF_SizeText(mFont, mTitleC.c_str(), &mTextSize.x, &mTextSize.y);;
		}


		return mTitle;
	}

	b8 Interface::PlaylistItem::IsPlaying()
	{
		return MP::Playlist::RamLoadedMusic.mID == mID;
	}

	s32 Interface::PlaylistItem::mOffsetY = 0;

	s32 Interface::PlaylistItem::mCount = 0;

	s32 Interface::PlaylistItem::mOffsetIndex = 0;

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
			model = glm::translate(model, glm::vec3(item->m_Pos, 0.7));
			model = glm::scale(model, glm::vec3(item->m_Size, 1.0));;
			m_Shader->setMat4("model", model);
			glBindTexture(GL_TEXTURE_2D, 0);
			Graphics::Shader::Draw(m_Shader);

			//render text on the item
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(item->m_TextPos, 0.8));
			model = glm::scale(model, glm::vec3(item->m_TextSize, 1.0));;
			m_Shader->setMat4("model", model);
			glBindTexture(GL_TEXTURE_2D, item->m_Tex);
			Graphics::Shader::Draw(m_Shader);
		}

	}

	void Interface::TextBox::UpdateItemPos()
	{
		for (u16 i = 0; i < m_Items.size(); i++)
		{
			m_Items[i]->m_Pos = glm::vec2(m_Pos.x, m_Pos.y + m_Items[i]->m_Index * Data::_TEXT_BOX_ITEM_HEIGHT);
			m_Items[i]->mPos = m_Items[i]->m_Pos;
			m_Items[i]->m_TextPos = glm::vec2(m_Items[i]->m_Pos.x + 40.f, m_Items[i]->m_Pos.y);
		}
	}

	void Interface::TextBox::SetPos(glm::vec2 pos)
	{
		m_Pos = pos;

		auto item = std::find_if(mdButtonsContainer.begin(), mdButtonsContainer.end(),
			[&](std::pair<Input::ButtonType, Interface::Button*> const& ref) { return ref.first == m_Type; });

		if (item != mdButtonsContainer.end())
		{
			item->second->mPos = m_Pos;
		}

	}

	void Interface::TextBox::SetSize(glm::vec2 size)
	{
		m_Size = size;

		auto item = std::find_if(mdButtonsContainer.begin(), mdButtonsContainer.end(),
			[&](std::pair<Input::ButtonType, Interface::Button*> const& ref) { return ref.first == m_Type; });

		if (item != mdButtonsContainer.end())
		{
			item->second->mSize = m_Size;
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

		TextBoxItem * item = new TextBoxItem(itemName, glm::vec2(m_Size.x, Data::_TEXT_BOX_ITEM_HEIGHT), glm::vec2(m_Pos.x, m_Pos.y + m_ItemsCount * Data::_TEXT_BOX_ITEM_HEIGHT),
													   glm::vec2(textSize) * m_ItemScale, glm::vec2(m_Pos.x + 40.f, m_Pos.y + m_ItemsCount * textSize.y * m_ItemScale),
											 tex);
		item->m_Index = m_ItemsCount;
		m_Items.push_back(item);
		m_ItemsCount++;
	}

	glm::vec2 Interface::TextBox::GetPos()
	{
		return m_Pos;
	}

	glm::vec2 Interface::TextBox::GetSize()
	{
		return m_Size;
	}

	b8 Interface::TextBox::hasItemFocus(const std::wstring name)
	{
		auto item = std::find_if(mdInterfaceButtonContainer.begin(), mdInterfaceButtonContainer.end(),
			[&](std::pair<const std::wstring, Button*> const & ref) { return ref.first.compare(name) == 0; });

		return item == mdInterfaceButtonContainer.end() ? false : item->second->hasFocus;
	}

	b8 Interface::TextBox::isItemPressed(const std::wstring name)
	{
		auto item = std::find_if(mdInterfaceButtonContainer.begin(), mdInterfaceButtonContainer.end(),
			[&](std::pair<const std::wstring, Button*> const & ref) { return ref.first.compare(name) == 0; });

		return item == mdInterfaceButtonContainer.end() ? false : item->second->isPressed;
	}

	
}