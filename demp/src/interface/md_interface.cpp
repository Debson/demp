#include "md_interface.h"

#include <gtc/matrix_transform.hpp>
#include <algorithm>
#include <random>
#include <iomanip>
#include <mutex>

#include "../app/application_window.h"
#include "../app/realtime_system_application.h"
#include "../settings/music_player_settings.h"
#include "../playlist/music_player_playlist.h"
#include "../player/music_player_state.h"
#include "../player/music_player_resources.h"
#include "../graphics/graphics.h"
#include "../graphics/music_player_graphics_playlist.h"
#include "../ui/music_player_ui.h"
#include "../audio/mp_audio.h"
#include "../utility/md_converter.h"
#include "../utility/utf8_to_utf16.h"
#include "../utility/md_text.h"
#include "../utility/md_load_texture.h"
#include "../utility/md_math.h"

using namespace mdEngine::MP::UI;
using namespace mdEngine::Graphics;
using namespace mdEngine::MP;

namespace mdEngine
{
	namespace Interface
	{
		static PlaylistSeparatorContainer	m_PlaylistSeparatorContainer;
		InterfaceButtonContainer			m_InterfaceButtonContainer;

		static std::mutex mutex;
	}

	void Interface::UpdateFont()
	{
		if ((State::CheckState(State::Window::Resized) == true || State::CheckState(State::PlaylistMovement) == true) &&
			Audio::AudioObject::AudioObjectFont == NULL)
		{
			Audio::AudioObject::AudioObjectFont = TTF_OpenFont(Strings::_FONT_PATH.c_str(), Audio::AudioObject::AudioObjectFontSize);
			md_log("audio object font");
		}
		else if (State::CheckState(State::Window::Resized) == false &&
			State::CheckState(State::PlaylistMovement) == false &&
			Audio::AudioObject::AudioObjectFont != NULL)
		{
			TTF_CloseFont(Audio::AudioObject::AudioObjectFont);
			Audio::AudioObject::AudioObjectFont = NULL;
		}


		if ((State::CheckState(State::Window::Resized) == true || State::CheckState(State::PlaylistMovement) == true) &&
			PlaylistSeparator::SeparatorItemFont == NULL)
		{
			PlaylistSeparator::SeparatorItemFont = TTF_OpenFont(Strings::_FONT_PATH.c_str(), PlaylistSeparator::SeparatorItemFontSize);
			md_log("audio object font");
		}
		else if (State::CheckState(State::Window::Resized) == false &&
			State::CheckState(State::PlaylistMovement) == false &&
			PlaylistSeparator::SeparatorItemFont != NULL)
		{
			TTF_CloseFont(PlaylistSeparator::SeparatorItemFont);
			PlaylistSeparator::SeparatorItemFont = NULL;
		}

		State::ResetState(State::PlaylistMovement);
	
	}

	void Interface::CloseInterface()
	{

		m_PlaylistSeparatorContainer.clear();
	}

	void Interface::PrintSeparatorAndItsSubFiles()
	{
		for (auto & i : m_PlaylistSeparatorContainer)
		{
			std::cout << i.second->GetSeparatorName() << std::endl;
			for (auto & k : *i.second->GetSubFilesContainer())
			{
				std::cout << "  ";
				std::cout << "ID: " << *k.first  << "   Path: " << *k.second << std::endl;
			}
		}
	}

	/* *************************************************** */
	Interface::Movable::Movable() { }

	Interface::Movable::Movable(glm::vec2 size, glm::vec2 pos, b8 addToCont) : m_Size(size), m_Pos(pos)
	{
		if(addToCont == true)
			mdMovableContainer.push_back(this);
	}

	/* *************************************************** */
	Interface::Resizable::Resizable() { }

	Interface::Resizable::Resizable(glm::vec2 size, glm::vec2 pos) : m_Size(size), m_Pos(pos)
	{
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
		m_MusicLengthText = NULL;
	}

	Interface::PlaylistItem::~PlaylistItem() 
	{ 
		DeleteTexture();
	}

	void Interface::PlaylistItem::InitItem()
	{
		m_ItemColor = Color::White;
		m_ButtonSize = Data::_PLAYLIST_ITEM_SIZE;
		m_ClickCount = 0;
		m_PlaylistItemHidden = false;
		m_ItemColor = Color::White;

		// Button position is predefined in playlist items rendering algorithm anyway, but leave it for now
		m_StartPos = Data::_PLAYLIST_ITEMS_SURFACE_POS;
		m_ButtonPos = glm::vec2(m_StartPos.x, m_StartPos.y + m_ItemID * (m_ButtonSize.y));
		m_TextPos = m_ButtonPos;

		m_TextScale = 1.f;
		SetTextColor(Color::White);

		m_TextString = Audio::Object::GetAudioObject(m_ItemID)->GetNameToPlaylist();

		assert(Audio::Object::GetAudioObject(m_ItemID) != nullptr);

		m_TitleC = m_TextString;


		//TTF_SizeUTF8(m_Font, m_TitleC.c_str(), &m_TextSize.x, &m_TextSize.y);
		//m_TextString = GetShortenTextString();
	}

