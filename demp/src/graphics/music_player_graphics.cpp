#include "music_player_graphics.h"

#include <iostream>
#include <algorithm>

#include <gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "../app/realtime_system_application.h"
#include "../graphics/graphics.h"
#include "../ui/music_player_ui.h"
#include "../audio/mp_audio.h"
#include "../playlist/music_player_playlist.h"
#include "../player/music_player.h"
#include "../player/music_player_state.h"
#include "../settings/music_player_string.h"
#include "../settings/music_player_settings.h"
#include "../utility/md_text.h"
#include "../utility/md_parser.h"
#include "../utility/md_math.h"
#include "../utility/md_load_texture.h"
#include "../utility/md_converter.h"


using namespace mdEngine::Graphics;
using namespace mdEngine::MP;

namespace mdEngine
{
namespace Graphics
{
	namespace MP
	{
		PlaylistObject m_Playlist;
		MainPlayerObject m_MainPlayer;
		Interface::Button* m_PlaylistBarSlider;
		Interface::Button* m_AddFileButtonRef;
		Interface::TextBox m_AddFileTextBox;

		GLuint main_background = 0, main_foreground = 0;
		GLuint exit_background, exit_icon, minimize_icon, stay_on_top_icon;
		GLuint volume_bar, volume_speaker, volume_speaker_muted;
		GLuint play_button, stop_button, next_button, previous_button, shuffle_button, repeat_button, dot_icon, playlist_button,
			   playlist_add_file;
		GLuint music_progress_bar;


		SeparatorsToRender playlistSeparatorToRenderVec;

		std::vector<s32> playlistSeparatorsIDsVec;


		//std::vector<s32> indexesToRenderVec;

		b8 texturesLoaded = false;
		GLuint* textTexture;
		GLuint* separatorTextTexture;
		glm::vec2* predefinedPos;
		b8 predefinedPosLoaded(false);

		s32 allocatedTextTexturesCount = 0;
		s32 allocatedSeparatorTexturesCount = 0;
		s32 maxPos, minPos;
		s32 maxPosTest, minPosTest;

		b8 shuffleActive(false);
		b8 repeatActive(false);
		b8 playActive(false);
		b8 volumeMuted(false);
		b8 volumeSliderActive(false);
		b8 musicSliderActive(false);
		b8 playlistSliderActive(false);
		b8 playlistActive(false);
		b8 playlistAddFileActive(false);
		b8 playlistAddFileButtonPressed(false);

		b8 playlistToggled(false);

		b8 playlistFirstEnter(true);

		b8 playlistFirstLoad(true);

		b8 playlistWasEmpty(false);

		b8 scrollBarAtBottom(false);


		f32 playlistPositionOffset;
		f32 playlistPositionOffsetPrevious;
		MP::PlaylistMovement playlistMove;
		s32 playlistCursorPosition;
		s32 minPosToRender, maxPosToRender;
		s32 mouseXPosPrevious, mouseYPosPrevious;
		s32 sliderYPosPrevious;
		b8 scrollBarMaxBoundsCrossed(false);
		s32 playlistPositionOffsetBeforeCross;


		Text::TextObject durationText;
		Text::TextObject itemsSizeText;
		Text::TextObject itemsCountText;
		Text::TextObject loadedItemsCountText;

		
		b8 updatePlaylistInfo(false);
		b8 updatePlaylistInfoPrevious(false);

		s32 hiddenItemsOffsetY;
		s32 hiddenItemsOffsetYSum;
		s32 hiddenItemsCount;

		f32 playlistPreviousOffsetY = 0;
		f32 playlistCurrentOffsetY = 0;
		s32 playlistCurrentPos = 0;

		s32 playlistSeparatorsOffset;

		s32 playlistPreviousPos = 0;
		s32 playlistIndex = 0;

		s32 currentlyRenderedItems = 0;

		s32 lastMousePos = 0;
		s32 deltaVolumePos = 0;
		s32 deltaMusicPos = 0;
		
		// Scroll bar
		b8 playlistSliderFirstEnter(true);
		s32 mouseX, mouseY;
		s32 lastMouseX = 0, lastMouseY = 0;
		s32 deltaMouseY = 0;
			
		f32 interp = 2.f;

		void InitializeConfig();

		void InitializeText();

		void InitializeTextBoxes();

		void RenderPlaylistWindow();

		void RenderVolume();

		void RenderMusicProgressBar();

		void RenderMusicUI();

		void RenderScrollBar(f32* playlistOffset, f32 displayedItems, f32 maxItems);

		void RenderPlaylistItems();

		void RenderPlaylistItemsNew();

		void RenderPlaylistButtons();

		void RenderSettingsButtons();

		void RenderWindowControlButtons();

		void RenderPlaylistInfo();

	}

	MP::PlaylistObject::PlaylistObject()
	{
		m_Enabled = false;
		m_Toggled = false;
		m_SelectedID = -1;
		m_PlayingID = -1;
		m_CurrentMinIndex = 0;
		m_CurrentMaxIndex = 0;
	}

	void MP::PlaylistObject::Enable()
	{
		m_Enabled = !m_Enabled;
	}

	void MP::PlaylistObject::Toggle()
	{
		m_Toggled = true;
	}

	void MP::PlaylistObject::UnToggle()
	{
		m_Toggled = false;
	}

	b8 MP::PlaylistObject::IsEnabled() const
	{
		return m_Enabled;
	}

	b8 MP::PlaylistObject::IsToggled() const
	{
		return m_Toggled;
	}

	void MP::PlaylistObject::SetSelectedID(s32 id)
	{
		m_SelectedID = id;
	}

	void MP::PlaylistObject::SetPlayingID(s32 id)
	{
		m_PlayingID = id;
	}

	s32 MP::PlaylistObject::GetSelectedID() const
	{
		return m_SelectedID;
	}

	s32 MP::PlaylistObject::GetPlayingID() const
	{
		return mdEngine::MP::Playlist::RamLoadedMusic.get() != NULL ? mdEngine::MP::Playlist::RamLoadedMusic.mID : -1;
	}

	glm::vec2 MP::PlaylistObject::GetPos() const
	{
		return m_Pos;
	}

	glm::vec2 MP::PlaylistObject::GetSize() const
	{
		return m_Size;
	}

	void MP::PlaylistObject::SetPos(glm::vec2 pos)
	{
		m_Pos = pos;
	}

	void MP::PlaylistObject::SetSize(glm::vec2 size)
	{
		m_Size = size;
	}

	void MP::PlaylistObject::SetItemsSize(f64 itemsSize)
	{
		m_ItemsSize = itemsSize;
		m_ItemsSizeStr = Converter::BytesToProperSizeFormat(itemsSize);
	}

	void MP::PlaylistObject::SetItemsDuration(f64 itemsDuration)
	{
		m_ItemsDuration = itemsDuration;
		m_ItemsDurationStr = Converter::SecToProperTimeFormat(itemsDuration);
	}

	void MP::PlaylistObject::SetIndexesToRender(std::vector<s32> indexesVec)
	{
		indexesToRender = indexesVec;
	}

	void MP::PlaylistObject::SetCurrentMinIndex(s32 min)
	{
		m_CurrentMinIndex = min;
	}

	void MP::PlaylistObject::SetCurrentMaxIndex(s32 max)
	{
		m_CurrentMaxIndex = max;
	}

	void MP::PlaylistObject::SetCurrentOffset(s32 offset)
	{
		m_CurrentOffset = offset;
	}

	void MP::PlaylistObject::SetHiddenSeparatorCount(s32 count)
	{
		m_HiddenSeparatorCount = count;
	}

	f64 MP::PlaylistObject::GetItemsSize() const
	{
		return m_ItemsSize;
	}

	f64 MP::PlaylistObject::GetItemsDuration() const
	{
		return m_ItemsDuration;
	}

	std::string MP::PlaylistObject::GetItemsSizeString() const
	{
		return m_ItemsSizeStr;
	}

	std::string MP::PlaylistObject::GetItemsDurationString() const
	{
		return m_ItemsDurationStr;
	}

	std::vector<s32> MP::PlaylistObject::GetIndexesToRender() const
	{
		return indexesToRender;
	}

	/*s32 MP::PlaylistObject::GetCurrentMinIndex() const
	{
		return m_CurrentMinIndex;
	}

	s32 MP::PlaylistObject::GetCurrentMaxIndex() const
	{
		return m_CurrentMaxIndex;
	}*/

	s32 MP::PlaylistObject::GetCurrentOffset() const
	{
		return m_CurrentOffset;
	}

	s32  MP::PlaylistObject::GetHiddenSeparatorCount()  const
	{
		return m_HiddenSeparatorCount;
	}

	b8 MP::PlaylistObject::hasFocus() const
	{
		int mouseX, mouseY;
		App::Input::GetMousePosition(&mouseX, &mouseY);

		return mouseX > m_Pos.x && mouseX < m_Size.x &&
			   mouseY > m_Pos.y && mouseY < m_Size.y;
	}

	MP::PlaylistObject* MP::GetPlaylistObject()
	{
		return &m_Playlist;
	}

	b8 MP::MainPlayerObject::hasFocus()
	{
		int mouseX, mouseY;
		App::Input::GetMousePosition(&mouseX, &mouseY);

		return mouseX > m_Pos.x && mouseX < m_Size.x &&
			mouseY > m_Pos.y && mouseY < m_Size.y;
	}

	void MP::MainPlayerObject::SetPos(glm::vec2 pos)
	{
		m_Pos = pos;
	}

	void MP::MainPlayerObject::SetSize(glm::vec2 size)
	{
		m_Size = size;
	}

	MP::MainPlayerObject* MP::GetMainPlayerObject()
	{
		return &m_MainPlayer;
	}

	MP::SeparatorsToRender* MP::GetSeparatorsToRender()
	{
		return &playlistSeparatorToRenderVec;
	}


	void MP::InitializeText()
	{
		durationText			= Text::TextObject(Data::_MUSIC_PLAYER_NUMBER_FONT, Color::Grey);
		itemsSizeText			= Text::TextObject(Data::_MUSIC_PLAYER_NUMBER_FONT, Color::Grey);
		itemsCountText			= Text::TextObject(Data::_MUSIC_PLAYER_NUMBER_FONT, Color::Grey);
		loadedItemsCountText	= Text::TextObject(Data::_MUSIC_PLAYER_NUMBER_FONT, Color::Grey);

		durationText.SetTextPos(Data::_TEXT_ITEMS_DURATION_POS);
		itemsSizeText.SetTextPos(Data::_TEXT_ITEMS_SIZE_POS);
		itemsCountText.SetTextPos(Data::_TEXT_ITEMS_COUNT_POS);
		loadedItemsCountText.SetTextPos(Data::_TEXT_LOADED_ITEMS_COUNT_POS);

	}

	void MP::InitializeTextBoxes()
	{
		m_AddFileTextBox = Interface::TextBox(Input::ButtonType::PlaylistAddFileTextBox, 
											Data::_PLAYLIST_ADD_BUTTON_TEXTBOX_SIZE,
											glm::vec2(Data::_PLAYLIST_ADD_BUTTON_TEXTBOX_POS.x,
													  Data::_PLAYLIST_ADD_BUTTON_TEXTBOX_POS.y +
													  Data::_PLAYLIST_ADD_BUTTON_SIZE.y),
											Shader::shaderDefault);
		m_AddFileTextBox.SetItemScale(1.f);
		m_AddFileTextBox.AddItem(Strings::_PLAYLIST_ADD_FILE);
		m_AddFileTextBox.AddItem(Strings::_PLAYLIST_ADD_FOLDER);
		m_AddFileTextBox.AddItem(L"hehe");
		m_AddFileTextBox.SetColor(Color::Grey);
	}

