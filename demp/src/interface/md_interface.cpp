#include "md_interface.h"

#include <gtc/matrix_transform.hpp>
#include <algorithm>
#include <random>

#include "../app/application_window.h"
#include "../settings/music_player_settings.h"
#include "../playlist/music_player_playlist.h"
#include "../graphics/graphics.h"
#include "../graphics/music_player_graphics.h"
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
		
		m_ItemID = *id;

		// Button position is predefined in playlist items rendering algorithm anyway, but leave it for now
		m_StartPos = Data::_PLAYLIST_ITEMS_SURFACE_POS;
		m_ButtonPos = glm::vec2(m_StartPos.x, m_StartPos.y + m_ItemID * (m_ButtonSize.y));
		m_TextPos = m_ButtonPos;

		m_Font = MP::Data::_MUSIC_PLAYER_FONT;
		m_TextScale = 1.f;
		SetTextColor(Color::White);

		m_TextString = Audio::Object::GetAudioObject(m_ItemID)->GetTitle();

		assert(Audio::Object::GetAudioObject(m_ItemID) != nullptr);

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

		if (Graphics::MP::GetPlaylistObject()->GetPlayingID() == m_ItemID)
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
		return MP::Playlist::RamLoadedMusic.mID == m_ItemID;
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

	Interface::ButtonSlider::ButtonSlider(glm::vec2 pos, f32* value, f32 step, glm::vec2 size) :	m_SliderPos(pos), 
																									m_SliderSize(size), 
																									m_ButtonSize(25, 25),
																									m_Value(value),
																									m_Step(step)
	{

		m_TextTexture = 0;
		m_TextPos = glm::vec2(pos.x + m_ButtonSize.x, m_SliderPos.y);
		SetTextColor(Color::White);
		m_Font = Data::_MUSIC_PLAYER_FONT;
		m_TextString = std::to_wstring(*m_Value);

		InitTextTexture();

		m_LeftPos = glm::vec2(pos.x, m_SliderPos.y);
		m_RightPos = glm::vec2(pos.x + m_ButtonSize.x + m_TextSize.x, m_SliderPos.y);

		m_Left = new Button(m_ButtonSize, m_LeftPos);
		m_Right = new Button(m_ButtonSize, m_RightPos);

	}

	Interface::ButtonSlider::~ButtonSlider() 
	{ 
		//delete m_Left;
		//delete m_Right;
	}

	void Interface::ButtonSlider::Init()
	{
		InitTextTexture();

	}

	void Interface::ButtonSlider::Update()
	{
		App::ProcessButton(m_Left);
		App::ProcessButton(m_Right);

		if (m_Right->isPressed == true)
		{
			*m_Value += m_Step;
			ReloadSliderInfo();
			md_log(*m_Value);
		}

		if (m_Left->isPressed == true)
		{
			*m_Value -= m_Step;
			ReloadSliderInfo();
			md_log(*m_Value);
		}
		
	}

	void Interface::ButtonSlider::Render()
	{
		glm::mat4 model;
		Shader::shaderWindow->use();
		model = glm::translate(model, glm::vec3(m_LeftPos, 0.5f));
		model = glm::scale(model, glm::vec3(m_ButtonSize, 1.f));
		Shader::shaderWindow->setMat4("model", model);
		Shader::shaderWindow->setVec3("color", Color::White);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		Shader::Draw(Shader::shaderWindow);


		DrawString(Shader::shaderWindow);


		model = glm::mat4();;
		Shader::shaderWindow->use();
		model = glm::translate(model, glm::vec3(m_RightPos, 0.5f));
		model = glm::scale(model, glm::vec3(m_ButtonSize, 1.f));
		Shader::shaderWindow->setMat4("model", model);
		Shader::shaderWindow->setVec3("color", Color::White);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		Shader::Draw(Shader::shaderWindow);
	}

	void Interface::ButtonSlider::ReloadSliderInfo()
	{
		m_TextString = std::to_wstring(*m_Value);
		ReloadTextTexture();
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