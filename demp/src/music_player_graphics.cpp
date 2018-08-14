#include "music_player_graphics.h"

#include <gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <algorithm>

#include "md_load_texture.h"
#include "realtime_system_application.h"
#include "music_player_playlist.h"
#include "graphics.h"
#include "md_text.h"
#include "music_player.h"
#include "music_player_string.h"
#include "md_parser.h"
#include "music_player_settings.h"
#include "music_player_ui.h"
#include "md_math.h"
#include "audio/mp_audio.h"
#include "music_player_state.h"

using namespace mdEngine::Graphics;
using namespace mdEngine::MP;
using namespace mdEngine::MP::UI;
using namespace Audio::Object;

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


		b8 texturesLoaded = false;
		GLuint* textTexture = NULL;
		glm::vec2* predefinedPos = NULL;
		b8 predefinedPosLoaded(false);

		s32 allocatedCount = 0;

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

		s32 playlistPreviousOffsetY = 0;
		f32 playlistCurrentOffsetY = 0;
		s32 playlistCurrentPos = 0;
		s32 playlistPreviousPos = 0;
		s32 playlistIndex = 0;

		f32 stretchPlaylistMultplier = 0.f;
		f32 stretchMultiplier = 1.f;
		f32 peekStretchMultiplier = 2.f;

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

		void InitializeTextBoxes();

		void RenderPlaylistWindow();

		void RenderVolume();

		void RenderMusicProgressBar();

		void RenderMusicUI();

		void RenderPlaylistItems();

		void RenderPlaylistButtons();

		void RenderWindowControlButtons();

	}


	MP::PlaylistObject::PlaylistObject()
	{
		m_Enabled = false;
		m_Toggled = false;
		m_SelectedID = -1;
		m_PlayingID = -1;
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

	b8 MP::PlaylistObject::IsEnabled()
	{
		return m_Enabled;
	}

	b8 MP::PlaylistObject::IsToggled()
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

	s32 MP::PlaylistObject::GetSelectedID()
	{
		return m_SelectedID;
	}

	s32 MP::PlaylistObject::GetPlayingID()
	{
		return mdEngine::MP::Playlist::RamLoadedMusic.get() != NULL ? mdEngine::MP::Playlist::RamLoadedMusic.mID : -1;
	}

	glm::vec2 MP::PlaylistObject::getPos()
	{
		return m_Pos;
	}

	glm::vec2 MP::PlaylistObject::getSize()
	{
		return m_Size;
	}

	void MP::PlaylistObject::setPos(glm::vec2 pos)
	{
		m_Pos = pos;
	}

	void MP::PlaylistObject::setSize(glm::vec2 size)
	{
		m_Size = size;
	}

	b8 MP::PlaylistObject::hasFocus()
	{
		int mouseX, mouseY;
		App::Input::GetMousePosition(&mouseX, &mouseY);

		return mouseX > m_Pos.x && mouseX < m_Size.x &&
			   mouseY > m_Pos.y && mouseY < m_Size.y;
	}

	b8 MP::MainPlayerObject::hasFocus()
	{
		int mouseX, mouseY;
		App::Input::GetMousePosition(&mouseX, &mouseY);

		return mouseX > m_Pos.x && mouseX < m_Size.x &&
			mouseY > m_Pos.y && mouseY < m_Size.y;
	}

	void MP::MainPlayerObject::setPos(glm::vec2 pos)
	{
		m_Pos = pos;
	}

	void MP::MainPlayerObject::setSize(glm::vec2 size)
	{
		m_Size = size;
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
		m_AddFileTextBox.SetColor(Color::White);
	}

	void MP::InitializeConfig()
	{
		std::string file = Strings::_SETTINGS_FILE;
		shuffleActive = Parser::GetInt(file, Strings::_SHUFFLE_STATE);
		repeatActive = Parser::GetInt(file, Strings::_REPEAT_STATE);
		Parser::GetInt(file, Strings::_PLAYLIST_STATE) == 1 ? (m_Playlist.Toggle(), m_Playlist.Enable()) : m_Playlist.UnToggle();

		mdEngine::MP::musicPlayerState = mdEngine::MP::MusicPlayerState::kMusicAdded;
	}

	void MP::RenderPlaylistWindow()
	{
		static s32 toggledWindowHeight = Window::windowProperties.mApplicationHeight;

		m_Playlist.setPos(glm::vec2(0.f, Data::_PLAYLIST_FOREGROUND_POS.y));
		m_Playlist.setSize(glm::vec2(Window::windowProperties.mWindowWidth, Window::windowProperties.mApplicationHeight));
		m_MainPlayer.setPos(glm::vec2(0.f, 0.f));
		m_MainPlayer.setSize(Data::_DEFAULT_PLAYER_SIZE);

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

			if (Input::hasFocus(Input::ButtonType::SliderVolume) && App::Input::IsKeyDown(App::KeyCode::MouseLeft) && 
								musicSliderActive == false && Window::windowProperties.mActualWindowEvent != App::WindowEvent::kResize)
				volumeSliderActive = true;
			else if (App::Input::IsKeyDown(App::KeyCode::MouseLeft) == false)
				volumeSliderActive = false;


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

		if (Input::hasFocus(Input::ButtonType::SliderMusic) && App::Input::IsKeyDown(App::KeyCode::MouseLeft) && 
							volumeSliderActive == false && Window::windowProperties.mActualWindowEvent != App::WindowEvent::kResize)
			musicSliderActive = true;
		else if (App::Input::IsKeyDown(App::KeyCode::MouseLeft) == false)
			musicSliderActive = false;

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

	void MP::RenderPlaylistItems()
	{

		if (m_Playlist.IsToggled() && m_Playlist.IsEnabled() && State::IsPlaylistEmpty == false)
		{
			//RenderPlaylistButtons();

			if (::GetSize() == 0)
				return;

			if (mdEngine::App::Input::IsScrollForwardActive() && m_Playlist.hasFocus())
			{
				playlistCurrentOffsetY += Data::PlaylistScrollStep;
			}
			if (mdEngine::App::Input::IsScrollBackwardActive() && m_Playlist.hasFocus())
			{
				playlistCurrentOffsetY -= Data::PlaylistScrollStep;
			}

			s32 itemH = ::GetAudioObject(0)->GetPlaylistItem()->mSize.y;
			s32 displayedItems = (Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y - Data::_PLAYLIST_ITEMS_SURFACE_POS.y) / itemH;
			s32 maxItems = ::GetSize();

			if (displayedItems > maxItems)
				displayedItems = maxItems;

			if (displayedItems < 0)
				displayedItems = 0;

			s32 currentSongID = mdEngine::MP::Playlist::RamLoadedMusic.mID;

			
			f32 top = playlistCurrentOffsetY;
			f32 low = top - displayedItems * itemH;
			f32 currentPos = 0;

			currentPos = currentSongID * itemH * -1;
			if (mdEngine::MP::musicPlayerState == mdEngine::MP::MusicPlayerState::kMusicChanged ||
				mdEngine::MP::musicPlayerState == mdEngine::MP::MusicPlayerState::kMusicAdded)
			{
				while (currentPos + itemH > top)
				{
					top += itemH;
					playlistCurrentOffsetY += itemH;
				}
				
				while (currentPos - 2 *itemH < low)
				{
					low -= itemH;
					playlistCurrentOffsetY -= itemH;
				}
				
				if (currentPos >= 0)
					playlistCurrentOffsetY -= itemH;
				//if(currentPos <= (maxItems - 1) * itemH * -1)
					//testOffsetY += itemH;
			}

			if (playlistCurrentOffsetY > 0)
				playlistCurrentOffsetY = 0;


			/* When playlist pos is different than top and low, keep resizing till it reaches end, then start roll out to the top */
			if (mdEngine::MP::musicPlayerState == mdEngine::MP::MusicPlayerState::kResized &&
				Window::windowProperties.mDeltaHeightResize > 0 &&
				playlistCurrentOffsetY <= (maxItems - displayedItems) * itemH * -1 &&
				displayedItems < maxItems)
			{
				playlistCurrentOffsetY += Window::windowProperties.mDeltaHeightResize;
			}
			else if(mdEngine::MP::musicPlayerState != mdEngine::MP::MusicPlayerState::kResized)
			{
				if (playlistCurrentOffsetY < (maxItems - displayedItems) * itemH * -1)
					playlistCurrentOffsetY = (maxItems - displayedItems) * itemH * -1;
			}
			
			playlistCurrentPos = (s32)(playlistCurrentOffsetY / Data::_PLAYLIST_ITEM_SIZE.y) * -1;

			if (mdEngine::MP::musicPlayerState != mdEngine::MP::MusicPlayerState::kIdle)
			{
				texturesLoaded = false;
				
				if (mdEngine::MP::musicPlayerState != mdEngine::MP::MusicPlayerState::kMusicDeleted)
					mdEngine::MP::musicPlayerState = mdEngine::MP::MusicPlayerState::kIdle;
						
			}
			
			s32 min = playlistCurrentPos - 2;
			if (min < 0)
				min = 0;
			s32 max = displayedItems + playlistCurrentPos + 2;
			if (max > ::GetSize())
				max = ::GetSize();
			s32 visibleSpectrum = max - min;


			if (playlistFirstEnter && playlistCurrentOffsetY > 80)
			{
				playlistPreviousPos = playlistCurrentPos;
				playlistPreviousOffsetY = playlistCurrentOffsetY;
				
				playlistFirstEnter = false;
			}

			if (abs(playlistPreviousOffsetY - playlistCurrentOffsetY) > 0)
			{
				for (u32 i = min; i < max; i++)
				{
					if(playlistPreviousOffsetY - playlistCurrentOffsetY > 0)
						::GetAudioObject(i)->GetPlaylistItem()->mPos.y -= abs(playlistPreviousOffsetY - playlistCurrentOffsetY);
					else
						::GetAudioObject(i)->GetPlaylistItem()->mPos.y += abs(playlistPreviousOffsetY - playlistCurrentOffsetY);
				}
				playlistPreviousOffsetY = playlistCurrentOffsetY;
			}

			if (playlistCurrentPos != playlistPreviousPos)
				texturesLoaded = false;

			if (texturesLoaded == false)
			{
				playlistPreviousPos = playlistCurrentPos;
				// Load all textures that are in in display range 
				if (textTexture != NULL)
				{
					glDeleteTextures(allocatedCount, textTexture);
					delete[] textTexture;
				}

				if (predefinedPos != NULL)
					delete[] predefinedPos;

				textTexture = new GLuint[visibleSpectrum];
				predefinedPos = new glm::vec2[visibleSpectrum];

				allocatedCount = visibleSpectrum;

				
				// Predefine positions user can actually see
				Interface::PlaylistItem* item = NULL;
				s32 itemsOffset = 0;
				glm::vec2 itemStartPos = glm::vec2(Data::_PLAYLIST_ITEMS_SURFACE_POS.x, 
												   Data::_PLAYLIST_ITEMS_SURFACE_POS.y + (min - 1) * itemH);
				//std::cout << itemStartPos.y << std::endl;
				predefinedPos[0] = glm::vec2(itemStartPos.x, itemStartPos.y);
				for (u16 i = min, t = 0; i < max; i++, t++)
				{
					item = ::GetAudioObject(i)->GetPlaylistItem();
					textTexture[t] = Text::LoadText(item->mFont,
													item->GetTitle(),
													item->mTextColor);

					if (t != -1)
					{

						predefinedPos[t] = glm::vec2(itemStartPos.x, itemStartPos.y + Data::_PLAYLIST_ITEM_SIZE.y + item->mOffsetY);
						itemStartPos = predefinedPos[t];
						predefinedPos[t].y += playlistCurrentOffsetY;
	
					}
					
				}
				// Write predefined positions to playlist items that are currently displayed
				for (u32 i = 0, t = 0; i < maxItems; i++)
				{
					if (i >= min && i < max)
					{
						::GetAudioObject(i)->GetPlaylistItem()->mPos = glm::vec2(predefinedPos[t].x, predefinedPos[t].y);
						//std::cout << mdItemContainer[i]->mPos.y << "   " << playlistCurrentOffsetY << std::endl;
						t++;
					}
					else
					{
						::GetAudioObject(i)->GetPlaylistItem()->mPos = glm::vec2(INVALID);
					}
					//std::cout << mdItemContainer[i]->mPos.y << "   " << playlistCurrentPos << "  " << playlistCurrentOffsetY << std::endl;
				}

				texturesLoaded = true;
			}


			// SCROLL BAR
			glm::mat4 model;
			if (displayedItems < ::GetSize())
			{
				f32 displayedItemsF = (Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y - Data::_PLAYLIST_ITEMS_SURFACE_POS.y) / itemH;
				f32 playlistSurface = Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y - Data::_PLAYLIST_ITEMS_SURFACE_POS.y;
				f32 scaleY = playlistSurface  / (float)::GetSize() * displayedItemsF;
				f32 scrollSurface = playlistSurface - scaleY;
				f32 scrollStep = (playlistCurrentOffsetY * scrollSurface) / ((maxItems - displayedItems) * itemH);
				s32 newPosY = (Data::_PLAYLIST_SCROLL_BAR_POS.y - scrollStep);


				assert(m_PlaylistBarSlider != NULL);
				
				m_PlaylistBarSlider->mSize.y = scaleY;
				m_PlaylistBarSlider->mPos.y = newPosY;

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
					playlistSliderActive = false;
					playlistSliderFirstEnter = true;
				}
			

				if (playlistSliderActive)
				{
					deltaMouseY = mouseY - lastMouseY;
					lastMouseY = mouseY;
					playlistCurrentOffsetY -= (deltaMouseY * maxItems / displayedItems);
				}

				if (m_PlaylistBarSlider->mSize.y < Data::_PLAYLIST_SCROLL_BAR_SIZE.y)
					m_PlaylistBarSlider->mSize.y = Data::_PLAYLIST_SCROLL_BAR_SIZE.y;
				if (m_PlaylistBarSlider->mPos.y < Data::_PLAYLIST_SCROLL_BAR_POS.y)
					m_PlaylistBarSlider->mPos.y = Data::_PLAYLIST_SCROLL_BAR_POS.y;
				if (m_PlaylistBarSlider->mPos.y + m_PlaylistBarSlider->mSize.y > Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y)
					m_PlaylistBarSlider->mPos.y = Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y - m_PlaylistBarSlider->mSize.y;

				model = glm::mat4();
				model = glm::translate(model, glm::vec3(Data::_PLAYLIST_SCROLL_BAR_POS.x, m_PlaylistBarSlider->mPos.y, 0.9f));
				model = glm::scale(model, glm::vec3(Data::_PLAYLIST_SCROLL_BAR_SIZE.x, m_PlaylistBarSlider->mSize.y, 1.f));
				Shader::shaderDefault->setVec3("color", Color::Black);
				Shader::shaderDefault->setMat4("model", model);
				glBindTexture(GL_TEXTURE_2D, 0);
				Shader::Draw(Shader::shaderDefault);
			}

			// Cut all items and item border if it is out of bou
			Shader::shaderDefault->setBool("playlistCut", true);
			Shader::shaderDefault->setFloat("playlistMinY", Data::_PLAYLIST_ITEMS_SURFACE_POS.y);
			Shader::shaderDefault->setFloat("playlistMaxY", Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y);
			Shader::shaderBorder->use();
			Shader::shaderBorder->setFloat("playlistMinY", Data::_PLAYLIST_ITEMS_SURFACE_POS.y);
			Shader::shaderBorder->setFloat("playlistMaxY", Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y);

			u16 texIndex = 0;
			playlistIndex = min;
			while (playlistIndex < max)
			{
				/* Playlist items are rendered on the same positions as positions of the first(0 - maxItems), 
				   they are only transformed by matrix, so when you want to check if specific item has focus, 
				   it will check the focus on the wrong item(0 - maxItems). To prevent that, always add mOffsetIndex,
				   do checked positions, so you will get index of actuall rendered item. */
				Interface::PlaylistItem::mOffsetIndex = playlistCurrentPos;
				Audio::AudioObject* aItem = ::GetAudioObject(playlistIndex);
				Interface::PlaylistItem* pItem = ::GetAudioObject(playlistIndex)->GetPlaylistItem();
				glm::vec3 itemColor;

				// Fint in vector with selected positions if current's id is inside
				auto it = std::find(m_Playlist.multipleSelect.begin(),
									m_Playlist.multipleSelect.end(),
									&aItem->GetID());

				if (m_Playlist.GetPlayingID() && it != m_Playlist.multipleSelect.end())
				{
					itemColor *= Color::Red * Color::Grey;
					pItem->DrawDottedBorder(playlistCurrentPos);
				}
				else if (aItem->GetID() == m_Playlist.GetPlayingID())
				{
					itemColor = Color::Red * Color::Grey;
				}
				else if (it != m_Playlist.multipleSelect.end())
				{
					itemColor = Color::Grey;
					pItem->DrawDottedBorder(playlistCurrentPos);
				}
				else
				{
					itemColor = pItem->mColor;
				}

				// Draw border for every selected item
				if (it != m_Playlist.multipleSelect.end())
				{
					if (aItem->GetID() == m_Playlist.GetPlayingID())
					{
						itemColor = Color::Red * Color::Grey;
						pItem->DrawDottedBorder(playlistCurrentPos);
					}
					else
					{
						itemColor = Color::Grey;
						pItem->DrawDottedBorder(playlistCurrentPos);
					}
				}
				//if (item->clickCount == 1)
					//std::cout << (float)item->mTextSize.x * item->mTextScale << std::endl;
			
				Shader::shaderDefault->use();
				model = glm::mat4();
				model = glm::translate(model, glm::vec3(pItem->mPos.x, pItem->mPos.y, 0.5f));
				model = glm::scale(model, glm::vec3(pItem->mSize, 1.0f));
				Shader::shaderDefault->setMat4("model", model);
				Shader::shaderDefault->setVec3("color", itemColor);
				glBindTexture(GL_TEXTURE_2D, main_foreground);
				Shader::Draw(Shader::shaderDefault);

				if (pItem->mPos.y < Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y)
				{
					glm::vec3 color(1.f);
					model = glm::mat4();
					model = glm::translate(model, glm::vec3(pItem->mPos.x + 5.f, pItem->mPos.y + pItem->mSize.y / 4.f, 0.8f));
					model = glm::scale(model, glm::vec3((float)pItem->mTextSize.x * pItem->mTextScale,
						(float)pItem->mTextSize.y * pItem->mTextScale, 1.0f));
					Shader::shaderDefault->setMat4("model", model);
					Shader::shaderDefault->setVec3("color", Color::White);
					glBindTexture(GL_TEXTURE_2D, textTexture[texIndex]);
					Shader::Draw(Shader::shaderDefault);
				}


				texIndex++;
				playlistIndex++;
			}

			Shader::shaderDefault->setBool("playlistCut", false);

			Shader::shaderDefault->setVec3("color", Color::White);
		}
	}

	void MP::RenderPlaylistButtons()
	{
		m_AddFileButtonRef->mPos = Data::_PLAYLIST_ADD_BUTTON_POS;
		m_AddFileButtonRef->mSize = Data::_PLAYLIST_ADD_BUTTON_SIZE;


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
		}

		model = glm::translate(model, glm::vec3(Data::_PLAYLIST_ADD_BUTTON_POS, 0.6));
		model = glm::scale(model, glm::vec3(Data::_PLAYLIST_ADD_BUTTON_SIZE, 1.0));
		Shader::shaderDefault->setMat4("model", model);
		Shader::shaderDefault->setVec3("color", color);
		glBindTexture(GL_TEXTURE_2D, playlist_add_file);
		Shader::Draw(Shader::shaderDefault);

		Shader::shaderDefault->setVec3("color", Color::White);
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

		InitializeTextBoxes();
	}

	void MP::UpdateMainWindow()
	{

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

		RenderPlaylistItems();

		RenderWindowControlButtons();

		Input::SetButtonExtraState(volumeSliderActive || musicSliderActive || playlistSliderActive || 
								   UI::fileBrowserActive || playlistAddFileActive);
	}

	void MP::CloseMainWindow()
	{
		delete[] textTexture;
		delete[] predefinedPos;
	}

}
}