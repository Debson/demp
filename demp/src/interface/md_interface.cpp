#include "md_interface.h"

#include <gtc/matrix_transform.hpp>
#include <algorithm>

#include "../settings/music_player_settings.h"
#include "../playlist/music_player_playlist.h"
#include "../graphics/graphics.h"
#include "../ui/music_player_ui.h"
#include "../audio/mp_audio.h"
#include "../utility/utf8_to_utf16.h"
#include "../utility/md_text.h"

using namespace mdEngine::MP::UI;
using namespace mdEngine::Graphics;
using namespace mdEngine::MP;

namespace mdEngine
{
	namespace Interface
	{
		static std::vector<std::pair<std::wstring, PlaylistSeparator*>> m_PlaylistSeparatorContainer;
		std::vector<std::pair<s32*, Interface::Button*>> m_PlaylistButtonsContainer;

		std::vector<std::pair<const std::wstring, Button*>> mdInterfaceButtonContainer;

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
	Interface::TextBoxItem::TextBoxItem(const std::wstring name, glm::vec2 itemSize, glm::vec2 itemPos,
																 glm::vec2 textSize, glm::vec2 textPos,
										GLuint tex) : m_Texture(tex)
	{
		m_Pos = itemPos;
		m_Size = itemSize;
		m_TextPos = textPos;
		m_TextSize = textSize;
		m_Pos = m_Pos;
		m_Size = m_Size;
		mdInterfaceButtonContainer.push_back(make_pair(name, this));
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

		m_ItemID = *id;

		// Button position is predefined in playlist items rendering algorithm anyway, but leave it for now
		m_StartPos = Data::_PLAYLIST_ITEMS_SURFACE_POS;
		m_ButtonPos = glm::vec2(m_StartPos.x, m_StartPos.y + m_ItemID * (m_ButtonSize.y));
		m_TextPos = m_ButtonPos;

		m_Font = MP::Data::_MUSIC_PLAYER_FONT;
		m_TextScale = 1.f;
		m_TextColor = SDLColor::Grey;
		m_TextString = Audio::Object::GetAudioObject(m_ItemID)->GetTitle();

		u16 len = m_TextString.length();
		m_TitleC.resize(len + 1);
		m_TitleC = utf16_to_utf8(m_TextString);

		TTF_SizeUTF8(m_Font, m_TitleC.c_str(), &m_TextSize.x, &m_TextSize.y);

		m_PlaylistButtonsContainer[*id] = std::make_pair(id, this);

	}

	void Interface::PlaylistItem::DrawDottedBorder(s16 playPos)
	{
		if (this->m_ButtonPos == glm::vec2(POS_INVALID))
			return;

		glm::mat4 model;
		Shader::shaderBorder->use();
		Shader::shaderBorder->setVec3("color", Color::Grey);
		f32 dotXOffset = 0.01;
		f32 dotYOffset = 0.1;
		Shader::shaderBorder->setFloat("xOffset", dotXOffset);
		Shader::shaderBorder->setFloat("yOffset", dotYOffset);
		model = glm::translate(model, glm::vec3(this->m_ButtonPos, 0.9));
		model = glm::scale(model, glm::vec3(this->m_ButtonSize, 1.f));
		Shader::shaderBorder->setMat4("model", model);
		Shader::DrawDot();
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

	b8 Interface::PlaylistItem::IsPlaying() const
	{
		return MP::Playlist::RamLoadedMusic.mID == m_ItemID;
	}

	b8 Interface::PlaylistItem::IsFolderRep() const
	{
		return m_FolderRep;
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
		m_TextString = name;
		isVisible = false;
		m_ButtonPos = glm::vec2(POS_INVALID);
		m_TextPos = glm::vec2(POS_INVALID);
		m_SepItemCount = 0;
		SepItemDuration = 0;
		m_ItemColor = Color::Blue;
		m_ButtonSize = glm::vec2(Data::_PLAYLIST_ITEM_SIZE.x, Data::_PLAYLIST_ITEM_SIZE.y / 2);
	}

	void Interface::PlaylistSeparator::InitItem(s32 posOfFirstFile)
	{
		m_Font = MP::Data::_MUSIC_PLAYER_FONT;
		m_TextScale = 1.f;
		m_TextColor = SDLColor::Grey;

		u16 len = m_TextString.length();
		m_TitleC.resize(len + 1);
		m_TitleC = utf16_to_utf8(m_TextString);

		TTF_SizeUTF8(m_Font, m_TitleC.c_str(), &m_TextSize.x, &m_TextSize.y);


		//InsertInProperOrder(posOfFirstFile);
		m_PlaylistSeparatorContainer.push_back(std::make_pair(m_TextString, this));
	}

	void Interface::PlaylistSeparator::SetSeperatorPath(std::wstring path)
	{
		m_TextString = path;
	}

	std::wstring Interface::PlaylistSeparator::GetSeparatorPath() const
	{
		return m_TextString;
	}

	std::wstring Interface::PlaylistSeparator::GetSeparatorName() const
	{
		return Audio::Info::GetFolder(m_TextString);
	}

	void Interface::PlaylistSeparator::SeparatorSubFilePushBack(const s32* fileIndex, std::wstring path)
	{
		m_SubFilesPaths.push_back(std::make_pair(fileIndex, path));
		m_SepItemCount++;
	}

	void Interface::PlaylistSeparator::SeparatorSubFileInsert(const s32* fileIndex, const std::wstring path, s32 pos)
	{
		assert(pos < m_SubFilesPaths.size());
		m_SubFilesPaths.insert(m_SubFilesPaths.begin() + pos, std::make_pair(fileIndex, path));
		m_SepItemCount++;
	}

	void Interface::PlaylistSeparator::SeparatorSubFileErased()
	{
		m_SepItemCount--;
	}

	std::vector<std::pair<const s32*, std::wstring>>* Interface::PlaylistSeparator::GetSubFilesContainer()
	{
		return &m_SubFilesPaths;
	}

	void Interface::PlaylistSeparator::InsertInProperOrder(s32 posOfFirstFile)
	{

		if (m_PlaylistSeparatorContainer.empty() == true)
		{
			m_PlaylistSeparatorContainer.push_back(std::make_pair(m_TextString, this));
			return;
		}

		s32 minPos = INT_MAX;
		/*for (auto & i : m_PlaylistSeparatorContainer)
		{
			auto subCon = i.second->GetSubFilesContainer();
			if (subCon->at(0).first < minPos && posOfFirstFile > subCon->at(0).first)
			{
				minPos = subCon->at(0).first;
			}
		}*/


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
			glBindTexture(GL_TEXTURE_2D, item->m_Texture);
			Graphics::Shader::Draw(m_Shader);
		}

	}