	void Interface::PlaylistItem::DrawDottedBorder()
	{
		m_PlaylistItemPos = glm::vec2(m_ButtonPos.x, m_ButtonPos.y - *m_PlaylistOffsetY);

		glm::mat4 model;
		Shader::shaderBorder->use();
		Shader::shaderBorder->setVec3("color", Color::Grey);
		f32 dotXOffset = 0.01;
		f32 dotYOffset = 0.1;
		Shader::shaderBorder->setFloat("xOffset", dotXOffset);
		Shader::shaderBorder->setFloat("yOffset", dotYOffset);
		model = glm::translate(model, glm::vec3(m_PlaylistItemPos, 0.9));
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

		m_PlaylistItemPos = glm::vec2(m_ButtonPos.x, m_ButtonPos.y - *m_PlaylistOffsetY);
		m_TextPos = m_PlaylistItemPos;

		if (Graphics::MP::GetPlaylistObject()->GetPlayingID() == m_ItemID &&
			State::CheckState(State::CurrentlyPlayingDeleted) == false)
		{
			m_ItemColor *= Color::Red;
		}


		if (m_MusicLengthText == NULL)
		{
			m_MusicLengthText = new Text::TextObject(Color::White, 12);
			m_MusicLengthText->SetTextString(Converter::SecToProperTimeFormatShort(Audio::Object::GetAudioObject(m_ItemID)->GetLength()));
			m_MusicLengthText->InitTextTexture();
		}
	
		
		m_MusicLengthText->SetTextPos(glm::vec2(m_ButtonPos.x + m_ButtonSize.x - m_MusicLengthText->GetTextSize().x, m_PlaylistItemPos.y));
		m_MusicLengthText->DrawString();
		

		Shader::shaderDefault->use();
		halvesColor *= m_ItemColor;


		if (topHasFocus == true)
		{
			model = glm::translate(model, glm::vec3(m_PlaylistItemPos, 0.5f));
			model = glm::scale(model, glm::vec3(m_ButtonSize.x, m_ButtonSize.y / 2.f, 1.0f));
			Shader::shaderDefault->setMat4("model", model);
			Shader::shaderDefault->setVec3("color", halvesColor);
			glBindTexture(GL_TEXTURE_2D, texture);
			Shader::Draw(Shader::shaderDefault);
		}


		if (bottomHasFocus == true)
		{
			model = glm::translate(model, glm::vec3(m_PlaylistItemPos.x, m_PlaylistItemPos.y + m_ButtonSize.y / 2.f, 0.5f));
			model = glm::scale(model, glm::vec3(m_ButtonSize.x, m_ButtonSize.y / 2.f, 1.0f));
			Shader::shaderDefault->setMat4("model", model);
			Shader::shaderDefault->setVec3("color", halvesColor);
			glBindTexture(GL_TEXTURE_2D, texture);
			Shader::Draw(Shader::shaderDefault);
		}


		model = glm::mat4();
		model = glm::translate(model, glm::vec3(m_PlaylistItemPos, 0.5f));
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

	glm::vec2& Interface::PlaylistItem::GetPlaylistItemPos()
	{
		assert(this != NULL);
		m_PlaylistItemPos = glm::vec2(m_ButtonPos.x, m_ButtonPos.y - *m_PlaylistOffsetY);
		return m_PlaylistItemPos;
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

	void Interface::PlaylistItem::SetFolderRep(b8 val)
	{
		m_FolderRep = val;
	}

	void Interface::PlaylistItem::SetClickCount(s8 count)
	{
		m_ClickCount = count;
	}

	void Interface::PlaylistItem::SetItemColor(glm::vec3 color)
	{
		m_ItemColor = color;
	}

	b8 Interface::PlaylistItem::IsSelected()
	{
		return m_ClickCount > 0 ? true : false;
	}

	void Interface::PlaylistItem::SetPlaylistOffsetY(f32* offsetY)
	{
		m_PlaylistOffsetY = offsetY;
	}

	void Interface::PlaylistItem::DeleteTexture()
	{
		if (m_TextTexture > 0)
		{
			glDeleteTextures(1, &m_TextTexture);
			m_TextTexture = 0;
		}

		if (m_MusicLengthText != NULL)
		{
			delete m_MusicLengthText;
			m_MusicLengthText = NULL;
		}
	}

	b8 Interface::PlaylistItem::IsVisible() const
	{
		return m_Visible;
	}

	b8 Interface::PlaylistItem::IsPlaying() const
	{
		return MP::Playlist::RamLoadedMusic.m_ID == m_ItemID;
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

	std::string Interface::PlaylistItem::GetShortenTextString()
	{
		u16 len = m_TextString.length();
		f32 textSize = m_TextSize.x * m_TextScale;

		if (textSize > this->m_ButtonSize.x)
		{
			float charSize = textSize / (float)len;
			u16 i = 0;
			u16 pos = 0;
			while (i * charSize < this->m_ButtonSize.x)
				i++;

			m_TextString = m_TextString.substr(0, i - 4);
			m_TextString += "...";

			len = m_TextString.length();
			m_TitleC.resize(len + 1);
			m_TitleC = m_TextString;
			TTF_Font* font = TTF_OpenFont(Strings::_FONT_PATH.c_str(), m_FontSize);
			TTF_SizeUTF8(font, m_TitleC.c_str(), &m_TextSize.x, &m_TextSize.y);;
			TTF_CloseFont(font);
		}

		return m_TextString;
	}

	s32 Interface::PlaylistItem::OffsetIndex = 0;

	f32* Interface::PlaylistItem::m_PlaylistOffsetY = NULL;

	/* *************************************************** */

	TTF_Font* Interface::PlaylistSeparator::SeparatorItemFont = NULL;

	s32 Interface::PlaylistSeparator::SeparatorItemFontSize = 12;

	Interface::PlaylistSeparator::PlaylistSeparator() { }

	Interface::PlaylistSeparator::~PlaylistSeparator()
	{
		m_SubFilesPaths.clear();
	}

	Interface::PlaylistSeparator::PlaylistSeparator(std::string& name)
	{ 
		u16 pos = name.find_last_of(L'\\');
		m_Path = std::string(name.substr(0, pos));

		m_TextString = Audio::Info::GetFolder(m_Path);
		m_Visible = false;
		m_ButtonPos = glm::vec2(POS_INVALID);
		m_TextPos = glm::vec2(POS_INVALID);
		m_SepItemCount = 0;
		SepItemDuration = 0;
		m_ItemColor = Color::Blue;
		m_ButtonSize = Data::_PLAYLIST_SEPARATOR_SIZE;
	}

	void Interface::PlaylistSeparator::InitItem()
	{
		m_RenderedItemsCount = 0;
		m_TextScale = 1.f;
		m_FontSize = 12;
		m_ItemColor = Color::Pink;
		SetTextColor(Color::Grey);

		m_SeparatorHidden = false;
		m_SeparatorSelected = false;

		u16 len = m_TextString.length();
		m_TitleC.resize(len + 1);
		m_TitleC = m_TextString;
		
		std::shared_ptr<PlaylistSeparator> shrPtr(this);

		m_PlaylistSeparatorContainer.push_back(std::make_pair(&m_Path, shrPtr));

		m_SeparatorInfoText = Text::TextObject(m_TextColorVec, SeparatorItemFontSize);
	}

	void Interface::PlaylistSeparator::DrawItem(GLuint texture)
	{
		m_PlaylistItemPos = glm::vec2(m_ButtonPos.x, m_ButtonPos.y - *m_PlaylistOffsetY);
		m_TextPos = m_PlaylistItemPos;
		Shader::shaderDefault->use();
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(m_PlaylistItemPos, 0.5f));
		model = glm::scale(model, glm::vec3(m_ButtonSize, 1.0f));
		Shader::shaderDefault->setMat4("model", model);
		Shader::shaderDefault->setVec3("color", m_ItemColor);
		glBindTexture(GL_TEXTURE_2D, texture);
		Shader::Draw(Shader::shaderDefault);

		// Draw String
		s32 offsetY = (m_ButtonSize.y - m_TextSize.y) / 2;
		SetTextOffset(glm::vec2(5.f, offsetY));
		DrawString();

	
		if (m_SeparatorInfoText.HasTexture() == false ||
			m_RenderedItemsCount != m_SubFilesPaths.size())
		{
			f64 duration = 0.f;
			for (auto & i : m_SubFilesPaths)
			{
				duration += Audio::Object::GetAudioObject(*i.first)->GetLength();
			}
			m_RenderedItemsCount = m_SubFilesPaths.size();
			std::string str = std::to_string(m_RenderedItemsCount) + " / " + Converter::SecToProperTimeFormatShort(duration);
			m_SeparatorInfoText.SetTextString(str);
			m_SeparatorInfoText.SetTextOffset(glm::vec2(5.f, offsetY));
			m_SeparatorInfoText.InitTextTexture();
		}
		m_SeparatorInfoText.SetTextPos(glm::vec2(m_PlaylistItemPos.x + m_ButtonSize.x - m_SeparatorInfoText.GetTextSize().x - 8.f, m_PlaylistItemPos.y));

		m_SeparatorInfoText.DrawString();
	}

	void Interface::PlaylistSeparator::InitTextTexture()
	{
		if (m_TextTexture == 0)
		{
			TTF_Font* font = NULL;
			if (SeparatorItemFont != NULL)
			{
				font = SeparatorItemFont;
				assert(font != NULL);
			}
			else
				font = TTF_OpenFont(Strings::_FONT_PATH.c_str(), SeparatorItemFontSize);

			Converter::GetShortenString(m_TextString, m_ButtonSize.x - 40, font);
			TTF_SizeUTF8(font, m_TextString.c_str(), &m_TextSize.x, &m_TextSize.y);
			m_TextTexture = LoadText(font, m_TextString, m_TextColorSDL);
			if (SeparatorItemFont == NULL)
			{
				TTF_CloseFont(font);
			}
		}
	}

	void Interface::PlaylistSeparator::DeleteTexture()
	{
		if (m_TextTexture > 0)
		{
			glDeleteTextures(1, &m_TextTexture);
			m_TextTexture = 0;
		}

		if (m_SeparatorInfoText.HasTexture() == true)
		{
			m_SeparatorInfoText.DeleteTexture();
		}
	}

	void Interface::PlaylistSeparator::SetSeperatorPath(std::string& path)
	{
		m_Path = path;
	}

	void Interface::PlaylistSeparator::SeparatorSubFilePushBack(s32* fileIndex, std::string& const path)
	{
		m_SubFilesPaths.push_back(std::make_pair(fileIndex, &path));
		m_SepItemCount++;
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
				audioCon->at(*i.first)->HidePlaylistItem(true);
			}
		}
		else
		{
			for (auto i : m_SubFilesPaths)
			{
				audioCon->at(*i.first)->HidePlaylistItem(false);
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

	std::string Interface::PlaylistSeparator::GetSeparatorPath() const
	{
		return m_Path;
	}

	std::string Interface::PlaylistSeparator::GetSeparatorName() const
	{
		return m_TextString;
	}


	Interface::SeparatorSubContainer* Interface::PlaylistSeparator::GetSubFilesContainer()
	{
		return &m_SubFilesPaths;
	}

	/* *************************************************** */
	Interface::TextBoxItem::TextBoxItem(std::string& name, glm::vec2 itemSize, glm::vec2 itemPos, GLuint iconTexture)
	{
		m_ButtonPos = itemPos;
		m_ButtonSize = itemSize;
		m_IconTexture = iconTexture;
		//m_InterfaceButtonContainer.push_back(make_pair(name, this));
	}

	void Interface::TextBoxItem::UpdateTextBoxItemPos(glm::vec2 pos, glm::vec2 offset)
	{
		m_ButtonPos = glm::vec2(pos.x, pos.y + m_Index * Data::_TEXT_BOX_ITEM_HEIGHT + offset.y);
		m_TextPos = glm::vec2(m_ButtonPos.x + offset.x, m_ButtonPos.y);
	}

	/* *************************************************** */
	Interface::TextBox::TextBox() { }

	Interface::TextBox::~TextBox()
	{
		m_Items.clear();
	}

	Interface::TextBox::TextBox(glm::vec2 pos, glm::vec2 size, mdShader* shader) : m_Pos(pos), 
																				   m_Size(size), 
																				   m_Shader(shader)
	{ 
		m_ButtonPos = pos;
		m_ButtonSize = size;
		m_ItemsCount = 0;
		m_TextBoxBackgroundTexture = 0;
	}

	void Interface::TextBox::Update()
	{
		App::ProcessButton(this);
		for (auto & i : m_Items)
			App::ProcessButton(i);
	}

	void Interface::TextBox::Render(b8 roundEdges)
	{
		assert(m_Shader != NULL);


		s32 winOffset = Data::_WINDOW_TEXTBOXES_OFFSET;
		m_Shader->use();
		m_Projection = glm::ortho(0.f, static_cast<f32>(Window::WindowProperties.m_WindowWidth + winOffset),
									   static_cast<f32>(Window::WindowProperties.m_WindowHeight), 0.f);
		m_Shader->setMat4("projection", m_Projection);
		glViewport(0.f, 0.f, static_cast<f32>(Window::WindowProperties.m_WindowWidth + winOffset),
							 static_cast<f32>(Window::WindowProperties.m_WindowHeight));

		glm::mat4 model;
		model = glm::translate(model, glm::vec3(m_Pos, 0.95f));
		model = glm::scale(model, glm::vec3(m_Size, 1.0));
		//m_Shader->setBool("plain", true);
		m_Shader->setVec3("color", Color::White);
		m_Shader->setMat4("model", model);
		if (roundEdges == true)
		{
			m_Shader->setBool("roundEdges", true);
			m_Shader->setFloat("roundEdgesFactor", 0.04);
		}
		glBindTexture(GL_TEXTURE_2D, m_TextBoxBackgroundTexture);
		Graphics::Shader::Draw(m_Shader);
		//m_Shader->setBool("plain", false);
		
		m_Shader->setBool("roundEdges", false);

		for (s32 i = 0; i < m_Items.size(); i++)
		{
			auto item = m_Items[i];

			if (item->m_IconTexture > 0)
			{
				model = glm::mat4();
				model = glm::translate(model, glm::vec3(Data::_PLAYLIST_ADD_ICON_POS.x, item->m_TextPos.y + (item->m_TextSize.y - Data::_PLAYLIST_ADD_ICON_SIZE.y) / 2.f, 1.f));
				model = glm::scale(model, glm::vec3(Data::_PLAYLIST_ADD_ICON_SIZE, 1.0));;
				m_Shader->setMat4("model", model);
				glBindTexture(GL_TEXTURE_2D, item->m_IconTexture);
				Graphics::Shader::Draw(m_Shader);
			}
			if(item->hasFocus == true)
				item->SetTextColor(Color::Green);
			else
				item->SetTextColor(Color::Grey);

			item->DrawString(true);

			if (item->hasFocus == true)
			{
				s32 offsetX = 3;
				model = glm::mat4();
				model = glm::translate(model, glm::vec3(glm::vec2(m_Pos.x + offsetX, item->m_ButtonPos.y), 1.f));
				model = glm::scale(model, glm::vec3(glm::vec2(m_Size.x - 2 * offsetX, item->m_ButtonSize.y), 1.0));;
				m_Shader->setMat4("model", model);
				glBindTexture(GL_TEXTURE_2D, m_TextBoxSelectTexture);
				Graphics::Shader::Draw(m_Shader);
			}
		}

		m_Shader->setMat4("projection", *Graphics::GetProjectionMatrix());
		glViewport(0.f, 0.f, Window::WindowProperties.m_WindowWidth, Window::WindowProperties.m_WindowHeight);
	}

	void Interface::TextBox::UpdateItemsPos()
	{
		for (u16 i = 0; i < m_Items.size(); i++)
		{
			m_Items[i]->UpdateTextBoxItemPos(m_Pos, m_ItemsOffset);
		}
	}

	void Interface::TextBox::UpdateItemsPos(glm::vec2 pos)
	{
		m_Pos = pos;
		for (u16 i = 0; i < m_Items.size(); i++)
		{
			m_Items[i]->UpdateTextBoxItemPos(m_Pos, m_ItemsOffset);
		}
	}

	void Interface::TextBox::SetBackgroundTexture(GLuint tex)
	{
		m_TextBoxBackgroundTexture = tex;
	}

	void Interface::TextBox::SetSelectTexture(GLuint tex)
	{
		m_TextBoxSelectTexture = tex;
	}

	void Interface::TextBox::SetPos(glm::vec2 pos)
	{
		f32 diffX = m_Pos.x - pos.x;
		f32 diffY = m_Pos.y - pos.y;

		for (auto & i : m_Items)
		{
			i->SetButtonPos(glm::vec2(i->GetButtonPos().x - diffX, i->GetButtonPos().y - diffY));
			i->SetTextPos(glm::vec2(i->GetTextPos().x - diffX, i->GetTextPos().y - diffY));
		}
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

	void Interface::TextBox::AddItem(std::string itemName, GLuint iconTexture)
	{
		glm::ivec2 textSize;
		std::string name = itemName;

		auto item = std::make_shared<TextBoxItem>(itemName, 
												  glm::vec2(m_Size.x, Data::_TEXT_BOX_ITEM_HEIGHT), 
												  glm::vec2(m_Pos.x, m_Pos.y + m_ItemsCount * Data::_TEXT_BOX_ITEM_HEIGHT),
												  iconTexture);
		item->m_Index = m_ItemsCount;
		item->SetTextPos(glm::vec2(m_Pos.x + m_ItemsOffset.x, m_Pos.y + m_ItemsCount * textSize.y * m_ItemScale + m_ItemsOffset.y));
		item->SetTextString(itemName);
		item->SetTextColor(m_TextColorVec);
		item->SetFontSize(m_FontSize);
		item->InitTextTexture();
		item->ResetState();
		m_Items.push_back(item);
		m_ItemsCount++;
	}

	void Interface::TextBox::SetItemsOffset(glm::vec2 offset)
	{
		m_ItemsOffset = offset;
	}

	glm::vec2 Interface::TextBox::GetPos() const
	{
		return m_Pos;
	}

	glm::vec2 Interface::TextBox::GetSize() const
	{
		return m_Size;
	}

	b8 Interface::TextBox::hasItemFocus(s32 id) const
	{
		/*auto item = std::find_if(m_InterfaceButtonContainer.begin(), m_InterfaceButtonContainer.end(),
			[&](std::pair<const std::string, Button*> const & ref) { return ref.first.compare(name) == 0; });*/

		if (id < 0 || id >= m_Items.size())
			return false;

		return m_Items[id]->hasFocus;
	}

	b8 Interface::TextBox::isItemPressed(s32 id) const
	{
		/*auto item = std::find_if(m_InterfaceButtonContainer.begin(), m_InterfaceButtonContainer.end(),
			[&](std::pair<const std::string, Button*> const & ref) { return ref.first.compare(name) == 0; });*/

		if (id < 0 || id >= m_Items.size())
			return false;

		return m_Items[id]->isPressed;
	}


	void Interface::TextBox::Free()
	{

	}

	void Interface::PlaylistItemTextBox::SetSelectedItemID(u32 id)
	{
		m_SelectedID = id;
	}

	u32 Interface::PlaylistItemTextBox::GetSelectedItemID()
	{
		return m_SelectedID;
	}


	void Interface::SettingsTextBox::Init()
	{
		this->SetPos(Data::_SETTINGS_BUTTON_TEXTBOX_POS);
		this->SetSize(Data::_SETTINGS_BUTTON_TEXTBOX_SIZE);
		this->SetTextColor(Color::White);
		this->SetBackgroundTexture(0);	// custom text
		this->SetSelectTexture(0); // custom tex
		this->SetItemsOffset(glm::vec2(10.f, 5.f));
		this->SetFontSize(12);

		this->AddItem("Options");
		this->AddItem("Add files");
		this->AddItem("Add folder");
		this->UpdateItemsPos();
	}

	Interface::ButtonSlider::ButtonSlider() { }

	Interface::ButtonSlider::ButtonSlider(std::string labelName, glm::ivec2 pos, f32* value, f32 step, f32 min, f32 max, glm::vec2 size) :	m_SliderSize(size),
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

	Interface::ButtonSlider::ButtonSlider(std::string labelName, glm::ivec2 pos, s32* value, s32 step, s32 min, s32 max, glm::vec2 size) :	m_SliderSize(size),
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

	Interface::ButtonSlider::~ButtonSlider()
	{
		//Free();
	}

	void Interface::ButtonSlider::Init(mdShader* shader)
	{
		m_Shader = shader;
		m_LeftTexture = Resources::interface_button_slider;
		m_RightTexture = m_LeftTexture;

		if (m_LeftTexture == NULL)
		{
			MD_ERROR("Texture options window");
		}

		glm::ivec2 buttonTexSize(12, 12);
		s32 offsetY = 5;

		s32 labelTextXOffset = 3;
		m_LabelTextObject = TextObject(Color::Black);
		m_LabelTextObject.SetTextString(m_LabelText);
		m_LabelTextObject.SetTextPos(glm::vec2(m_SliderPos.x - labelTextXOffset, m_SliderPos.y));
		// Init sdl text
		m_LabelTextObject.InitTextTexture();
		

		s32 outlineSizeOffsetX = 4;
		m_SliderOutline = glm::vec4(m_SliderPos.x - outlineSizeOffsetX, m_SliderPos.y + m_LabelTextObject.GetTextSize().y + offsetY - (m_SliderSize.y - buttonTexSize.y) / 2,
									m_SliderSize.x + 2 * outlineSizeOffsetX - 1.f, m_SliderSize.y);

		m_ValueTextObject = TextObject(Color::Black);
		if (m_Value == NULL)
		{
			std::stringstream ss;
			std::string val;
			ss << std::setprecision(3) << *m_ValueF;
			ss >> val;
			m_ValueTextObject.SetTextString(val);
		}
		else
		{
			m_ValueTextObject.SetTextString(std::to_string(*m_Value));
		}


		m_ValueTextObject.InitTextTexture();


		s32 offsetX = (m_SliderSize.x - 2 * buttonTexSize.x - m_ValueTextObject.GetTextSize().x) / 2;

		m_ValueTextObject.SetTextPos(glm::vec2(m_SliderPos.x + buttonTexSize.x + offsetX,
											   m_SliderPos.y + m_LabelTextObject.GetTextSize().y + offsetY - (m_SliderSize.y - m_ValueTextObject.GetTextSize().y) / 2));

		//SDL_QueryTexture(m_LeftTexture, NULL, NULL, &texW, &texH);	

		s32 texW = 10;
		s32 texH = 10;

		m_LeftDest	= glm::vec4(m_SliderPos.x + 10, m_SliderPos.y + m_LabelTextObject.GetTextSize().y + offsetY,
								buttonTexSize.x, buttonTexSize.y);
		m_RightDest	= glm::vec4(m_SliderPos.x + buttonTexSize.x + m_ValueTextObject.GetTextSize().x + 2 * offsetX,
								m_SliderPos.y + m_LabelTextObject.GetTextSize().y + offsetY, 
								buttonTexSize.x, buttonTexSize.y);


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

		m_RightBackground = glm::vec4(m_RightButton->GetButtonPos(), m_RightButton->GetButtonSize());


		m_LeftBackground = glm::vec4(m_LeftButton->GetButtonPos(), m_LeftButton->GetButtonSize());

		m_RightBackgroundAlpha	= 0;
		m_LeftBackgroundAlpha	= 0;

		m_ButtonsBackgroundColor = Color::Orange;

		s32 fadeTimeValue	= 150;
		m_FadeTimerRight	= Time::Timer(fadeTimeValue);
		m_FadeTimerLeft		= Time::Timer(fadeTimeValue);

		m_SliderBackground = m_SliderOutline;
		m_SliderBackgroundColor = Color::Grey;



		m_DefaultRect = glm::vec4(m_SliderPos.x + m_SliderSize.x + 10, m_SliderOutline.y,
								  70, m_SliderSize.y);

		m_DefaultButton = new Button();
		m_DefaultButton->SetButtonPos(glm::vec2(m_DefaultRect.x, m_DefaultRect.y));
		m_DefaultButton->SetButtonSize(glm::vec2(m_DefaultRect.z, m_DefaultRect.w));

		m_DefaultTextObject = TextObject(Color::Black);

		m_DefaultTextObject.SetTextString("Default");
		m_DefaultTextObject.InitTextTexture();
		// Init sdl text

		offsetX = (m_DefaultRect.z - m_DefaultTextObject.GetTextSize().x) / 2;
		offsetY = (m_DefaultRect.w - m_DefaultTextObject.GetTextSize().y) / 2;
		m_DefaultTextObject.SetTextPos(glm::vec2(m_DefaultRect.x + offsetX,
												 m_SliderOutline.y + offsetY));


	}

	void Interface::ButtonSlider::Update()
	{
		f32 startAlpha = 1.f;
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
			m_DefaultButtonColor = Color::Silver;
		}
		else
		{
			m_DefaultButtonColor = Color::DarkGrey;
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
		m_Shader->use();
		glm::mat4 model;

		m_Shader->setBool("plain", true);
		model = glm::translate(model, glm::vec3(m_SliderBackground.x, m_SliderBackground.y, 0.5));
		model = glm::scale(model, glm::vec3(m_SliderBackground.z, m_SliderBackground.w, 1.0));
		m_Shader->setMat4("model", model);
		m_Shader->setVec3("color", Color::Grey);
		Shader::Draw(m_Shader);
		m_Shader->setVec3("color", Color::White);
		m_Shader->setBool("plain", false);

		Shader::DrawOutline(m_SliderBackground, 1.1f);


		m_Shader->setBool("plainRGBA", true);
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(m_RightBackground.x, m_RightBackground.y, 0.8));
		model = glm::scale(model, glm::vec3(m_RightBackground.z, m_RightBackground.w, 1.0));
		m_Shader->setMat4("model", model);
		m_Shader->setVec4("colorRGBA", m_ButtonsBackgroundColor.r, m_ButtonsBackgroundColor.g, m_ButtonsBackgroundColor.b, m_RightBackgroundAlpha);
		Shader::Draw(m_Shader);


		model = glm::mat4();
		model = glm::translate(model, glm::vec3(m_LeftBackground.x, m_LeftBackground.y, 0.8));
		model = glm::scale(model, glm::vec3(m_LeftBackground.z, m_LeftBackground.w, 1.0));
		m_Shader->setMat4("model", model);
		m_Shader->setVec4("colorRGBA", m_ButtonsBackgroundColor.r, m_ButtonsBackgroundColor.g, m_ButtonsBackgroundColor.b, m_LeftBackgroundAlpha);
		Shader::Draw(m_Shader);
		m_Shader->setBool("plainRGBA", false);
	
		m_Shader->setVec4("colorRGBA", 1.f, 1.f, 1.f, 1.f);

		m_Shader->setBool("plain", true);
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(m_DefaultRect.x, m_DefaultRect.y, 0.5));
		model = glm::scale(model, glm::vec3(m_DefaultRect.z, m_DefaultRect.w, 1.0));
		m_Shader->setMat4("model", model);
		m_Shader->setVec3("color", m_DefaultButtonColor);
		Shader::Draw(m_Shader);
		m_Shader->setVec3("color", Color::White);
		m_Shader->setBool("plain", false);

		Shader::DrawOutline(m_DefaultRect, 1.1f);

		m_DefaultTextObject.DrawString(m_Shader);


		m_LabelTextObject.DrawString(m_Shader);
		
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(m_LeftDest.x, m_LeftDest.y, 0.9));
		model = glm::scale(model, glm::vec3(m_LeftDest.z, m_LeftDest.w, 1.0));
		model = glm::rotate(model, glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
		m_Shader->setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, m_RightTexture);
		Shader::Draw(m_Shader);


		model = glm::mat4();
		model = glm::translate(model, glm::vec3(m_RightDest.x, m_RightDest.y, 0.9));
		model = glm::scale(model, glm::vec3(m_RightDest.z, m_RightDest.w, 1.0));
		m_Shader->setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, m_RightTexture);
		Shader::Draw(m_Shader);

		m_ValueTextObject.DrawString(m_Shader);

	}