	void MP::InitializeConfig()
	{
		std::string file = Strings::_SETTINGS_FILE;
		shuffleActive = Parser::GetInt(file, Strings::_SHUFFLE_STATE);
		repeatActive = Parser::GetInt(file, Strings::_REPEAT_STATE);
		Parser::GetInt(file, Strings::_PLAYLIST_STATE) == 1 ? (m_Playlist.Toggle(), m_Playlist.Enable()) : m_Playlist.UnToggle();

		file = Strings::_PATHS_FILE;
		m_Playlist.SetItemsDuration(Parser::GetFloat(file, Strings::_CONTENT_DURATION));
		m_Playlist.SetItemsSize(Parser::GetFloat(file, Strings::_CONTENT_SIZE));


		State::SetState(State::AudioAdded);
	}

	void MP::RenderPlaylistWindow()
	{
		static s32 toggledWindowHeight = Window::windowProperties.mApplicationHeight;

		m_Playlist.SetPos(glm::vec2(0.f, Data::_PLAYLIST_FOREGROUND_POS.y));
		m_Playlist.SetSize(glm::vec2(Window::windowProperties.mWindowWidth, Window::windowProperties.mApplicationHeight));
		m_MainPlayer.SetPos(glm::vec2(0.f, 0.f));
		m_MainPlayer.SetSize(Data::_DEFAULT_PLAYER_SIZE);

		// If playlist button pressed, roll down/up playlist
		if (Input::isButtonPressed(Input::ButtonType::Playlist) && interp > 1.f)
		{
			interp = 0.f;

			m_Playlist.Enable();
		}
		// Playlist rolling down
		if (m_Playlist.IsEnabled() && !m_Playlist.IsToggled())
		{
			Data::_MAIN_BACKGROUND_SIZE.y = Math::Lerp(Data::_DEFAULT_PLAYER_SIZE.y, Window::windowProperties.mApplicationHeight, interp);
			interp += (Data::PlaylistRollMultiplier * 10.f / toggledWindowHeight) * Time::deltaTime;
			if (interp > 1.f)
				m_Playlist.Toggle();
		}

		// Playlist rolling up
		if (!m_Playlist.IsEnabled() && m_Playlist.IsToggled())
		{
			Data::_MAIN_BACKGROUND_SIZE.y = Math::Lerp(Window::windowProperties.mApplicationHeight, Data::_DEFAULT_PLAYER_SIZE.y, interp);
			interp += (Data::PlaylistRollMultiplier * 10.f / toggledWindowHeight) * Time::deltaTime;
			if (interp > 1.f)
				m_Playlist.UnToggle();
		}

		// Playlist is enabled
		if (m_Playlist.IsToggled() && m_Playlist.IsEnabled())
		{
			Data::_MAIN_BACKGROUND_SIZE.y = Window::windowProperties.mApplicationHeight;
			toggledWindowHeight = Window::windowProperties.mApplicationHeight;
		}


		/* Main background*/
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(Data::_MAIN_BACKGROUND_POS.x, Data::_MAIN_BACKGROUND_POS.y, 0.f));
		model = glm::scale(model, glm::vec3(Data::_MAIN_BACKGROUND_SIZE.x, Data::_MAIN_BACKGROUND_SIZE.y, 1.f));;
		Shader::shaderDefault->setMat4("model", model);
		Shader::shaderDefault->setBool("cut", true);
		glBindTexture(GL_TEXTURE_2D, main_background);
		Shader::Draw(Shader::shaderDefault);
		Shader::shaderDefault->setBool("cut", false);

		// Main foreground
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(Data::_MAIN_FOREGROUND_POS, 0.1f));
		model = glm::scale(model, glm::vec3(Data::_MAIN_FOREGROUND_SIZE, 1.f));
		Shader::shaderDefault->setMat4("model", model);
		Shader::shaderDefault->setBool("cut", true);
		glBindTexture(GL_TEXTURE_2D, main_foreground);
		Shader::Draw(Shader::shaderDefault);;
		Shader::shaderDefault->setBool("cut", false);