	void Interface::TextBox::UpdateItemPos()
	{
		for (u16 i = 0; i < m_Items.size(); i++)
		{
			m_Items[i]->m_Pos = glm::vec2(m_Pos.x, m_Pos.y + m_Items[i]->m_Index * Data::_TEXT_BOX_ITEM_HEIGHT);
			m_Items[i]->m_Pos = m_Items[i]->m_Pos;
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

		TextBoxItem * item = new TextBoxItem(itemName, glm::vec2(m_Size.x, Data::_TEXT_BOX_ITEM_HEIGHT), glm::vec2(m_Pos.x, m_Pos.y + m_ItemsCount * Data::_TEXT_BOX_ITEM_HEIGHT),
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
		auto item = std::find_if(mdInterfaceButtonContainer.begin(), mdInterfaceButtonContainer.end(),
			[&](std::pair<const std::wstring, Button*> const & ref) { return ref.first.compare(name) == 0; });

		return item == mdInterfaceButtonContainer.end() ? false : item->second->hasFocus;
	}

	b8 Interface::TextBox::isItemPressed(const std::wstring name) const
	{
		auto item = std::find_if(mdInterfaceButtonContainer.begin(), mdInterfaceButtonContainer.end(),
			[&](std::pair<const std::wstring, Button*> const & ref) { return ref.first.compare(name) == 0; });

		return item == mdInterfaceButtonContainer.end() ? false : item->second->isPressed;
	}

	
	std::vector<std::pair<std::wstring, Interface::PlaylistSeparator*>>*  Interface::Separator::GetContainer()
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

	void Interface::Separator::SortSeparatorContainer()
	{
		// Delete all playlist separators that has empty sub files containers
		for (s32 i = m_PlaylistSeparatorContainer.size() - 1; i >= 0; i--)
		{
			auto subCon = m_PlaylistSeparatorContainer[i].second->GetSubFilesContainer();
			if (subCon->empty() == true)
			{
				delete m_PlaylistSeparatorContainer[i].second;
				m_PlaylistSeparatorContainer.erase(m_PlaylistSeparatorContainer.begin() + i);
			}
		}

		std::sort(m_PlaylistSeparatorContainer.begin(), m_PlaylistSeparatorContainer.end(),
			[&](const std::pair<std::wstring, PlaylistSeparator*>  a, const std::pair<std::wstring, PlaylistSeparator*> b)
		{
			return *a.second->GetSubFilesContainer()->at(0).first < *b.second->GetSubFilesContainer()->at(0).first;
		});
	}

	s32 Interface::Separator::GetSize()
	{
		return m_PlaylistSeparatorContainer.size();
	}


	std::vector<std::pair<s32*, Interface::Button*>>* Interface::PlaylistButton::GetContainer()
	{
		return &m_PlaylistButtonsContainer;
	}

	Interface::Button* Interface::PlaylistButton::GetButton(s32 id)
	{
		if (m_PlaylistButtonsContainer.empty() == true		|| 
			m_PlaylistButtonsContainer[id].second == NULL	||
			id > m_PlaylistButtonsContainer.size()			||
			id < 0)
			return nullptr;

		return m_PlaylistButtonsContainer[id].second;
	}

	s32 Interface::PlaylistButton::GetSize()
	{
		return m_PlaylistButtonsContainer.size();
	}

}