	void Interface::ButtonSlider::Free()
	{
		m_ValueTextObject.DeleteTexture();
		m_LabelTextObject.DeleteTexture();
		m_DefaultTextObject.DeleteTexture();

		m_LeftTexture	= NULL;
		m_RightTexture	= NULL;

		// Buttons are deleted anyway
		m_RightButton	= NULL;
		m_LeftButton	= NULL;
		m_DefaultButton = NULL;
	}

	void Interface::ButtonSlider::ReloadSliderInfo()
	{
		if (m_Value == NULL)
		{
			std::stringstream ss;
			std::string val;
			ss << std::setprecision(3) << *m_ValueF;
			ss >> val;
			m_ValueTextObject.SetTextString(val);
		}
		else
		{
			m_ValueTextObject.SetTextString(std::to_string(*m_Value));
		}
		s32 textSizeBefore = m_ValueTextObject.GetTextSize().x;
		m_ValueTextObject.ReloadTextTexture();
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

	Interface::CheckBox::CheckBox(std::string labelName, glm::vec2 pos, b8* val) : m_Value(val)
	{
		s32 textOffsetX = 5;
		m_TextString = labelName;
		m_TextPos = glm::vec2(pos.x - textOffsetX, pos.y);
		m_TextColorSDL = SDLColor::Black;
	}

	Interface::CheckBox::~CheckBox()
	{
		//Free();
	}

	void Interface::CheckBox::Init(mdShader* shader)
	{
		m_Shader = shader;
		InitTextTexture();

		s32 offsetY = 5;
		m_ButtonPos = glm::vec2(m_TextPos.x, m_TextPos.y + m_TextSize.y + offsetY);
		m_ButtonSize = glm::vec2(15, 15);

		m_CheckBoxOutline = glm::vec4((s32)m_ButtonPos.x, (s32)m_ButtonPos.y,
									  (s32)m_ButtonSize.x, (s32)m_ButtonSize.y);

		m_CheckBoxColor = Color::Orange;

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
		DrawString(m_Shader);

		glm::vec3 color;
		*m_Value == true ? color = Color::Orange : color = Color::Azure;

		glm::mat4 model;
		m_Shader->use();
		m_Shader->setBool("plain", true);
		model = glm::translate(model, glm::vec3(m_CheckBoxOutline.x, m_CheckBoxOutline.y, 0.6f));
		model = glm::scale(model, glm::vec3(m_CheckBoxOutline.z, m_CheckBoxOutline.w, 1.f));
		m_Shader->setMat4("model", model);
		m_Shader->setVec3("color", color);
		Shader::Draw(m_Shader);
		m_Shader->setBool("plain", false);

		Shader::DrawOutline(m_CheckBoxOutline, 1.1f);
	}

	void Interface::CheckBox::Free()
	{
		m_Shader = NULL;
		m_Value = NULL;
		DeleteTexture();
	}


	Interface::MusicTimeProgressObject::MusicTimeProgressObject()
	{
		SetTextColor(Color::White);
		m_FontPath = Strings::_FONT_DIGITAL_PATH;
		m_FontSize = 36;
		SetTextString("00:00");
		SetTextPos(Data::_MUSIC_TIME_PROGRESS_POS);
		m_Transparency = 0.8f;
		m_TimeReversed = false;

		/*m_BackgroundText = Text::TextObject(Color::White, m_FontSize, m_FontPath, 0.2);
		m_BackgroundText.SetTextString("888:88");*/
	}

	void Interface::MusicTimeProgressObject::Init()
	{
		InitTextTexture();
		/*SetTextPos(glm::vec2((Data::_MUSIC_TIME_PROGRESS_POS.x - this->GetTextSize().x) / 2.f,
							  Data::_MUSIC_TIME_PROGRESS_POS.y - this->GetTextSize().y / 2.f));*/

		SetTextPos(Data::_MUSIC_TIME_PROGRESS_POS);

		/*TTF_Font* font = TTF_OpenFont(m_FontPath.c_str(), m_FontSize);
		s32 w, h;
		TTF_SizeUTF8(font, "-", &w, &h);
		TTF_CloseFont(font);
		m_BackgroundText.SetTextPos(glm::vec2(GetTextPos().x - w, GetTextPos().y));
		m_BackgroundText.InitTextTexture();*/

		SetButtonPos(GetTextPos());
		SetButtonSize(GetTextSize());
	}

	void Interface::MusicTimeProgressObject::Update()
	{
		App::ProcessButton(this);
		if (this->isPressed == true && Playlist::IsPlaying() == true)
		{
			m_TimeReversed = !m_TimeReversed;
			TTF_Font* font = TTF_OpenFont(m_FontPath.c_str(), m_FontSize);
			s32 w, h;
			TTF_SizeUTF8(font, "-", &w, &h);
			TTF_CloseFont(font);
			SetTextPos(glm::vec2(GetTextPos().x - (m_TimeReversed ? (w) : (w * -1)), GetTextPos().y));
		}


		if (m_CurrentTime != (s32)Playlist::GetPosition() && Playlist::IsPlaying() == false)
		{
			if (m_TimeReversed == true)
			{
				TTF_Font* font = TTF_OpenFont(m_FontPath.c_str(), m_FontSize);
				s32 w, h;
				TTF_SizeUTF8(font, "-", &w, &h);
				TTF_CloseFont(font);
				SetTextPos(glm::vec2(GetTextPos().x + w, GetTextPos().y));
				m_TimeReversed = false;
			}

			this->SetTextString("00:00");
			this->ReloadTextTexture();
		}
		else if (m_CurrentTime != (s32)Playlist::GetPosition() || this->isPressed == true)
		{
			//md_log((s32)Playlist::GetPosition());
			m_CurrentTime = (s32)Playlist::GetPosition();
			if (m_TimeReversed == true)
			{
				this->SetTextString("-" + Converter::SecToProperTimeFormatShort(Playlist::GetMusicLength() - m_CurrentTime));
			}
			else
				this->SetTextString(Converter::SecToProperTimeFormatShort(m_CurrentTime));

			// Greater than 100 minutes
			/*if (Playlist::GetMusicLength() - m_CurrentTime > 6000 || m_CurrentTime > 6000)
			{
				this->SetTextString("99:59");
			}*/

			this->ReloadTextTexture();
		}
	}

	void Interface::MusicTimeProgressObject::Render()
	{
		this->DrawString();
		//m_BackgroundText.DrawString();
	}


	Interface::PlaylistAddButton::PlaylistAddButton() { }

	Interface::PlaylistAddButton::PlaylistAddButton(glm::vec2 pos, glm::vec2 size, GLuint tex) : 
												m_Pos(pos), m_Size(size), m_Texture(tex), m_Color(Color::Grey) 
	{ 
		m_FadeTimer = Time::Timer(500);
		m_HadFocus = false;
		m_FadeActive = false;
		m_Interp = 0.f;
	}

	void Interface::PlaylistAddButton::Update()
	{
		if (Input::hasFocus(Input::ButtonType::PlaylistAddFile))
		{
			m_Color = Color::Red * Color::Grey;
			m_HadFocus = true;
		}

		if (Input::hasFocus(Input::ButtonType::PlaylistAddFile) == false && m_HadFocus == true)
		{
			m_FadeActive = true;
			m_HadFocus = false;
			m_Interp = 0.f;
		}
		
		if (m_FadeActive == true)
		{
			m_Color = glm::vec3(Math::Lerp(Color::Red.r, Color::Grey.r, m_Interp),
								Math::Lerp(Color::Red.g, Color::Grey.g, m_Interp),
								Math::Lerp(Color::Red.b, Color::Grey.b, m_Interp));
			if (m_Interp > 1.f)
				m_FadeActive= false;
			m_Interp += 7.f * Time::deltaTime;
			//md_log(interp);
		}
		
	}
	
	void Interface::PlaylistAddButton::Render()
	{
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(Data::_PLAYLIST_ADD_BUTTON_POS, 0.6));
		model = glm::scale(model, glm::vec3(Data::_PLAYLIST_ADD_BUTTON_SIZE, 1.0));
		Shader::shaderDefault->setMat4("model", model);
		Shader::shaderDefault->setVec3("color", m_Color);
		glBindTexture(GL_TEXTURE_2D, m_Texture);
		Shader::Draw(Shader::shaderDefault);
		Shader::shaderDefault->setVec3("color", Color::White);
	}