		// Playlist foreground(render when playlist toggled)
		if (m_Playlist.IsToggled() && m_Playlist.IsEnabled())
		{
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(Data::_PLAYLIST_FOREGROUND_POS, 0.4f));
			model = glm::scale(model, glm::vec3(Data::_PLAYLIST_FOREGROUND_SIZE, 1.f));
			Shader::shaderDefault->setMat4("model", model);
			Shader::shaderDefault->setBool("cut", true);
			glBindTexture(GL_TEXTURE_2D, main_foreground);
			Shader::Draw(Shader::shaderDefault);
			Shader::shaderDefault->setBool("cut", false);
		}
		


		// Playlist button
		if (Input::hasFocus(Input::ButtonType::Playlist))
		{
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(Data::_PLAYLIST_BUTTON_POS, 0.4));
			lastMousePos = deltaVolumePos;
			model = glm::scale(model, glm::vec3(Data::_PLAYLIST_BUTTON_SIZE, 1.f));
			Shader::shaderDefault->setMat4("model", model);
			glBindTexture(GL_TEXTURE_2D, playlist_button);
			Shader::Draw(Shader::shaderDefault);
		}
		else
		{
			Shader::shaderDefault->setVec3("color", Color::Grey);
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(Data::_PLAYLIST_BUTTON_POS, 0.4));
			lastMousePos = deltaVolumePos;
			model = glm::scale(model, glm::vec3(Data::_PLAYLIST_BUTTON_SIZE, 1.f));
			Shader::shaderDefault->setMat4("model", model);
			glBindTexture(GL_TEXTURE_2D, playlist_button);
			Shader::Draw(Shader::shaderDefault);
			Shader::shaderDefault->setVec3("color", Color::White);

		}
	}

	void MP::RenderVolume()
	{
		glm::mat4 model;
		/* Volume bar */
		{
			// TODO: When mouse pressed on dot, add concave shape inside dot
			//		 Add speaker level of volume (1 line, 2 lines etc.)

			// Calculate current slider pos basing on current volume in range (0 - volume_bar_width)
			deltaVolumePos = (s32)(mdEngine::MP::Playlist::GetVolume() * (float)Data::_VOLUME_BAR_SIZE.x);

			if (Input::isButtonPressed(Input::ButtonType::Volume))
				volumeMuted = !volumeMuted;

			if (Input::hasFocus(Input::ButtonType::SliderVolume) &&
				App::Input::IsKeyDown(App::KeyCode::MouseLeft) &&
				musicSliderActive == false &&
				State::CheckState(State::Window::Resized) == false)
			{
				volumeSliderActive = true;
			}
			else if (App::Input::IsKeyDown(App::KeyCode::MouseLeft) == false)
			{
				volumeSliderActive = false;
			}


			if (volumeSliderActive == true)
			{
				s32 mouseX, mouseY;
				App::Input::GetMousePosition(&mouseX, &mouseY);
				deltaVolumePos = mouseX - Data::_VOLUME_BAR_POS.x;
				if (deltaVolumePos > Data::_VOLUME_BAR_SIZE.x)
					deltaVolumePos = Data::_VOLUME_BAR_SIZE.x;
				if (deltaVolumePos < 0)
					deltaVolumePos = 0;

				mdEngine::MP::Playlist::SetVolume(deltaVolumePos / (float)Data::_VOLUME_BAR_SIZE.x);
			}

			/* xD */
			if (volumeMuted == true || deltaVolumePos <= 0 && Input::hasFocus(Input::ButtonType::Volume))
			{
				mdEngine::MP::Playlist::MuteVolume(true);
				model = glm::mat4();
				model = glm::translate(model, glm::vec3(Data::_VOLUME_SPEAKER_POS, 0.3f));
				model = glm::scale(model, glm::vec3(Data::_VOLUME_SPEAKER_SIZE, 1.f));
				Shader::shaderDefault->setMat4("model", model);
				glBindTexture(GL_TEXTURE_2D, volume_speaker_muted);
				Shader::Draw(Shader::shaderDefault);
			}
			else if (volumeMuted || deltaVolumePos <= 0)
			{
				Shader::shaderDefault->setVec3("color", Color::Grey);
				mdEngine::MP::Playlist::MuteVolume(true);
				model = glm::mat4();
				model = glm::translate(model, glm::vec3(Data::_VOLUME_SPEAKER_POS, 0.3f));
				model = glm::scale(model, glm::vec3(Data::_VOLUME_SPEAKER_SIZE, 1.f));
				Shader::shaderDefault->setMat4("model", model);
				glBindTexture(GL_TEXTURE_2D, volume_speaker_muted);
				Shader::Draw(Shader::shaderDefault);
				Shader::shaderDefault->setVec3("color", Color::White);
			}
			else if (volumeMuted == false && Input::hasFocus(Input::ButtonType::Volume))
			{
				mdEngine::MP::Playlist::MuteVolume(false);
				model = glm::mat4();
				model = glm::translate(model, glm::vec3(Data::_VOLUME_SPEAKER_POS, 0.3f));
				model = glm::scale(model, glm::vec3(Data::_VOLUME_SPEAKER_SIZE, 1.f));
				Shader::shaderDefault->setMat4("model", model);
				glBindTexture(GL_TEXTURE_2D, volume_speaker);
				Shader::Draw(Shader::shaderDefault);
			}
			else if (volumeMuted == false)
			{
				Shader::shaderDefault->setVec3("color", Color::Grey);
				mdEngine::MP::Playlist::MuteVolume(false);
				model = glm::mat4();
				model = glm::translate(model, glm::vec3(Data::_VOLUME_SPEAKER_POS, 0.3f));
				model = glm::scale(model, glm::vec3(Data::_VOLUME_SPEAKER_SIZE, 1.f));
				Shader::shaderDefault->setMat4("model", model);
				glBindTexture(GL_TEXTURE_2D, volume_speaker);
				Shader::Draw(Shader::shaderDefault);
				Shader::shaderDefault->setVec3("color", Color::White);
			}


			/* Volume bar beneath volume slider */
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(Data::_VOLUME_BAR_POS, 0.3f));
			model = glm::scale(model, glm::vec3(Data::_VOLUME_BAR_SIZE, 1.f));
			Shader::shaderDefault->setMat4("model", model);
			Shader::shaderDefault->setVec3("color", Color::DarkGrey);
			glBindTexture(GL_TEXTURE_2D, volume_bar);
			Shader::Draw(Shader::shaderDefault);
			Shader::shaderDefault->setVec3("color", Color::White);


			if (Input::hasFocus(Input::ButtonType::SliderVolume) || volumeSliderActive == true)
			{
				if (volumeMuted == true)
				{
					model = glm::mat4();
					model = glm::translate(model, glm::vec3(Data::_VOLUME_BAR_POS.x - Data::_SLIDER_DOT_SIZE.x / 2.f, Data::_VOLUME_BAR_DOT_POS.y, 0.4));
					if (lastMousePos != deltaVolumePos)
						volumeMuted = false;
				}
				else
				{
					model = glm::mat4();
					model = glm::translate(model, glm::vec3(Data::_VOLUME_BAR_POS.x + deltaVolumePos - Data::_SLIDER_DOT_SIZE.x / 2.f, Data::_VOLUME_BAR_DOT_POS.y, 0.4));
					lastMousePos = deltaVolumePos;
				}

				model = glm::scale(model, glm::vec3(Data::_SLIDER_DOT_SIZE, 1.f));
				Shader::shaderDefault->setMat4("model", model);
				glBindTexture(GL_TEXTURE_2D, dot_icon);
				Shader::Draw(Shader::shaderDefault);

				model = glm::mat4();
				model = glm::translate(model, glm::vec3(Data::_VOLUME_BAR_POS, 0.35f));
				model = glm::scale(model, glm::vec3(deltaVolumePos, Data::_VOLUME_BAR_SIZE.y, 1.f));
				Shader::shaderDefault->setMat4("model", model);
				Shader::shaderDefault->setVec3("color", Color::Green);
				glBindTexture(GL_TEXTURE_2D, volume_bar);
				Shader::Draw(Shader::shaderDefault);
				Shader::shaderDefault->setVec3("color", Color::White);
			}
			else if (volumeMuted == false)
			{
				model = glm::mat4();
				model = glm::translate(model, glm::vec3(Data::_VOLUME_BAR_POS, 0.35f));
				model = glm::scale(model, glm::vec3(deltaVolumePos, Data::_VOLUME_BAR_SIZE.y, 1.f));
				Shader::shaderDefault->setMat4("model", model);
				Shader::shaderDefault->setVec3("color", Color::Grey);
				glBindTexture(GL_TEXTURE_2D, volume_bar);
				Shader::Draw(Shader::shaderDefault);
				Shader::shaderDefault->setVec3("color", Color::White);
			}
		}
	}

	void MP::RenderMusicProgressBar()
	{
		glm::mat4 model;
		f32 musicPos = mdEngine::MP::Playlist::GetPosition();
		f32 musicLength = mdEngine::MP::Playlist::GetMusicLength();

		if (musicPos < 0 || musicLength < 0)
			deltaMusicPos = 0;
		else
			deltaMusicPos = musicPos / musicLength * Data::_MUSIC_PROGRESS_BAR_SIZE.x;

		if (musicSliderActive == true)
		{
			s32 mouseX, mouseY;
			App::Input::GetMousePosition(&mouseX, &mouseY);
			deltaMusicPos = mouseX - Data::_MUSIC_PROGRESS_BAR_POS.x;
			if (deltaMusicPos > Data::_MUSIC_PROGRESS_BAR_SIZE.x)
				deltaMusicPos = Data::_MUSIC_PROGRESS_BAR_SIZE.x;
			if (deltaMusicPos < 0)
				deltaMusicPos = 0;

			if (App::Input::IsKeyDown(App::KeyCode::MouseLeft) == false)
				mdEngine::MP::Playlist::SetPosition(deltaMusicPos / (float)Data::_MUSIC_PROGRESS_BAR_SIZE.x * mdEngine::MP::Playlist::GetMusicLength());
		}

		if (Input::hasFocus(Input::ButtonType::SliderMusic) &&
			App::Input::IsKeyDown(App::KeyCode::MouseLeft) &&
			volumeSliderActive == false &&
			State::CheckState(State::Window::Resized) == false)
		{
			musicSliderActive = true;
		}
		else if (App::Input::IsKeyDown(App::KeyCode::MouseLeft) == false)
		{
			musicSliderActive = false;
		}

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(Data::_MUSIC_PROGRESS_BAR_POS, 0.3f));
		model = glm::scale(model, glm::vec3(Data::_MUSIC_PROGRESS_BAR_SIZE, 1.f));
		Shader::shaderDefault->setMat4("model", model);
		Shader::shaderDefault->setVec3("color", Color::DarkGrey);
		glBindTexture(GL_TEXTURE_2D, music_progress_bar);
		Shader::Draw(Shader::shaderDefault);
		Shader::shaderDefault->setVec3("color", Color::White);

		if (Input::hasFocus(Input::ButtonType::SliderMusic) || musicSliderActive)
		{
			Shader::shaderDefault->setVec3("color", Color::Green);
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(Data::_MUSIC_PROGRESS_BAR_POS, 0.35f));
			model = glm::scale(model, glm::vec3(deltaMusicPos, Data::_MUSIC_PROGRESS_BAR_SIZE.y, 1.f));
			Shader::shaderDefault->setMat4("model", model);
			glBindTexture(GL_TEXTURE_2D, music_progress_bar);
			Shader::Draw(Shader::shaderDefault);
			Shader::shaderDefault->setVec3("color", Color::White);


			model = glm::mat4();
			model = glm::translate(model, glm::vec3(Data::_MUSIC_PROGRESS_BAR_POS.x + deltaMusicPos - Data::_SLIDER_DOT_SIZE.x / 2.f, Data::_MUSIC_PROGRESS_BAR_DOT_POS.y, 0.4));
			lastMousePos = deltaVolumePos;
			model = glm::scale(model, glm::vec3(Data::_SLIDER_DOT_SIZE, 1.f));
			Shader::shaderDefault->setMat4("model", model);
			glBindTexture(GL_TEXTURE_2D, dot_icon);
			Shader::Draw(Shader::shaderDefault);


		}
		else
		{
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(Data::_MUSIC_PROGRESS_BAR_POS, 0.35f));
			model = glm::scale(model, glm::vec3(deltaMusicPos, Data::_MUSIC_PROGRESS_BAR_SIZE.y, 1.f));
			Shader::shaderDefault->setMat4("model", model);
			glBindTexture(GL_TEXTURE_2D, music_progress_bar);
			Shader::Draw(Shader::shaderDefault);
		}
	}

	void MP::RenderMusicUI()
	{
		glm::mat4 model;
		f32 dotOffsetXShuffle = 0.4f;
		f32 dotOffsetXRepeat = -0.2f;
		/*  Music UI*/
	
		f32 focusMultiplier;

		// Shuffle 
		glm::mat4 dotModel;
		model = glm::mat4();

		if (Input::isButtonPressed(Input::ButtonType::Shuffle))
			shuffleActive = !shuffleActive;
		glm::vec3 color;
		if (Input::hasFocus(Input::ButtonType::Shuffle) && shuffleActive == false)
		{
			focusMultiplier = 1.02f;
			color = Color::White;
			model = glm::translate(model, glm::vec3(Data::_SHUFFLE_BUTTON_POS.x + (Data::_SHUFFLE_BUTTON_SIZE.x / 2.f) * (1.f - focusMultiplier),
				Data::_SHUFFLE_BUTTON_POS.y + (Data::_SHUFFLE_BUTTON_SIZE.y / 2.f) * (1.f - focusMultiplier), 0.3f));
			model = glm::scale(model, glm::vec3(Data::_SHUFFLE_BUTTON_SIZE * focusMultiplier, 1.f));

		}
		else if (shuffleActive == false)
		{
			color = Color::Grey;
			model = glm::translate(model, glm::vec3(Data::_SHUFFLE_BUTTON_POS, 0.3f));
			model = glm::scale(model, glm::vec3(Data::_SHUFFLE_BUTTON_SIZE, 1.f));;
		}
		else
		{
			color = Color::Green;
			model = glm::translate(model, glm::vec3(Data::_SHUFFLE_BUTTON_POS, 0.3f));
			model = glm::scale(model, glm::vec3(Data::_SHUFFLE_BUTTON_SIZE, 1.f));;

			dotModel = glm::translate(dotModel, glm::vec3(Data::_SHUFFLE_BUTTON_POS.x + Data::_SHUFFLE_BUTTON_SIZE.x / 2.f - Data::_DOT_BUTTON_STATE_SIZE.x / 2.f - dotOffsetXShuffle, 
														  Data::_SHUFFLE_BUTTON_POS.y + 18.f, 0.3f));
			dotModel = glm::scale(dotModel, glm::vec3(Data::_DOT_BUTTON_STATE_SIZE, 1.f));
			Shader::shaderDefault->setVec3("color", color);
			Shader::shaderDefault->setMat4("model", dotModel);
			glBindTexture(GL_TEXTURE_2D, dot_icon);
			Shader::Draw(Shader::shaderDefault);

		}
		Shader::shaderDefault->setVec3("color", color);
		Shader::shaderDefault->setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, shuffle_button);
		Shader::Draw(Shader::shaderDefault);


		// Previous
		model = glm::mat4();
		if (Input::hasFocus(Input::ButtonType::Previous))
		{
			focusMultiplier = 1.02f;
			color = Color::White;
			model = glm::translate(model, glm::vec3(Data::_PREVIOUS_BUTTON_POS.x + (Data::_PREVIOUS_BUTTON_SIZE.x / 2.f) * (1.f - focusMultiplier),
				Data::_PREVIOUS_BUTTON_POS.y + (Data::_PREVIOUS_BUTTON_SIZE.y / 2.f) * (1.f - focusMultiplier), 0.3f));
			model = glm::scale(model, glm::vec3(Data::_PREVIOUS_BUTTON_SIZE * focusMultiplier, 1.f));

		}
		else
		{
			color = Color::Grey;
			model = glm::translate(model, glm::vec3(Data::_PREVIOUS_BUTTON_POS, 0.3f));
			model = glm::scale(model, glm::vec3(Data::_PREVIOUS_BUTTON_SIZE, 1.f));;
		}
		Shader::shaderDefault->setVec3("color", color);
		Shader::shaderDefault->setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, previous_button);
		Shader::Draw(Shader::shaderDefault);


		// Play
		if (Input::isButtonPressed(Input::ButtonType::Play) && mdEngine::MP::Playlist::IsLoaded() == true)
			playActive = !playActive;

		if (Input::isButtonPressed(Input::ButtonType::Previous) || Input::isButtonPressed(Input::ButtonType::Next) &&
			playActive == false)
			playActive = true;

		model = glm::mat4();
		if (Input::hasFocus(Input::ButtonType::Play) && playActive == false)
		{
			focusMultiplier = 1.07f;
			color = Color::White;
			model = glm::translate(model, glm::vec3(Data::_PLAY_BUTTON_POS.x + (Data::_PLAY_BUTTON_SIZE.x / 2.f) * (1.f - focusMultiplier),
				Data::_PLAY_BUTTON_POS.y + (Data::_PLAY_BUTTON_SIZE.y / 2.f) * (1.f - focusMultiplier), 0.3f));
			model = glm::scale(model, glm::vec3(Data::_PLAY_BUTTON_SIZE * focusMultiplier, 1.f));

		}
		else if (!Input::hasFocus(Input::ButtonType::Play) && playActive == false)
		{
			color = Color::Grey;
			model = glm::translate(model, glm::vec3(Data::_PLAY_BUTTON_POS, 0.3f));
			model = glm::scale(model, glm::vec3(Data::_PLAY_BUTTON_SIZE, 1.f));;
		}

		if (playActive == false)
		{
			Shader::shaderDefault->setVec3("color", color);

			Shader::shaderDefault->setMat4("model", model);
			glBindTexture(GL_TEXTURE_2D, play_button);
			Shader::Draw(Shader::shaderDefault);
		}

		// Stop



		model = glm::mat4();
		if (Input::hasFocus(Input::ButtonType::Stop) && playActive == true)
		{
			focusMultiplier = 1.07f;
			color = Color::White;
			model = glm::translate(model, glm::vec3(Data::_PLAY_BUTTON_POS.x + (Data::_PLAY_BUTTON_SIZE.x / 2.f) * (1.f - focusMultiplier),
				Data::_PLAY_BUTTON_POS.y + (Data::_PLAY_BUTTON_SIZE.y / 2.f) * (1.f - focusMultiplier), 0.3f));
			model = glm::scale(model, glm::vec3(Data::_PLAY_BUTTON_SIZE * focusMultiplier, 1.f));

		}
		else if (!Input::hasFocus(Input::ButtonType::Stop) && playActive == true)
		{
			color = Color::Grey;
			model = glm::translate(model, glm::vec3(Data::_PLAY_BUTTON_POS, 0.3f));
			model = glm::scale(model, glm::vec3(Data::_PLAY_BUTTON_SIZE, 1.f));;
		}

		if (playActive)
		{
			Shader::shaderDefault->setVec3("color", color);

			Shader::shaderDefault->setMat4("model", model);
			glBindTexture(GL_TEXTURE_2D, stop_button);
			Shader::Draw(Shader::shaderDefault);
		}


		// Next
		model = glm::mat4();
		if (Input::hasFocus(Input::ButtonType::Next))
		{
			focusMultiplier = 1.02f;
			color = Color::White;
			model = glm::translate(model, glm::vec3(Data::_NEXT_BUTTON_POS.x + (Data::_NEXT_BUTTON_SIZE.x / 2.f) * (1.f - focusMultiplier),
				Data::_NEXT_BUTTON_POS.y + (Data::_NEXT_BUTTON_SIZE.y / 2.f) * (1.f - focusMultiplier), 0.3f));
			model = glm::scale(model, glm::vec3(Data::_NEXT_BUTTON_SIZE * focusMultiplier, 1.f));

		}
		else
		{
			color = Color::Grey;
			model = glm::translate(model, glm::vec3(Data::_NEXT_BUTTON_POS, 0.3f));
			model = glm::scale(model, glm::vec3(Data::_NEXT_BUTTON_SIZE, 1.f));;
		}
		Shader::shaderDefault->setVec3("color", color);
		Shader::shaderDefault->setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, next_button);
		Shader::Draw(Shader::shaderDefault);


		// Repeat
		if (Input::isButtonPressed(Input::ButtonType::Repeat))
			repeatActive = !repeatActive;

		model = glm::mat4();
		if (Input::hasFocus(Input::ButtonType::Repeat) && repeatActive == false)
		{
			focusMultiplier = 1.02f;
			color = Color::White;
			model = glm::translate(model, glm::vec3(Data::_REPEAT_BUTTON_POS.x + (Data::_REPEAT_BUTTON_SIZE.x / 2.f) * (1.f - focusMultiplier),
				Data::_REPEAT_BUTTON_POS.y + (Data::_REPEAT_BUTTON_SIZE.y / 2.f) * (1.f - focusMultiplier), 0.3f));
			model = glm::scale(model, glm::vec3(Data::_REPEAT_BUTTON_SIZE * focusMultiplier, 1.f));

		}
		else if (repeatActive == false)
		{
			color = Color::Grey;
			model = glm::translate(model, glm::vec3(Data::_REPEAT_BUTTON_POS, 0.3f));
			model = glm::scale(model, glm::vec3(Data::_REPEAT_BUTTON_SIZE, 1.f));;
		}
		else
		{
			color = Color::Green;
			model = glm::translate(model, glm::vec3(Data::_REPEAT_BUTTON_POS, 0.3f));
			model = glm::scale(model, glm::vec3(Data::_REPEAT_BUTTON_SIZE, 1.f));

			dotModel = glm::mat4();
			dotModel = glm::translate(dotModel, glm::vec3(Data::_REPEAT_BUTTON_POS.x + Data::_REPEAT_BUTTON_SIZE.x / 2.f - Data::_DOT_BUTTON_STATE_SIZE.x / 2.f - dotOffsetXRepeat, 
									  Data::_REPEAT_BUTTON_POS.y + 18.f, 0.3f));
			dotModel = glm::scale(dotModel, glm::vec3(Data::_DOT_BUTTON_STATE_SIZE, 1.f));
			Shader::shaderDefault->setVec3("color", color);
			Shader::shaderDefault->setMat4("model", dotModel);
			glBindTexture(GL_TEXTURE_2D, dot_icon);
			Shader::Draw(Shader::shaderDefault);
		}
		Shader::shaderDefault->setVec3("color", color);
		Shader::shaderDefault->setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, repeat_button);
		Shader::Draw(Shader::shaderDefault);

		Shader::shaderDefault->setVec3("color", Color::White);		
	}

	void MP::RenderPlaylistInfo()
	{
		if (m_Playlist.IsToggled() &&
			m_Playlist.IsEnabled())
		{
			
			durationText.DrawString();
			itemsCountText.DrawString();
			itemsSizeText.DrawString();

			if (State::CheckState(State::FilesInfoLoaded) == false)
			{
				//md_log(utf16_to_utf8(Audio::Info::GetProcessedItemsCountStr()));
				loadedItemsCountText.SetTextString(Audio::Info::GetProcessedItemsCountStr());
				loadedItemsCountText.ReloadTextTexture();
				loadedItemsCountText.DrawString();
			}

		}
		
	}

	void MP::RenderScrollBar(f32* playlistOffset, f32 displayedItems, f32 maxItems)
	{
		auto sepCon = Interface::Separator::GetContainer();
		auto audioCon = Audio::Object::GetAudioObjectContainer();
		s32 separatorH = Data::_PLAYLIST_SEPARATOR_SIZE.y;
		s32 bottomPlaylistBorder = Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y;
		
		if (displayedItems < Audio::Object::GetSize())
		{
			//displayedItems += 3;
			s32 itemH = Data::_PLAYLIST_ITEM_SIZE.y;
			s32 separatorsOffset = sepCon->size() * separatorH;

			f32 playlistSurface = Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y - Data::_PLAYLIST_ITEMS_SURFACE_POS.y;
			f32 scaleY = (playlistSurface / ((float)Audio::Object::GetSize() + separatorsOffset / itemH)) * (displayedItems);
			// Set minimum size for scroll bar and do calculations with changed size
			if (scaleY < Data::_PLAYLIST_SCROLL_BAR_SIZE.y)
				scaleY = Data::_PLAYLIST_SCROLL_BAR_SIZE.y;
			f32 scrollSurface = playlistSurface - scaleY;
			


			assert(m_PlaylistBarSlider != NULL);

			m_PlaylistBarSlider->GetButtonSize().y = scaleY;


			

			if (App::Input::IsKeyDown(App::KeyCode::MouseLeft))
			{

				App::Input::GetGlobalMousePosition(&mouseX, &mouseY);


				if (Input::hasFocus(Input::ButtonType::SliderPlaylist))
				{
					playlistSliderActive = true;

					if (playlistSliderFirstEnter)
					{
						lastMouseY = mouseY;
						playlistSliderFirstEnter = false;
					}
				}
			}
			else
			{
				App::Input::GetMousePosition(&mouseXPosPrevious, &mouseYPosPrevious);
				sliderYPosPrevious = m_PlaylistBarSlider->GetButtonPos().y;
				playlistSliderActive = false;
				playlistSliderFirstEnter = true;
			}

			App::Input::GetMousePosition(&mouseX, &mouseY);

			if (playlistSliderActive && 
				mouseY > Data::_PLAYLIST_ITEMS_SURFACE_POS.y &&
				mouseY < Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y)
			{
				s32 relX, relY;
				App::Input::GetRelavtiveMousePosition(&relX, &relY);

				if (relY > 0 &&
					scrollBarAtBottom == true)
				{
					relY = 0;
				}
				if(relY != 0)
					State::SetState(State::PlaylistMovement);


				*playlistOffset += ((relY * maxItems) / (displayedItems));

				f32 scrollStep = ((*playlistOffset) * scrollSurface) / ((maxItems - displayedItems) * itemH + separatorsOffset);
				f32 newPosY = (Data::_PLAYLIST_SCROLL_BAR_POS.y + scrollStep);
				m_PlaylistBarSlider->SetButtonPos(glm::vec2(m_PlaylistBarSlider->GetButtonPos().x, newPosY));


				if (m_PlaylistBarSlider->GetButtonPos().y < Data::_PLAYLIST_ITEMS_SURFACE_POS.y)
				{
					m_PlaylistBarSlider->GetButtonPos().y = Data::_PLAYLIST_ITEMS_SURFACE_POS.y;
				}
				if (m_PlaylistBarSlider->GetButtonPos().y + m_PlaylistBarSlider->GetButtonSize().y > Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y)
				{
					m_PlaylistBarSlider->GetButtonPos().y = Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y - m_PlaylistBarSlider->GetButtonSize().y;
				}

			}

			if (playlistSliderActive &&
				mouseY < Data::_PLAYLIST_ITEMS_SURFACE_POS.y)
			{
				m_PlaylistBarSlider->SetButtonPos(glm::vec2(m_PlaylistBarSlider->GetButtonPos().x, Data::_PLAYLIST_ITEMS_SURFACE_POS.y));
				*playlistOffset = 0;
			}

			if (playlistSliderActive &&
				mouseY > Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y)
			{
				m_PlaylistBarSlider->SetButtonPos(glm::vec2(m_PlaylistBarSlider->GetButtonPos().x, 
															Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y - m_PlaylistBarSlider->GetButtonSize().y));
				*playlistOffset = audioCon.size() * itemH + sepCon->size() * separatorH;
			}

			if (playlistSliderActive)
			{
				s32 offsetX = Data::PlaylistBarMovableZoneXOffset;
				s32 winPosX, winPosY;
				App::Input::GetGlobalMousePosition(&mouseX, &mouseY);
				Window::GetWindowPos(&winPosX, &winPosY);
				mouseX -= winPosX - 10; // Should'nt be hardcored, recheck dimension's definitions
				mouseY -= winPosY;

				// If mouse is outside scroll bar movable zone, move scroll bar to the beginning
				if (mouseX < Data::_PLAYLIST_SCROLL_BAR_POS.x - offsetX ||
					mouseX > Data::_PLAYLIST_SCROLL_BAR_POS.x + offsetX)
				{
					scrollBarMaxBoundsCrossed = true;
					
					m_PlaylistBarSlider->SetButtonPos(glm::vec2(m_PlaylistBarSlider->GetButtonPos().x, Data::_PLAYLIST_ITEMS_SURFACE_POS.y));
					*playlistOffset = 0;
				}
				else
				{
					/*	Move playlist and scroll bar after it is back in bar movable zone to exactly
						the same position as mouse cursor's
					*/
					if (scrollBarMaxBoundsCrossed == true)
					{
						
						s32 mouseRelativeToPlaylistPos = mouseY - Data::_PLAYLIST_ITEMS_SURFACE_POS.y;
						f32 divider = mouseRelativeToPlaylistPos / playlistSurface;
						if (divider < 0)
							divider = 0;
						if (divider > 1.f)
							divider = 1.f;
						md_log(divider);
						*playlistOffset = (audioCon.size() * itemH + sepCon->size() * separatorH) * divider;
						if (mouseY < Data::_PLAYLIST_ITEMS_SURFACE_POS.y)
						{
							*playlistOffset = 0;
						}
						if (mouseY > Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y)
						{
							*playlistOffset = audioCon.size() * itemH + sepCon->size() * separatorH;
						}


						scrollBarMaxBoundsCrossed = false;
					}
				}

			}

			f32 scrollStep = ((*playlistOffset) * scrollSurface) / ((maxItems - displayedItems) * itemH + separatorsOffset);
			f32 newPosY = (Data::_PLAYLIST_SCROLL_BAR_POS.y + scrollStep);
			m_PlaylistBarSlider->SetButtonPos(glm::vec2(m_PlaylistBarSlider->GetButtonPos().x, newPosY));

			if (m_PlaylistBarSlider->GetButtonPos().y < Data::_PLAYLIST_ITEMS_SURFACE_POS.y)
			{
				m_PlaylistBarSlider->GetButtonPos().y = Data::_PLAYLIST_ITEMS_SURFACE_POS.y;
			}
			if (m_PlaylistBarSlider->GetButtonPos().y + m_PlaylistBarSlider->GetButtonSize().y > Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y)
			{
				m_PlaylistBarSlider->GetButtonPos().y = Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y - m_PlaylistBarSlider->GetButtonSize().y;
			}


			glm::mat4 model;
			model = glm::translate(model, glm::vec3(Data::_PLAYLIST_SCROLL_BAR_POS.x, m_PlaylistBarSlider->GetButtonPos().y, 0.9f));
			model = glm::scale(model, glm::vec3(Data::_PLAYLIST_SCROLL_BAR_SIZE.x, m_PlaylistBarSlider->GetButtonSize().y, 1.f));
			Shader::shaderDefault->setVec3("color", Color::Black);
			Shader::shaderDefault->setMat4("model", model);
			glBindTexture(GL_TEXTURE_2D, 0);
			Shader::Draw(Shader::shaderDefault);
		}

	}

	void MP::RenderPlaylistItemsNew()
	{

		if (m_Playlist.IsToggled() &&
			m_Playlist.IsEnabled())
		{
			RenderPlaylistButtons();
		}
		else
		{
			playlistAddFileActive = false;
		}


		if (m_Playlist.IsToggled() &&
			m_Playlist.IsEnabled() &&
			State::CheckState(State::PlaylistEmpty) == false)
		{
			auto audioCon = Audio::Object::GetAudioObjectContainer();
			auto sepCon = Interface::Separator::GetContainer();
			b8 loadItemsPositions(false);
			b8 loadItemsTextures(false);

			f32 itemH = Data::_PLAYLIST_ITEM_SIZE.y;
			f32 separatorH = Data::_PLAYLIST_SEPARATOR_SIZE.y;
			f32 dividor = itemH;

			f32 sumPos = audioCon.size() * itemH + sepCon->size() * separatorH;
			dividor = sumPos / (float)audioCon.size();
			//md_log(dividor);


			s32 upperPlaylistBorder = Data::_PLAYLIST_ITEMS_SURFACE_POS.y - 2 * itemH;
			s32 bottomPlaylistBorder = Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y;

			s32 upperBoundOfVisible = Data::_PLAYLIST_ITEMS_SURFACE_POS.y - 2 * itemH;
			s32 lowerBoundOfVisible = Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y + 2 * itemH;

			f32 displayedItems = float(Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y - Data::_PLAYLIST_ITEMS_SURFACE_POS.y) / itemH;
			
			if (playlistPositionOffset == 0)
				playlistMove = PlaylistMovement::Down;
			

			
			if (maxPosToRender > audioCon.size() - 1)
				maxPosToRender = audioCon.size() - 1;
			s32 passedSeparatorsCount = 0;
			f32 scrollStep = Data::PlaylistScrollStep;
			/*if(playlistFirstEnter == false)
			{
				for (auto & i : *sepCon)
				{
					if (i.second->GetButtonPos().y < audioCon[minPosToRender]->GetButtonPos().y &&
						playlistPositionOffset > 0)
					{
						i.second->Visible(false);
						passedSeparatorsCount++;
					}
					else if (i.second->GetButtonPos().y < audioCon[maxPosToRender]->GetButtonPos().y)
					{
						i.second->Visible(true);
					}
					else
					{
						i.second->Visible(false);
					}
				}
			}*/


			if (mdEngine::App::Input::IsScrollForwardActive() && m_Playlist.hasFocus())
			{
				playlistPositionOffset -= scrollStep;
				State::SetState(State::PlaylistMovement);
			}
			if (mdEngine::App::Input::IsScrollBackwardActive() && m_Playlist.hasFocus())
			{
				playlistPositionOffset += scrollStep;
				State::SetState(State::PlaylistMovement);
			}

			// Render playlist scroll bar
			//RenderScrollBar(&playlistPositionOffset, displayedItems, audioCon.size());



			if (playlistPositionOffset < 0)
			{
				playlistPositionOffset = 0;
			}

			while (abs(playlistPositionOffsetPrevious - playlistPositionOffset) > 0 ||
				   State::CheckState(State::AudioChanged) == true ||
				   State::CheckState(State::Window::Resized) == true)
			{
				for (auto & i : audioCon)
				{
					i->DeleteTexture();
					/*s32 index = i;
					if (Audio::Object::GetAudioObject(i) == NULL)
						index = i + Audio::GetFilesAddedCount();*/


					if (playlistPositionOffsetPrevious - playlistPositionOffset > 0)
					{
						i->SetButtonPos(
							glm::vec2(i->GetButtonPos().x,
									  i->GetButtonPos().y +
										abs(playlistPositionOffsetPrevious - playlistPositionOffset)));
						playlistMove = PlaylistMovement::Up;
					}
					else
					{
						i->SetButtonPos(
							glm::vec2(i->GetButtonPos().x,
									  i->GetButtonPos().y -
										abs(playlistPositionOffsetPrevious - playlistPositionOffset)));
						playlistMove = PlaylistMovement::Down;
					}

					if (playlistCursorPosition >= i->GetButtonPos().y &&
						playlistCursorPosition <= i->GetButtonPos().y + i->GetButtonSize().y)
					{
						minPosToRender = i->GetID();
					}

				}


				for (auto & i : *sepCon)
				{
					if (playlistPositionOffsetPrevious - playlistPositionOffset > 0)
					{
						i.second->SetButtonPos(
							glm::vec2(i.second->GetButtonPos().x,
								i.second->GetButtonPos().y +
								abs(playlistPositionOffsetPrevious - playlistPositionOffset)));

					}
					else
					{
						i.second->SetButtonPos(
							glm::vec2(i.second->GetButtonPos().x,
								i.second->GetButtonPos().y -
								abs(playlistPositionOffsetPrevious - playlistPositionOffset)));
					}
				}


				playlistPositionOffsetPrevious = playlistPositionOffset;

				// Lock playlist at the bottom, and when keep proper position when resizing
				if (bottomPlaylistBorder > audioCon.back()->GetButtonPos().y + audioCon.back()->GetButtonSize().y &&
					bottomPlaylistBorder - audioCon.back()->GetButtonPos().y > 1 &&
					playlistPositionOffset > 0 &&
					playlistFirstEnter == false)
				{
					playlistPositionOffset -= (bottomPlaylistBorder)-(audioCon.back()->GetButtonPos().y + audioCon.back()->GetButtonSize().y);
					playlistPositionOffset < 0 ? playlistPositionOffset = 0 : 0;
				}
				else
				{
					State::ResetState(State::Window::Resized);
				}

				if (State::CheckState(State::AudioChanged) == true)
				{
					s32 playingID = m_Playlist.GetPlayingID();

					s32 indexOfLast = minPosToRender + displayedItems - 1;
					s32 indexOfFirst = minPosToRender + 3;
					if (indexOfLast == audioCon.size() - 2 ||
						indexOfFirst == 0)
					{
						State::ResetState(State::AudioChanged);
						loadItemsTextures = true;
						continue;
					}

					if (indexOfLast > audioCon.size() - 1)
						indexOfLast > audioCon.size() - 1;

					if (audioCon[playingID]->GetButtonPos().y > audioCon[indexOfLast]->GetButtonPos().y)
					{
						playlistPositionOffset += itemH;
					}
					else if(audioCon[playingID]->GetButtonPos().y < audioCon[indexOfFirst]->GetButtonPos().y && 
							playlistPositionOffset > 0)
					{
						playlistPositionOffset -= itemH;
					}
					else
					{
						State::ResetState(State::AudioChanged);
					}
					//md_log(playlistPositionOffset);
					//State::ResetState(State::AudioChanged);
				}



				loadItemsTextures = true;
			}


			// Render playlist scroll bar after playlist position offset is checked
			RenderScrollBar(&playlistPositionOffset, displayedItems, audioCon.size());

			/*  Sometimes method that is used to obtain playlist position is not accurate, when playlist scroll bar
				is moved really quick. Make sure that playlist position is accurate.
				TRY TO FIX THAT IN MORE EFFICIENT WAY
			*/
			while (playlistCursorPosition < audioCon[minPosToRender]->GetButtonPos().y &&
				   minPosToRender > 0)
			{
				minPosToRender--;
			}

			while (playlistCursorPosition > audioCon[minPosToRender]->GetButtonPos().y + audioCon[minPosToRender]->GetButtonSize().y &&
				minPosToRender < audioCon.size() - 1)
			{
				minPosToRender++;
			}

			if (minPosToRender < 0)
				minPosToRender = 0;
			if (playlistPositionOffset < 2 * itemH)
				minPosToRender = 0;

			/*if (State::CheckState(State::AudioChanged) == true)
			{
				s32 playingID = m_Playlist.GetPlayingID();
				md_log_compare(minPosToRender, maxPosToRender);
				md_log(playingID);
				md_log(displayedItems);

				if(playingID > minPosToRender + (s32)displayedItems - 3)
				{
					playlistPositionOffset += itemH;
					minPosToRender++;
				}

				State::ResetState(State::AudioChanged);
			}*/

			if (audioCon.back() == NULL)
				return;
			
			// Find the max position that will be visible considering playlist separators size
			maxPosToRender = minPosToRender + displayedItems + 2;
			if (maxPosToRender > audioCon.size() - 1)
				maxPosToRender = audioCon.size() - 1;




			if (audioCon[maxPosToRender] == NULL || audioCon[minPosToRender] == NULL)
				return;

			f32 posDifference = audioCon[maxPosToRender]->GetButtonPos().y - audioCon[minPosToRender]->GetButtonPos().y;
			displayedItems = posDifference / itemH;
			displayedItems += 2;
			maxPosToRender = minPosToRender + displayedItems + 2;
			if (maxPosToRender > audioCon.size())
				maxPosToRender = audioCon.size();

			//md_log_compare(minPosToRender, maxPosToRender);

			b8 loadTextureFirstEnter(false);
			if (State::CheckState(State::AudioAdded) ||
				State::CheckState(State::AudioDeleted) ||
				State::CheckState(State::Window::Resized))
			{
				loadItemsPositions = true;
				playlistFirstEnter = false;
				loadTextureFirstEnter = true;

				State::ResetMusicPlayerState();
				State::ResetState(State::Window::Resized);
			}

			if (loadItemsPositions == true)
			{
				glm::vec2 startPos = glm::vec2(Data::_PLAYLIST_ITEMS_SURFACE_POS.x,
											   Data::_PLAYLIST_ITEMS_SURFACE_POS.y - playlistPositionOffset);

				playlistCursorPosition = Data::_PLAYLIST_ITEMS_SURFACE_POS.y - 1 * itemH;

				if (loadItemsTextures == true)
				{
					for (s32 i = minPosToRender; i < maxPosToRender; i++)
					{
						audioCon[i]->ReloadTextTexture();
					}

					loadItemsTextures = false;
				}

				for (auto & i : audioCon)
				{
					if (i->IsFolderRep() == true)
					{
						auto playlistSeparator = Interface::Separator::GetSeparatorByID(i->GetID());
						assert(playlistSeparator != nullptr);

						playlistSeparator->SetButtonPos(glm::vec2(startPos.x - Data::_PLAYLIST_SEPARATOR_POS_OFFSET.x,
							startPos.y));
						startPos.y += playlistSeparator->GetButtonSize().y;
					}

					i->SetButtonPos(startPos);
					i->DeleteTexture();
					//i->InitTextTexture();

					startPos.y += itemH;
				}

				for (auto & i : *sepCon)
				{
					i.second->InitTextTexture();
				}

				loadItemsPositions = false;
				loadItemsTextures = true;
			}

			if (loadItemsTextures == true)
			{
				//md_log_compare(minPosToRender, maxPosToRender);
				for (s32 i = minPosToRender; i < maxPosToRender; i++)
				{
					audioCon[i]->ReloadTextTexture();
				}

				loadItemsTextures = false;
			}

			// ACTUAL RENDERING

			Shader::shaderDefault->use();
			Shader::shaderDefault->setBool("playlistCut", true);
			Shader::shaderDefault->setFloat("playlistMinY", Data::_PLAYLIST_ITEMS_SURFACE_POS.y);
			Shader::shaderDefault->setFloat("playlistMaxY", Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y);
			Shader::shaderBorder->use();
			Shader::shaderBorder->setFloat("playlistMinY", Data::_PLAYLIST_ITEMS_SURFACE_POS.y);
			Shader::shaderBorder->setFloat("playlistMaxY", Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y);


			//md_log_compare(minPosToRender, maxPosToRender);
			std::vector<s32> indexes;
			// Render ALL playlist separators
			for (auto & i : *sepCon)
			{
				i.second->DrawItem(main_foreground);
			}

			// Render VISIBLE playlist items
			for (s32 i = minPosToRender; i < maxPosToRender; i++)
			{
				if (audioCon[i] == NULL)
					return;

				indexes.push_back(i);
				
				if (audioCon[i]->HasTexture() == false)
				{
					m_Playlist.SetIndexesToRender(indexes);
					Shader::shaderDefault->setVec3("color", Color::White);
					continue;
				}
				audioCon[i]->DrawItem(main_foreground);
			}

			
			for (auto & i : m_Playlist.multipleSelect)
			{
				audioCon[*i]->DrawDottedBorder();
			}

			m_Playlist.SetIndexesToRender(indexes);
			Shader::shaderDefault->use();
			Shader::shaderDefault->setBool("playlistCut", false);
			Shader::shaderDefault->setVec3("color", Color::White);
		}
		else
		{
			
			m_Playlist.SetIndexesToRender(std::vector<s32>());
			playlistPositionOffset = 0;
			playlistFirstEnter = true;
		}



	}



	void MP::RenderPlaylistItems()
	{
		/* Functions has guards if statements like 

				"if (GetAudioObject(i) == NULL)
					 return;"

		   to prevent from accessing playlist element that was not loaded yet. Playlist items will be rendered by this functions
		   only when flag "State::IsPlaylistEmpty" is false, meaning that files were added to the music player, but not every file
		   may be loaded yet.
		*/

		if (m_Playlist.IsToggled() &&
			m_Playlist.IsEnabled())
		{
			RenderPlaylistButtons();
		}
		else
		{
			playlistAddFileActive = false;
		}


		if (m_Playlist.IsToggled() && 
			m_Playlist.IsEnabled() && 
			State::CheckState(State::PlaylistEmpty) == false)
		{

			if (mdEngine::App::Input::IsScrollForwardActive() && m_Playlist.hasFocus())
			{
				playlistCurrentOffsetY += Data::PlaylistScrollStep;
			}
			if (mdEngine::App::Input::IsScrollBackwardActive() && m_Playlist.hasFocus())
			{
				playlistCurrentOffsetY -= Data::PlaylistScrollStep;
			}
			
			s32 itemH = Data::_PLAYLIST_ITEM_SIZE.y;
			s32 displayedItems = (Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y - Data::_PLAYLIST_ITEMS_SURFACE_POS.y) / itemH;
			f32 displayedItemsF = float(Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y - Data::_PLAYLIST_ITEMS_SURFACE_POS.y) / (float)itemH;
			s32 maxItems = Audio::Object::GetSize();

			auto indexesToRenderVec = m_Playlist.GetIndexesToRender();

			if (displayedItems > maxItems)
			{
				displayedItems = maxItems;
				displayedItemsF = maxItems;
			}

			if (displayedItems < 0)
			{
				displayedItems = 0;
				displayedItemsF = 0;
			}

			s32 currentSongID = mdEngine::MP::Playlist::RamLoadedMusic.mID;

			
			f32 top = playlistCurrentOffsetY;
			f32 low = top - displayedItems * itemH;
			f32 currentPos = 0;

			currentPos = currentSongID * itemH * -1;
			/* When song is changed, scope to song's position in the playlist */
			if (State::CheckState(State::AudioChanged) == true &&
				State::CheckState(State::PathLoadedFromFile) == true)
			{
				while (currentPos + itemH > top)
				{
					top += itemH;
					playlistCurrentOffsetY += itemH;
				}
				
				while (currentPos - (2 * itemH) < low)
				{
					low -= itemH;
					playlistCurrentOffsetY -= itemH;
				}
				
				if (currentPos >= 0)
					playlistCurrentOffsetY -= itemH;
				//if(currentPos <= (maxItems - 1) * itemH * -1)
					//testOffsetY += itemH;
				State::ResetMusicPlayerState();
			}

			if (playlistCurrentOffsetY > 0)
				playlistCurrentOffsetY = 0;

			/* When playlist pos is different than top and low, keep resizing till it reaches end, 
			   then start roll out to the top 
			*/
			
			if (State::CheckState(State::Window::Resized) == true &&
				abs(Window::windowProperties.mDeltaHeightResize) > 0 &&
				playlistCurrentOffsetY <= (maxItems - displayedItems - 1) * itemH * -1 &&
				displayedItems < maxItems - 2 )
			{
				playlistCurrentOffsetY += Window::windowProperties.mDeltaHeightResize;
			}
	

			s32 playlistOffset = playlistCurrentOffsetY;
			
			playlistCurrentPos = (s32)(playlistOffset / itemH) * -1;


			


			/* If playlist is resized and displayed items + 4 is greater than current rendered items,
			   start rendering more items(prevent from updating textures and positions on every resize,
			   only when it is needed).
			*/
			if (State::CheckState(State::DeletionFinished) == true)
			{
				if ((playlistCurrentPos + displayedItems > Audio::Object::GetSize() ||
					playlistFirstLoad == true ||
					currentlyRenderedItems < displayedItems + 4) &&
					State::CheckState(State::FilesLoaded) == true)
				{
					texturesLoaded = false;
					playlistFirstLoad = false;

					if (State::CheckState(State::AudioDeleted) == false)
						State::ResetMusicPlayerState();
				}

				State::ResetState(State::DeletionFinished);
			}

			if (State::CheckState(State::AudioDeleted) == true)
			{
				playlistSeparatorToRenderVec.clear();
			}

			if (State::CheckState(State::AudioDeleted) == true ||
				State::CheckState(State::ContainersResized) == true ||
				State::CheckState(State::Window::Resized) == true ||
				State::CheckState(State::AudioHidden) == true)
			{
				texturesLoaded = false;
				State::ResetMusicPlayerState();
			}
;
			if (playlistCurrentPos > Audio::Object::GetSize())
				playlistCurrentPos = Audio::Object::GetSize();

			if (State::CheckState(State::FilesLoaded) == true)
			{
				minPos = playlistCurrentPos;
				maxPos = displayedItems + playlistCurrentPos + 2;;
			}



			hiddenItemsOffsetY = 0;
			hiddenItemsCount = 0;
			auto sepCon = Interface::Separator::GetContainer();
			s32 hiddenSepCount = 0;
			for (auto & k : *sepCon)
			{
				if (k.second->IsSeparatorHidden() == true)
				{
					auto sepSubCon = k.second->GetSubFilesContainer();

					hiddenItemsCount += sepSubCon->size();
					hiddenItemsOffsetY += sepSubCon->size() * itemH;
				}
			}

			indexesToRenderVec.clear();


			if (minPos < 0)
				minPos = 0;

			if (maxPos > Audio::Object::GetSize())
				maxPos = Audio::Object::GetSize();

			s32 visibleSpectrum = maxPos - minPos;

			auto audioCon = Audio::Object::GetAudioObjectContainer();
			for (s32 i = minPos; i < maxPos; i++)
			{
				indexesToRenderVec.push_back(i);
			}

			m_Playlist.SetIndexesToRender(indexesToRenderVec);


			/*if (playlistFirstEnter && playlistCurrentOffsetY > 80)
			{
				playlistPreviousPos = playlistCurrentPos;
				playlistPreviousOffsetY = playlistCurrentOffsetY;

				playlistFirstEnter = false;
			}

			/*if (playlistCurrentOffsetY < (maxItems - displayedItems) * itemH * -1)
				playlistCurrentOffsetY = (maxItems - displayedItems) * itemH * -1;*/

			if (playlistCurrentOffsetY < (maxItems - displayedItems) * itemH * -1 + (displayedItemsF - displayedItems) * itemH &&
				playlistCurrentOffsetY != 0)
			{
				playlistCurrentOffsetY = (maxItems - displayedItems) * itemH * -1 + (displayedItemsF - displayedItems) * itemH;
			}


			if (abs(playlistPreviousOffsetY - playlistCurrentOffsetY) > 0)
			{
				//for(s32 i = minPos; i < maxPos; i++)
				for (auto i : indexesToRenderVec)
				{
					s32 index = i;
					if (Audio::Object::GetAudioObject(i) == NULL)
						index = i + Audio::GetFilesAddedCount();

					if (playlistPreviousOffsetY - playlistCurrentOffsetY > 0)
					{
						Audio::Object::GetAudioObject(index)->SetButtonPos(
							glm::vec2(Audio::Object::GetAudioObject(i)->GetButtonPos().x,
								Audio::Object::GetAudioObject(i)->GetButtonPos().y -
								abs(playlistPreviousOffsetY - playlistCurrentOffsetY)));
					}
					else
					{
						Audio::Object::GetAudioObject(index)->SetButtonPos(
							glm::vec2(Audio::Object::GetAudioObject(i)->GetButtonPos().x,
								Audio::Object::GetAudioObject(i)->GetButtonPos().y +
								abs(playlistPreviousOffsetY - playlistCurrentOffsetY)));
					}
					
				}

				auto sepCon = Interface::Separator::GetContainer();
				for (s32 i = 0; i < sepCon->size(); i++)
				{
					//auto sep = playlistSeparatorToRenderVec[i];
					/* Check if any of items in the vectors are NULL. If it's NULL it means that this
					   separator was empty and was deleted */

					if (sepCon->at(i).second->GetButtonPos() != glm::vec2(POS_INVALID))
					{
						if (playlistPreviousOffsetY - playlistCurrentOffsetY > 0)
						{
							sepCon->at(i).second->SetButtonPos(glm::vec2(sepCon->at(i).second->GetButtonPos().x,
								sepCon->at(i).second->GetButtonPos().y -
								abs(playlistPreviousOffsetY - playlistCurrentOffsetY)));
						}
						else
						{
							sepCon->at(i).second->SetButtonPos(glm::vec2(sepCon->at(i).second->GetButtonPos().x,
								sepCon->at(i).second->GetButtonPos().y +
								abs(playlistPreviousOffsetY - playlistCurrentOffsetY)));
						}
					}
				}

				playlistPreviousOffsetY = playlistCurrentOffsetY;
			}

			
			if (playlistCurrentPos != playlistPreviousPos)
				texturesLoaded = false;


			//md_log(playlistCurrentOffsetY);
			if (texturesLoaded == false && State::CheckState(State::FilesLoaded) == true)
			{
				if (audioCon[playlistPreviousPos]->IsFolderRep() == true &&
					playlistCurrentPos > playlistPreviousPos)
				{
					playlistSeparatorsOffset += Data::_PLAYLIST_SEPARATOR_SIZE.y;
				}

				if (audioCon[playlistCurrentPos]->IsFolderRep() == true &&
					playlistCurrentPos < playlistPreviousPos)
				{
					playlistSeparatorsOffset -= Data::_PLAYLIST_SEPARATOR_SIZE.y;
					//indexesToRenderVec.insert(indexesToRenderVec.begin(), 0);
				}

				/*if (audioCon[playlistPreviousPos]->IsFolderRep() == true &&
					playlistCurrentPos > playlistPreviousPos &&
					playlistCurrentPos == 3)
				{
					playlistSeparatorsOffset += Data::_PLAYLIST_SEPARATOR_SIZE.y;
				}

				if (audioCon[playlistCurrentPos]->IsFolderRep() == true &&
						 playlistCurrentPos < playlistPreviousPos &&
						 playlistPreviousPos == 3)
				{
					playlistSeparatorsOffset -= Data::_PLAYLIST_SEPARATOR_SIZE.y;
				}*/

				playlistPreviousPos = playlistCurrentPos;
				// Load all textures that are in in display range 
				if (textTexture != NULL)
				{
					glDeleteTextures(allocatedTextTexturesCount, textTexture);
					delete[] textTexture;
				}

				if (separatorTextTexture != NULL)
				{
					glDeleteTextures(allocatedSeparatorTexturesCount, separatorTextTexture);
					delete[] separatorTextTexture;
				}

				if (predefinedPos != NULL)
					delete[] predefinedPos;

				auto sepCon = Interface::Separator::GetContainer();
				for (auto & i : *sepCon)
				{
					i.second->Visible(false);
				}
				playlistSeparatorToRenderVec.clear();

				s32 visibleSeparators = 0;
				//for(s32 i = minPos; i < maxPos; i++)
				for (auto & i : indexesToRenderVec)
				{
					if (Audio::Object::GetAudioObject(i)->IsFolderRep() == true)
						visibleSeparators++;
				}

				playlistSeparatorsIDsVec.clear();

				currentlyRenderedItems	= visibleSpectrum;
				textTexture				= new GLuint[visibleSpectrum];
				separatorTextTexture	= new GLuint[visibleSeparators];
				predefinedPos			= new glm::vec2[visibleSpectrum];


				allocatedTextTexturesCount = visibleSpectrum;
				allocatedSeparatorTexturesCount = visibleSeparators;

				// Predefine positions user can actually see
				Audio::AudioObject* item = NULL;
				s32 itemsOffset = 0;
				glm::vec2 itemStartPos = glm::vec2(Data::_PLAYLIST_ITEMS_SURFACE_POS.x, 
												   Data::_PLAYLIST_ITEMS_SURFACE_POS.y + (minPos - 1) * itemH);


				/*if (playlistCurrentPos == 1)
					itemStartPos.y += audioCon[playlistCurrentPos - 1]->GetButtonPos().y + itemH - Data::_PLAYLIST_ITEMS_SURFACE_POS.y;*/

				/*s32 texIndex = 0;
				for (auto & k : *sepCon)
				{
					if (k.second->IsSeparatorHidden() == true)
					{
						s32 index = minPos + 2;
						
						md_log_compare(k.second->GetButtonPos().y, Audio::Object::GetAudioObject(index)->GetButtonPos().y - 3 * itemH);
						if (k.second->GetButtonPos().y > Audio::Object::GetAudioObject(index)->GetButtonPos().y - 3 * itemH)
						{
							separatorTextTexture[texIndex] = k.second->GetLoadedTexture();
							itemStartPos.y += k.second->GetButtonSize().y;
							playlistSeparatorToRenderVec.push_back(k.second);
							texIndex++;
						}
					}
				}*/

			
				predefinedPos[0] = glm::vec2(itemStartPos.x, itemStartPos.y);
				//md_log(itemStartPos.y);
				s32 i = 0, t = 0, k = 0;
				//for (u16 i = minPos, t = 0, k = 0; i < maxPos; i++, t++)
				for (auto & i : indexesToRenderVec)
				{
					if (Audio::Object::GetAudioObject(i) == NULL)
						return;

					item = Audio::Object::GetAudioObject(i);
					textTexture[t] = item->GetLoadedTexture();

				
					if (item->IsFolderRep() == true)
					{
						auto playlistSeparator = Interface::Separator::GetSeparatorByID(i);
						assert(playlistSeparator != nullptr);

						s32 pSepPos = itemStartPos.y + Data::_PLAYLIST_ITEM_SIZE.y + playlistCurrentOffsetY;

						playlistSeparator->SetButtonPos(glm::vec2(itemStartPos.x - Data::_PLAYLIST_SEPARATOR_POS_OFFSET.x,
																  pSepPos));
						playlistSeparator->Visible(true);
						playlistSeparatorToRenderVec.push_back(playlistSeparator);
						itemStartPos.y += playlistSeparator->GetButtonSize().y;
						//playlistSeparatorsOffset += playlistSeparator->GetButtonSize().y;

						// Load separator texture
						separatorTextTexture[k] = playlistSeparator->GetLoadedTexture();
						k++;
						
					}


					if (t != -1)
					{
						predefinedPos[t] = glm::vec2(itemStartPos.x, itemStartPos.y + Data::_PLAYLIST_ITEM_SIZE.y);
						itemStartPos = predefinedPos[t];
						predefinedPos[t].y += playlistCurrentOffsetY;
					}

					t++;
				}
				/*if (playlistCurrentPos > 0)
					playlistSeparatorsOffset = 0;*/

				// Write predefined positions to playlist items that are currently displayed
				for (s32 i = 0, t = 0; i < Audio::Object::GetSize(); i++)
				{
					if (Audio::Object::GetAudioObject(i) == NULL)
						break;

					if (i >= minPos && i < maxPos)
					{
						Audio::Object::GetAudioObject(i)->SetButtonPos(glm::vec2(predefinedPos[t].x, predefinedPos[t].y));
						t++;
					}
					else
					{
						Audio::Object::GetAudioObject(i)->SetButtonPos(glm::vec2(POS_INVALID));
					}
				}


				texturesLoaded = true;
			}


			//RenderScrollBar();



			// Cut all items and item border if is outside playlist bounds
			Shader::shaderDefault->setBool("playlistCut", true);
			Shader::shaderDefault->setFloat("playlistMinY", Data::_PLAYLIST_ITEMS_SURFACE_POS.y);
			Shader::shaderDefault->setFloat("playlistMaxY", Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y);
			Shader::shaderBorder->use();
			Shader::shaderBorder->setFloat("playlistMinY", Data::_PLAYLIST_ITEMS_SURFACE_POS.y);
			Shader::shaderBorder->setFloat("playlistMaxY", Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y);

			u16 texIndex = 0;
			//playlistIndex = minPos;

			for(auto & playlistIndex : indexesToRenderVec)
			//while (playlistIndex < maxPos)
			{
				/* Playlist items are rendered on the same positions as positions of the first(0 - maxItems), 
				   they are only transformed by matrix, so when you want to check if specific item has focus, 
				   it will check the focus on the wrong item(0 - maxItems). To prevent that, always add mOffsetIndex,
				   do checked positions, so you will get index of actuall rendered item. */
				Interface::PlaylistItem::OffsetIndex = playlistCurrentPos;

				//if (GetAudioObject(playlistIndex) == NULL)
					//return;

				Audio::AudioObject* aItem = nullptr;
				aItem = Audio::Object::GetAudioObject(playlistIndex);

				// It will prevent from accessing wrong audio object
				if (State::CheckState(State::FilesLoaded) == false && Audio::GetDroppedOnIndex() <= playlistIndex)
				{
					auto test = Audio::Object::GetAudioObjectContainer();
					s32 testIndex = Audio::GetFilesAddedCount();
					aItem = Audio::Object::GetAudioObject(Audio::GetFilesAddedCount() + playlistIndex);
				}
				else
				{
					aItem = Audio::Object::GetAudioObject(playlistIndex);
				}

				assert(aItem != NULL);

				glm::vec3 itemColor;

				// Fint in vector with selected positions if current's id is inside
				auto it = std::find(m_Playlist.multipleSelect.begin(),
									m_Playlist.multipleSelect.end(),
									&aItem->GetID());

				if (m_Playlist.GetPlayingID() && it != m_Playlist.multipleSelect.end())
				{
					itemColor *= Color::Red * Color::Grey;
					aItem->DrawDottedBorder();
				}
				else if (aItem->GetID() == m_Playlist.GetPlayingID())
				{
					itemColor = Color::Red * Color::Grey;
				}
				else if (it != m_Playlist.multipleSelect.end())
				{
					itemColor = Color::Grey;
					aItem->DrawDottedBorder();
				}
				else
				{
					itemColor = aItem->GetItemColor();;
				}

				// Render all folder's reps in blue



				// Draw border for every selected item
				if (it != m_Playlist.multipleSelect.end())
				{
					if (aItem->GetID() == m_Playlist.GetPlayingID())
					{
						itemColor = Color::Red * Color::Grey;
						aItem->DrawDottedBorder();
					}
					else
					{
						itemColor = Color::Grey;
						aItem->DrawDottedBorder();
					}
				}
				//if (item->clickCount == 1)
					//std::cout << (float)item->mTextSize.x * item->mTextScale << std::endl;
			
				/*if (aItem->IsFolderRep() == true)
					itemColor *= Color::Blue;*/

				Shader::shaderDefault->use();
				glm::mat4 model;

				for (s32 i = 0; i < playlistSeparatorToRenderVec.size(); i++)
				{
					auto sep = playlistSeparatorToRenderVec[i];
					model = glm::mat4();
					model = glm::translate(model, glm::vec3(sep->GetButtonPos().x, sep->GetButtonPos().y, 0.6f));
					model = glm::scale(model, glm::vec3(sep->GetButtonSize(), 1.0f));
					Shader::shaderDefault->setMat4("model", model);
					Shader::shaderDefault->setVec3("color", Color::Pink);
					glBindTexture(GL_TEXTURE_2D, main_foreground);
					Shader::Draw(Shader::shaderDefault);
					Shader::shaderDefault->setVec3("color", Color::White);

					sep->SetTextOffset(glm::vec2(5.f, s32(sep->GetButtonSize().y - sep->GetTextSize().y) / 2));
					sep->DrawString(separatorTextTexture[i]);
					
				}

		
				//if (aItem->IsPlaylistItemHidden() == false)
				{
					model = glm::mat4();
					model = glm::translate(model, glm::vec3(aItem->GetButtonPos().x, aItem->GetButtonPos().y, 0.5f));
					model = glm::scale(model, glm::vec3(aItem->GetButtonSize(), 1.0f));
					Shader::shaderDefault->setMat4("model", model);
					Shader::shaderDefault->setVec3("color", itemColor);
					glBindTexture(GL_TEXTURE_2D, main_foreground);
					Shader::Draw(Shader::shaderDefault);
					Shader::shaderDefault->setVec3("color", Color::White);


					glm::vec3 col = Color::DarkGrey * itemColor;
					if (aItem->topHasFocus)
					{
						model = glm::mat4();
						model = glm::translate(model, glm::vec3(aItem->GetButtonPos().x, aItem->GetButtonPos().y, 0.6f));
						model = glm::scale(model, glm::vec3(glm::vec2(aItem->GetButtonSize().x, aItem->GetButtonSize().y / 2.f), 1.0f));
						Shader::shaderDefault->setMat4("model", model);
						Shader::shaderDefault->setVec3("color", col);
						glBindTexture(GL_TEXTURE_2D, main_foreground);
						Shader::Draw(Shader::shaderDefault);
						Shader::shaderDefault->setVec3("color", Color::White);

					}

					if (aItem->bottomHasFocus)
					{
						model = glm::mat4();
						model = glm::translate(model, glm::vec3(aItem->GetButtonPos().x, aItem->GetButtonPos().y + aItem->GetButtonSize().y / 2.f, 0.6f));
						model = glm::scale(model, glm::vec3(glm::vec2(aItem->GetButtonSize().x, aItem->GetButtonSize().y / 2.f), 1.0f));
						Shader::shaderDefault->setMat4("model", model);
						Shader::shaderDefault->setVec3("color", col);
						glBindTexture(GL_TEXTURE_2D, main_foreground);
						Shader::Draw(Shader::shaderDefault);
						Shader::shaderDefault->setVec3("color", Color::White);
					}



					if (aItem->GetButtonPos().y < Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y)
					{
						aItem->SetTextOffset(glm::vec2(5.f, 8.f));
						aItem->DrawString(textTexture[texIndex]);
					}
				}

				texIndex++;
				//playlistIndex++;
			}

			Shader::shaderDefault->setBool("playlistCut", false);

			Shader::shaderDefault->setVec3("color", Color::White);
		}
		else
		{
			m_Playlist.SetCurrentMinIndex(0);
			m_Playlist.SetCurrentMaxIndex(0);
			currentlyRenderedItems = 0;
			playlistPreviousOffsetY = 0;
			playlistCurrentOffsetY = 0;
			playlistCurrentPos = 0;
			playlistPreviousPos = 0;
			playlistIndex = 0;
			playlistFirstLoad = true;
		}


	}

	void MP::RenderPlaylistButtons()
	{
		m_AddFileButtonRef->GetButtonPos() = Data::_PLAYLIST_ADD_BUTTON_POS;
		m_AddFileButtonRef->GetButtonSize() = Data::_PLAYLIST_ADD_BUTTON_SIZE;

		glm::mat4 model;
		glm::vec3 color(1.f);
		if (Input::hasFocus(Input::ButtonType::PlaylistAddFile))
			color = Color::Red * Color::Grey;
		else
			color = Color::Grey;

		/* If add file button is pressed, display text box, else if mouse 
		   is clicked outside add file button and text box, disable it*/
		if (Input::isButtonPressed(Input::ButtonType::PlaylistAddFile))
		{
			playlistAddFileActive = !playlistAddFileActive;
		}
		else if (App::Input::IsKeyDown(App::KeyCode::MouseLeft) &&
				!Input::hasFocus(Input::ButtonType::PlaylistAddFile) &&
				!Input::hasFocus(Input::ButtonType::PlaylistAddFileTextBox))
		{
			playlistAddFileActive = false;
		}
	

		m_AddFileTextBox.SetPos(glm::vec2(Data::_PLAYLIST_ADD_BUTTON_POS.x,
								Data::_PLAYLIST_ADD_BUTTON_POS.y + Data::_PLAYLIST_ADD_BUTTON_SIZE.y));
		m_AddFileTextBox.UpdateItemPos();

		if (playlistAddFileActive)
		{
			m_AddFileTextBox.Render();

			// If any of there text box items are pressed, disable text box
			if (m_AddFileTextBox.isItemPressed(Strings::_PLAYLIST_ADD_FILE))
			{
				std::wcout << Strings::_PLAYLIST_ADD_FILE;
				playlistAddFileActive = false;
			}

			if (m_AddFileTextBox.isItemPressed(Strings::_PLAYLIST_ADD_FOLDER))
			{
				std::wcout << Strings::_PLAYLIST_ADD_FOLDER;
				playlistAddFileActive = false;
			}

			if (m_AddFileTextBox.isItemPressed(L"hehe"))
			{
				std::wcout << L"hehehe\n";
			}
		}

		model = glm::translate(model, glm::vec3(Data::_PLAYLIST_ADD_BUTTON_POS, 0.6));
		model = glm::scale(model, glm::vec3(Data::_PLAYLIST_ADD_BUTTON_SIZE, 1.0));
		Shader::shaderDefault->setMat4("model", model);
		Shader::shaderDefault->setVec3("color", color);
		glBindTexture(GL_TEXTURE_2D, playlist_add_file);
		Shader::Draw(Shader::shaderDefault);

		Shader::shaderDefault->setVec3("color", Color::White);
	}

	void MP::RenderSettingsButtons()
	{
		glm::mat4 model;
		/* UI Window buttons*/
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(Data::_UI_BUTTONS_BACKGROUND_LEFT_POS, 0.2f));
		model = glm::scale(model, glm::vec3(Data::_UI_BUTTONS_BACKGROUND_LEFT_SIZE, 1.f));;
		Shader::shaderDefault->setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, exit_background);
		Shader::Draw(Shader::shaderDefault);


		model = glm::mat4();
		model = glm::translate(model, glm::vec3(Data::_SETTINGS_BUTTON_POS, 0.3f));
		model = glm::scale(model, glm::vec3(Data::_SETTINGS_BUTTON_SIZE, 1.f));;
		Shader::shaderDefault->setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, exit_icon);
		Shader::Draw(Shader::shaderDefault);
	}

	void MP::RenderWindowControlButtons()
	{
		glm::mat4 model;
		/* UI Window buttons*/
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(500.f - 70.f, 5.f, 0.2f));
		model = glm::scale(model, glm::vec3(40.f, 15.0f, 1.f));;
		Shader::shaderDefault->setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, exit_background);
		Shader::Draw(Shader::shaderDefault);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(Data::_EXIT_BUTTON_POS, 0.3f));
		model = glm::scale(model, glm::vec3(Data::_EXIT_BUTTON_SIZE, 1.f));;
		Shader::shaderDefault->setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, exit_icon);
		Shader::Draw(Shader::shaderDefault);
	}

	void MP::StartMainWindow()
	{
		InitializeConfig();

		main_background			= mdLoadTexture("assets/main_stretched.png");
		main_foreground			= mdLoadTexture("assets/main_foreground.png");

		exit_background			= mdLoadTexture("assets/exit_background.png");
		exit_icon				= mdLoadTexture("assets/exit_icon.png");

		volume_bar				= mdLoadTexture("assets/volume_bar.png");
		volume_speaker			= mdLoadTexture("assets/volume_speaker.png");
		volume_speaker_muted	= mdLoadTexture("assets/volume_speaker_muted.png");

		play_button				= mdLoadTexture("assets/play_button.png");
		stop_button				= mdLoadTexture("assets/stop_button.png");
		next_button				= mdLoadTexture("assets/next_button.png");
		previous_button			= mdLoadTexture("assets/previous_button.png");
		shuffle_button			= mdLoadTexture("assets/shuffle_button.png");
		repeat_button			= mdLoadTexture("assets/repeat_button.png");
		dot_icon				= mdLoadTexture("assets/dot_button_state.png");
		playlist_button			= mdLoadTexture("assets/playlist_button.png");
		playlist_add_file		= mdLoadTexture("assets/playlist_add.png");

		music_progress_bar		= mdLoadTexture("assets/music_progress_bar.png");


		deltaVolumePos = (s32)(mdEngine::MP::Playlist::GetVolume() * 100.f * 0.9f);

		auto it = std::find_if(mdButtonsContainer.begin(), mdButtonsContainer.end(),
			[&](std::pair<Input::ButtonType, Interface::Button*> const& ref) { return ref.first == Input::ButtonType::SliderPlaylist; });

		if (it != mdButtonsContainer.end())
			m_PlaylistBarSlider = it->second;

		auto item = std::find_if(mdButtonsContainer.begin(), mdButtonsContainer.end(),
			[&](std::pair<Input::ButtonType, Interface::Button*> const& ref) { return ref.first == Input::ButtonType::PlaylistAddFile; });

		if (item != mdButtonsContainer.end())
			m_AddFileButtonRef = item->second;

		InitializeText();

		InitializeTextBoxes();
	}

	void MP::UpdateMainWindow()
	{
		/* On every file addition or deletion update the overall file durations and size
		   (if more files are added, it will update only once, after all files are loaded)
		*/
		if ((State::CheckState(State::FilesInfoLoaded) == true && updatePlaylistInfoPrevious == false) ||
			(State::CheckState(State::FilesLoaded) == true &&
				State::CheckState(State::PathLoadedFromFileVolatile) == true &&
				updatePlaylistInfoPrevious == false) ||
			State::CheckState(State::AudioDeleted) == true)
		{
			updatePlaylistInfoPrevious = true;
			updatePlaylistInfo = true;
		}

		updatePlaylistInfoPrevious = State::CheckState(State::FilesInfoLoaded);

		if (State::CheckState(State::PlaylistEmpty) == true &&
			playlistWasEmpty == false)
		{
			updatePlaylistInfo = true;
		}

			
		// Load items duration and size form file is commented right now
		if (updatePlaylistInfo == true &&
			State::CheckState(State::PlaylistEmpty) == true)
		{
			m_Playlist.SetItemsDuration(0.f);
			m_Playlist.SetItemsSize(0.f);

			durationText.SetTextString(utf8_to_utf16(m_Playlist.GetItemsDurationString()));
			itemsSizeText.SetTextString(utf8_to_utf16(m_Playlist.GetItemsSizeString()));
			itemsCountText.SetTextString(std::to_wstring(Audio::Object::GetSize()));

			durationText.ReloadTextTexture();
			itemsSizeText.ReloadTextTexture();
			itemsCountText.ReloadTextTexture();

			updatePlaylistInfo = false;
		}
		else if(updatePlaylistInfo == true)
		{
			f64 duration = 0;
			f64 size = 0;


			auto audioObj = Audio::Object::GetAudioObject(0);
			if (audioObj == nullptr)
				return;
			s32 k = 0;
			auto sepCon = Interface::Separator::GetContainer();
			auto itSep = Interface::Separator::GetSeparator(audioObj->GetFolderPath());
			if (itSep == nullptr)
				return;
			itSep->SepItemDuration = 0;
			for (auto & i : Audio::Object::GetAudioObjectContainer())
			{
				assert(i != nullptr);
				assert(itSep != NULL);
				
				if (itSep->GetSeparatorPath().compare(i->GetFolderPath()) != 0)
				{
					itSep = Interface::Separator::GetSeparator(i->GetFolderPath());
					k++;
					assert(itSep != NULL);
					if (k > Interface::Separator::GetSize())
						break;
					itSep->SepItemDuration = 0;
				}
				
				if (itSep == nullptr)
					break;

				itSep->SepItemDuration += i->GetLength();

				size += i->GetObjectSize();
			}

			for (auto & i : *Interface::Separator::GetContainer())
			{
				duration += i.second->SepItemDuration;
			}


			m_Playlist.SetItemsDuration(duration);
			m_Playlist.SetItemsSize(size);

			durationText.SetTextString(utf8_to_utf16(m_Playlist.GetItemsDurationString()));
			itemsSizeText.SetTextString(utf8_to_utf16(m_Playlist.GetItemsSizeString()));
			itemsCountText.SetTextString(std::to_wstring(Audio::Object::GetSize()));

			durationText.ReloadTextTexture();
			itemsSizeText.ReloadTextTexture();
			itemsCountText.ReloadTextTexture();

			updatePlaylistInfo = false;

			// Should I delete it?
			State::SetState(State::AudioAdded);
		}



		/*else if (updatePlaylistInfo == true && State::PathLoadedFromFileConst == true)
		{

			m_Playlist.SetItemsDuration(m_Playlist.GetItemsDuration());
			m_Playlist.SetItemsSize(m_Playlist.GetItemsSize());

			durationText.SetTextString(utf8_to_utf16(m_Playlist.GetItemsDurationString()));
			itemsSizeText.SetTextString(utf8_to_utf16(m_Playlist.GetItemsSizeString()));
			itemsCountText.SetTextString(std::to_wstring(Audio::Object::GetSize()));

			durationText.InitTextTexture();
			itemsSizeText.InitTextTexture();
			itemsCountText.InitTextTexture();

			updatePlaylistInfo = false;

			mdEngine::MP::musicPlayerState = mdEngine::MP::MusicPlayerState::kMusicAdded;
		}*/
	}

	void MP::RenderMainWindow()
	{
		/* Main Bar */
		glm::mat4 model;
		glm::mat4 dotModel;
		f32 dotOffsetXShuffle = 0.4f;
		f32 dotOffsetXRepeat = -0.2f;
		
		Shader::shaderDefault->use();
		Shader::shaderDefault->setVec3("color", Color::White);
		glActiveTexture(GL_TEXTURE0);

		RenderPlaylistWindow();

		RenderVolume();

		RenderMusicProgressBar();

		RenderMusicUI();

		RenderPlaylistInfo();

		RenderPlaylistItemsNew();

		RenderWindowControlButtons();

		RenderSettingsButtons();

		Input::SetButtonExtraState(volumeSliderActive || musicSliderActive || playlistSliderActive || 
								   UI::fileBrowserActive || playlistAddFileActive);


	}

	void MP::CloseMainWindow()
	{
		delete[] textTexture;
		delete[] predefinedPos;
	}