	// ***************************
	Interface::VolumeChangedText::VolumeChangedText() { }

	Interface::VolumeChangedText::VolumeChangedText(glm::vec2 pos) : m_StartPos(pos)
	{
		m_FontSize = 14;
		SetTextColor(Color::Red);
		m_CurrentVol = Playlist::GetVolume();
		m_TextString = "Volume " + std::to_string(s32(m_CurrentVol * 100)) + "%";
		InitTextTexture();
		m_TextPos = glm::vec2(m_StartPos.x - GetTextSize().x / 2.f, m_StartPos.y);
		m_TextTimer = Time::Timer(2000);
		m_TextTimer.Start();
	}

	void Interface::VolumeChangedText::Update()
	{
		if (Playlist::GetVolume() != m_CurrentVol)
		{
			m_CurrentVol = Playlist::GetVolume();
			m_TextString = "Volume " + std::to_string(s32(m_CurrentVol * 100)) + "%";
			ReloadTextTexture();
			m_TextPos = glm::vec2(m_StartPos.x - GetTextSize().x / 2.f, m_StartPos.y);
		}


		m_TextTimer.Update();
	}

	void Interface::VolumeChangedText::Render()
	{
		DrawString(true);
	}

	void Interface::VolumeChangedText::Reset()
	{
		m_TextTimer.Stop();
		m_TextTimer.Reset();
		m_TextTimer.Start();
	}

	b8 Interface::VolumeChangedText::IsActive()
	{
		return !m_TextTimer.finished;
	}

	Interface::PlaylistSeparatorContainer*  Interface::Separator::GetContainer()
	{
		return &m_PlaylistSeparatorContainer;
	}

	std::shared_ptr<Interface::PlaylistSeparator> Interface::Separator::GetSeparator(std::string text)
	{
		if (m_PlaylistSeparatorContainer.empty() == true)
			return nullptr;

		auto it = find_if(m_PlaylistSeparatorContainer.begin(), m_PlaylistSeparatorContainer.end(),
			[&](std::pair<std::string*, std::shared_ptr<Interface::PlaylistSeparator>> const & ref) { return ref.first->compare(text) == 0; });

		if (it == m_PlaylistSeparatorContainer.end())
			return nullptr;

		return it->second;
	}

	std::shared_ptr<Interface::PlaylistSeparator> Interface::Separator::GetSeparatorByID(s32 id)
	{
		if (m_PlaylistSeparatorContainer.empty() == true)
			return nullptr;

		for (auto & i : m_PlaylistSeparatorContainer)
		{
			auto subCon = i.second->GetSubFilesContainer();
			assert(subCon->empty() == false);
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
			std::cout << i.second->GetSeparatorName();
			for(s32 k = i.second->GetSeparatorName().length(); k < maxNameLen; k++)
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
}