#ifdef _DEBUG_
	void MP::PrintVisibleItemsInfo()
	{
		s32 maxNameLen = 100;
		auto audioCon = Audio::Object::GetAudioObjectContainer();
		for (s32 i = minPos; i < maxPos; i++)
		{
			std::cout << utf16_to_utf8(audioCon[i]->GetTitle());
			for (s32 k = utf16_to_utf8(audioCon[i]->GetTitle()).length(); k < maxNameLen; k++)
				std::cout << " ";
			std::cout << " || ";
			std::cout << "Pos X: ";
			std::cout << audioCon[i]->GetButtonPos().x;
			std::cout << " || ";
			std::cout << "Pos Y: ";
			std::cout << audioCon[i]->GetButtonPos().y;
			std::cout << "\n";
		}
	}

	void MP::PrintAudioObjectInfo()
	{
		for (auto & i : Audio::Object::GetAudioObjectContainer())
		{
			std::cout << "ID: ";
			std::cout << i->GetID();
			std::cout << " || ";
			std::cout << i->GetButtonPos().x;
			std::cout << " || ";
			std::cout << "Pos Y: ";
			std::cout << i->GetButtonPos().y;
			std::cout << "\n";

		}
	}

	void MP::PrintIndexesToRender()
	{
		for (auto i : m_Playlist.GetIndexesToRender())
		{
			std::cout << i << " : ";
		}
		std::cout << "\n";
		std::cout << minPos << "    " << maxPos << std::endl;
	}


#endif
}
}