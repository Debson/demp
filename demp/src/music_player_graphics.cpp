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

using namespace mdEngine::Graphics;
using namespace mdEngine::MP;
using namespace mdEngine::MP::UI;

namespace mdEngine
{
namespace Graphics
{
	namespace MP
	{
		PlaylistObject playlist;
		Button* playlistBarSlider;

		GLuint main_background = 0, main_foreground = 0;
		GLuint exit_background, exit_icon, minimize_icon, stay_on_top_icon;
		GLuint volume_bar, volume_speaker, volume_speaker_muted;
		GLuint play_button, stop_button, next_button, previous_button, shuffle_button, repeat_button, dot_icon, playlist_button;
		GLuint music_progress_bar;

		b8 texturesLoaded = false;
		GLuint* textTexture = NULL;
		glm::vec2* predefinedPos = NULL;
		b8 predefinedPosLoaded(false);

		b8 shuffleActive(false);
		b8 repeatActive(false);
		b8 playActive(false);
		b8 volumeMuted(false);
		b8 volumeSliderActive(false);
		b8 musicSliderActive(false);
		b8 playlistActive(false);

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
		b8 playlistSliderActive(false);
		b8 playlistSliderFirstEnter(true);
		s32 mouseX, mouseY;
		s32 lastMouseX = 0, lastMouseY = 0;
		s32 deltaMouseY = 0;
			

		void InitializeConfig();

		void RenderPlaylistWindow();

		void RenderVolume();

		void RenderMusicProgressBar();

		void RenderMusicUI();

		void RenderPlaylistItems();

		void RenderPlaylistScrollBar();

		void RenderWindowControlButtons();

	}


	MP::PlaylistObject::PlaylistObject()
	{
		currTime = 0;
		prevTime = 0;
		enabled = false;
		toggled = false;
		unwindTime = 0;
		selectedID = -1;
		playingID = -1;
	}

	void MP::PlaylistObject::Enable()
	{
		timer.start();
		enabled = true;
	}

	void MP::PlaylistObject::Disable()
	{
		timer.start();
		enabled = false;
	}

	void MP::PlaylistObject::Update()
	{
		timer.update();

		prevTime = currTime;
		currTime = timer.currentTime;

		if (timer.finished == true && enabled == true && toggled == false)
		{
			toggled = true;
			timer.reset();
		}

		if (timer.finished == true && enabled == false && toggled == true)
		{
			toggled = false;
			timer.reset();
		}
	}

	b8 MP::PlaylistObject::IsEnabled()
	{
		return enabled;
	}

	b8 MP::PlaylistObject::IsToggled()
	{
		return toggled;
	}

	b8 MP::PlaylistObject::IsRolling()
	{
		return prevTime != currTime;
	}

	void MP::PlaylistObject::SetSelectedID(s32 id)
	{
		selectedID = id;
	}

	void MP::PlaylistObject::SetPlayingID(s32 id)
	{
		playingID = id;
	}

	s32 MP::PlaylistObject::GetSelectedID()
	{
		return selectedID;
	}

	s32 MP::PlaylistObject::GetPlayingID()
	{
		return mdEngine::MP::Playlist::RamLoadedMusic.get() != NULL ? mdEngine::MP::Playlist::RamLoadedMusic.mID : -1;
	}

	void MP::PlaylistObject::SetRollTime(s16 time)
	{
		timer.targetTime = time;
	}

	f32 MP::PlaylistObject::GetRollProgress()
	{
		return timer.progress();
	}

	void MP::InitializeConfig()
	{
		std::string file = Strings::_SETTINGS_FILE;
		shuffleActive = Parser::GetInt(file, Strings::_SHUFFLE_STATE);
		repeatActive = Parser::GetInt(file, Strings::_REPEAT_STATE);
		Parser::GetInt(file, Strings::_PLAYLIST_STATE) == 1 ? playlist.Enable() : (void)0;
		//Window::windowProperties.mApplicationHeight = Parser::GetInt(file, Strings::_APP_HEIGHT);

		mdEngine::MP::musicPlayerState = mdEngine::MP::MusicPlayerState::kMusicChanged;
	}

	void MP::RenderPlaylistWindow()
	{
		glm::mat4 model;

		f32 scaleY = (float)Window::windowProperties.mApplicationHeight / Data::_DEFAULT_WINDOW_SIZE.y;

		if (playlist.IsRolling() && playlist.IsEnabled() == true)
		{
			stretchMultiplier = (1.f + playlist.GetRollProgress());
			stretchPlaylistMultplier = playlist.GetRollProgress();
			peekStretchMultiplier = stretchMultiplier;
		}
		else if (playlist.IsRolling())
		{
			stretchMultiplier = (peekStretchMultiplier  - playlist.GetRollProgress() * (peekStretchMultiplier - 1));
			stretchPlaylistMultplier = 1.f - playlist.GetRollProgress();
		}
		if (playlist.IsToggled() && playlist.IsEnabled())
		{
			stretchMultiplier = 2.f * scaleY;
			stretchPlaylistMultplier = (2.f * scaleY) - 1.f;
		}



		/* Main background*/
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(Data::_MAIN_BACKGROUND_POS.x, Data::_MAIN_BACKGROUND_POS.y, 0.f));
		model = glm::scale(model, glm::vec3(Data::_MAIN_BACKGROUND_SIZE.x, Data::_MAIN_BACKGROUND_SIZE.y * stretchMultiplier, 1.f));;
		Shader::shaderDefault->setMat4("model", model);
		Shader::shaderDefault->setBool("cut", true);
		glBindTexture(GL_TEXTURE_2D, main_background);
		Shader::Draw();
		Shader::shaderDefault->setBool("cut", false);

		// Main foreground
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(Data::_MAIN_FOREGROUND_POS, 0.1f));
		model = glm::scale(model, glm::vec3(Data::_MAIN_FOREGROUND_SIZE, 1.f));
		Shader::shaderDefault->setMat4("model", model);
		Shader::shaderDefault->setBool("cut", true);
		glBindTexture(GL_TEXTURE_2D, main_foreground);
		Shader::Draw();;
		Shader::shaderDefault->setBool("cut", false);

		// Playlist foreground

		if (stretchPlaylistMultplier > 0.05)
		{
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(Data::_PLAYLIST_FOREGROUND_POS, 0.4f));
			model = glm::scale(model, glm::vec3(Data::_PLAYLIST_FOREGROUND_SIZE.x, Data::_PLAYLIST_FOREGROUND_SIZE.y * stretchPlaylistMultplier, 1.f));
			Shader::shaderDefault->setMat4("model", model);
			Shader::shaderDefault->setBool("cut", true);
			glBindTexture(GL_TEXTURE_2D, main_foreground);
			Shader::Draw();
			Shader::shaderDefault->setBool("cut", false);
		}

		if (Input::isButtonPressed(Input::ButtonType::Playlist) && playlist.IsRolling() == false)
		{
			if (playlist.IsEnabled() == true)
				playlist.Disable();
			else
				playlist.Enable();
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
			Shader::Draw();
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
			Shader::Draw();
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
				Shader::Draw();
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
				Shader::Draw();
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
				Shader::Draw();
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
				Shader::Draw();
				Shader::shaderDefault->setVec3("color", Color::White);
			}


			/* Volume bar under volume slider */
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(Data::_VOLUME_BAR_POS, 0.3f));
			model = glm::scale(model, glm::vec3(Data::_VOLUME_BAR_SIZE, 1.f));
			Shader::shaderDefault->setMat4("model", model);
			Shader::shaderDefault->setVec3("color", Color::DarkGrey);
			glBindTexture(GL_TEXTURE_2D, volume_bar);
			Shader::Draw();
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
				Shader::Draw();

				model = glm::mat4();
				model = glm::translate(model, glm::vec3(Data::_VOLUME_BAR_POS, 0.35f));
				model = glm::scale(model, glm::vec3(deltaVolumePos, Data::_VOLUME_BAR_SIZE.y, 1.f));
				Shader::shaderDefault->setMat4("model", model);
				Shader::shaderDefault->setVec3("color", Color::Green);
				glBindTexture(GL_TEXTURE_2D, volume_bar);
				Shader::Draw();
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
				Shader::Draw();
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
		Shader::Draw();
		Shader::shaderDefault->setVec3("color", Color::White);

		if (Input::hasFocus(Input::ButtonType::SliderMusic) || musicSliderActive)
		{
			Shader::shaderDefault->setVec3("color", Color::Green);
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(Data::_MUSIC_PROGRESS_BAR_POS, 0.35f));
			model = glm::scale(model, glm::vec3(deltaMusicPos, Data::_MUSIC_PROGRESS_BAR_SIZE.y, 1.f));
			Shader::shaderDefault->setMat4("model", model);
			glBindTexture(GL_TEXTURE_2D, music_progress_bar);
			Shader::Draw();
			Shader::shaderDefault->setVec3("color", Color::White);


			model = glm::mat4();
			model = glm::translate(model, glm::vec3(Data::_MUSIC_PROGRESS_BAR_POS.x + deltaMusicPos - Data::_SLIDER_DOT_SIZE.x / 2.f, Data::_MUSIC_PROGRESS_BAR_DOT_POS.y, 0.4));
			lastMousePos = deltaVolumePos;
			model = glm::scale(model, glm::vec3(Data::_SLIDER_DOT_SIZE, 1.f));
			Shader::shaderDefault->setMat4("model", model);
			glBindTexture(GL_TEXTURE_2D, dot_icon);
			Shader::Draw();


		}
		else
		{
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(Data::_MUSIC_PROGRESS_BAR_POS, 0.35f));
			model = glm::scale(model, glm::vec3(deltaMusicPos, Data::_MUSIC_PROGRESS_BAR_SIZE.y, 1.f));
			Shader::shaderDefault->setMat4("model", model);
			glBindTexture(GL_TEXTURE_2D, music_progress_bar);
			Shader::Draw();
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
			Shader::Draw();

		}
		Shader::shaderDefault->setVec3("color", color);
		Shader::shaderDefault->setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, shuffle_button);
		Shader::Draw();


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
		Shader::Draw();


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
			Shader::Draw();
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
			Shader::Draw();
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
		Shader::Draw();


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
			Shader::Draw();
		}
		Shader::shaderDefault->setVec3("color", color);
		Shader::shaderDefault->setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, repeat_button);
		Shader::Draw();

		Shader::shaderDefault->setVec3("color", Color::White);		
	}

	void MP::RenderPlaylistItems()
	{
		glm::mat4 model;

		if (playlist.IsToggled() && mdItemContainer.size() > 0)
		{
			if (mdEngine::App::Input::IsScrollForwardActive())
			{
				playlistCurrentOffsetY += Data::PlaylistScrollStep;
			}
			if (mdEngine::App::Input::IsScrollBackwardActive())
			{
				playlistCurrentOffsetY -= Data::PlaylistScrollStep;
			}

			s32 displayedItems = (Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y - Data::_PLAYLIST_ITEMS_SURFACE_POS.y) / mdItemContainer[0]->mSize.y;
			s32 maxItems = mdItemContainer.size();

			if (displayedItems > maxItems)
				displayedItems = maxItems;

			s32 currentSongID = mdEngine::MP::Playlist::RamLoadedMusic.mID;
			s32 itemH = mdItemContainer[0]->mSize.y;

			
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

			/* When playlist pos is different than top and low, resize till reach low then change offset to reach top */
			if (mdEngine::MP::musicPlayerState == mdEngine::MP::MusicPlayerState::kResized &&
				Window::windowProperties.mDeltaHeightResize > 0 &&
				playlistCurrentOffsetY <= (maxItems - displayedItems) * itemH * -1)
			{
				playlistCurrentOffsetY += Window::windowProperties.mDeltaHeightResize;
			}
			else if(mdEngine::MP::musicPlayerState != mdEngine::MP::MusicPlayerState::kResized)
			{
				if (playlistCurrentOffsetY < (maxItems - displayedItems) * itemH * -1)
					playlistCurrentOffsetY = (maxItems - displayedItems) * itemH * -1;
			}
			
			playlistCurrentPos = (s32)(playlistCurrentOffsetY / Data::_PLAYLIST_ITEM_SIZE.y) * -1;

			//std::cout << playlistCurrentPos << std::endl;

			if (mdEngine::MP::musicPlayerState != mdEngine::MP::MusicPlayerState::kIdle)
			{
				texturesLoaded = false;
				
				if (mdEngine::MP::musicPlayerState != mdEngine::MP::MusicPlayerState::kMusicDeleted)
					mdEngine::MP::musicPlayerState = mdEngine::MP::MusicPlayerState::kIdle;
						
			}
			//std::cout << testOffsetY << std::endl;
			s32 min = playlistCurrentPos - 2;
			if (min < 0)
				min = 0;
			s32 max = displayedItems + playlistCurrentPos + 2;
			if (max > mdItemContainer.size())
				max = mdItemContainer.size();
			s32 visibleSpectrum = max - min;

			{ // text debug
				//std::cout << "Min: " << min << "  Max: " << max << std::endl;
				//std::cout << playlistCurrentOffsetY << std::endl;
				//std::cout << "\n\n\n\n\n\n\n\n";
				for (u32 i = min; i < max; i++)
				{
					//std::cout << mdItemContainer[i]->mPos.y << std::endl;
				}
			}

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
						mdItemContainer[i]->mPos.y -= abs(playlistPreviousOffsetY - playlistCurrentOffsetY);
					else
						mdItemContainer[i]->mPos.y += abs(playlistPreviousOffsetY - playlistCurrentOffsetY);
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
					glDeleteTextures(visibleSpectrum, textTexture);
					delete[] textTexture;
				}

				if (predefinedPos != NULL)
					delete[] predefinedPos;

				textTexture = new GLuint[visibleSpectrum];
				predefinedPos = new glm::vec2[visibleSpectrum];


				// Predefine positions user can actually see
				PlaylistItem* item = NULL;
				s32 itemsOffset = 0;
				glm::vec2 itemStartPos = glm::vec2(Data::_PLAYLIST_ITEMS_SURFACE_POS.x, 
												   Data::_PLAYLIST_ITEMS_SURFACE_POS.y + (min - 1) * itemH);
				//std::cout << itemStartPos.y << std::endl;
				predefinedPos[0] = glm::vec2(itemStartPos.x, itemStartPos.y);
				for (u16 i = min, t = 0; i < max; i++, t++)
				{
					item = mdItemContainer[i];
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
						mdItemContainer[i]->mPos = glm::vec2(predefinedPos[t].x, predefinedPos[t].y);
						//std::cout << mdItemContainer[i]->mPos.y << "   " << playlistCurrentOffsetY << std::endl;
						t++;
					}
					else
					{
						mdItemContainer[i]->mPos = glm::vec2(INVALID);
					}
					//std::cout << mdItemContainer[i]->mPos.y << "   " << playlistCurrentPos << "  " << playlistCurrentOffsetY << std::endl;
				}

				texturesLoaded = true;
			}


			// SCROLL BAR
			if (displayedItems < mdItemContainer.size())
			{
				//std::cout << testOffsetY << std::endl;
				f32 playlistSurface = Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y - Data::_PLAYLIST_ITEMS_SURFACE_POS.y;
				s32 scaleY = playlistSurface  / mdItemContainer.size() * displayedItems;
				f32 scrollSurface = playlistSurface - scaleY;
				//std::cout << scrollSurface << std::endl;
				f32 scrollStep = (playlistCurrentOffsetY * scrollSurface) / ((maxItems - displayedItems) * itemH);
				s32 newPosY = (Data::_PLAYLIST_SCROLL_BAR_POS.y - scrollStep);

				playlistBarSlider->mPos.y = newPosY;

				if(playlistBarSlider != NULL)
					playlistBarSlider->mSize.y = scaleY;

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

				Input::SetButtonExtraState(playlistSliderActive);

				if (playlistSliderActive)
				{
					deltaMouseY = mouseY - lastMouseY;
					lastMouseY = mouseY;
					playlistCurrentOffsetY -= deltaMouseY;
				}

				if (playlistBarSlider->mPos.y < Data::_PLAYLIST_SCROLL_BAR_POS.y)
					playlistBarSlider->mPos.y = Data::_PLAYLIST_SCROLL_BAR_POS.y;
				if (playlistBarSlider->mPos.y + playlistBarSlider->mSize.y > Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y)
					playlistBarSlider->mPos.y = Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y - playlistBarSlider->mSize.y;

				model = glm::mat4();
				model = glm::translate(model, glm::vec3(Data::_PLAYLIST_SCROLL_BAR_POS.x, newPosY, 0.9f));
				model = glm::scale(model, glm::vec3(Data::_PLAYLIST_SCROLL_BAR_SIZE.x, scaleY, 1.f));
				Shader::shaderDefault->setVec3("color", Color::Black);
				Shader::shaderDefault->setMat4("model", model);
				glBindTexture(GL_TEXTURE_2D, 0);
				Shader::Draw();
			}


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
				PlaylistItem::mOffsetIndex = playlistCurrentPos;
				PlaylistItem* item = mdItemContainer[playlistIndex];
				glm::vec3 itemColor;

				// Fint in vector with selected positions if current's id is inside
				auto it = std::find(playlist.multipleSelect.begin(),
									playlist.multipleSelect.end(),
									&item->mID);

				if (playlist.GetPlayingID() && it != playlist.multipleSelect.end())
				{
					itemColor *= Color::Red * Color::Grey;
					item->DrawDottedBorder(playlistCurrentPos);
				}
				else if (item->mID == playlist.GetPlayingID())
				{
					itemColor = Color::Red * Color::Grey;
				}
				else if (it != playlist.multipleSelect.end())
				{
					itemColor = Color::Grey;
					item->DrawDottedBorder(playlistCurrentPos);
				}
				else
				{
					itemColor = item->mColor;
				}

				// Draw border for every selected item
				if (it != playlist.multipleSelect.end())
				{
					if (item->mID == playlist.GetPlayingID())
					{
						itemColor = Color::Red * Color::Grey;
						item->DrawDottedBorder(playlistCurrentPos);
					}
					else
					{
						itemColor = Color::Grey;
						item->DrawDottedBorder(playlistCurrentPos);
					}
				}
				//if (item->clickCount == 1)
					//std::cout << (float)item->mTextSize.x * item->mTextScale << std::endl;
			
				Shader::shaderDefault->use();
				model = glm::mat4();
				model = glm::translate(model, glm::vec3(item->mPos.x, item->mPos.y, 0.5f));
				model = glm::scale(model, glm::vec3(item->mSize, 1.0f));
				Shader::shaderDefault->setMat4("model", model);
				Shader::shaderDefault->setVec3("color", itemColor);
				glBindTexture(GL_TEXTURE_2D, main_foreground);
				Shader::Draw();

				glm::vec3 color(1.f);
				model = glm::mat4();
				model = glm::translate(model, glm::vec3(item->mPos.x + 5.f, item->mPos.y + item->mSize.y / 4.f, 0.8f));
				model = glm::scale(model, glm::vec3((float)item->mTextSize.x * item->mTextScale,
													(float)item->mTextSize.y * item->mTextScale, 1.0f));
				Shader::shaderDefault->setMat4("model", model);
				Shader::shaderDefault->setVec3("color", Color::White);
				glBindTexture(GL_TEXTURE_2D, textTexture[texIndex]);
				Shader::Draw();


				texIndex++;
				playlistIndex++;
			}

			Shader::shaderDefault->setBool("playlistCut", false);

			Shader::shaderDefault->setVec3("color", Color::White);
		}
	}

	void MP::RenderPlaylistScrollBar()
	{
		if (playlist.IsToggled() && mdItemContainer.size() > 0)
		{
			s32 maxItems = (Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y - Data::_PLAYLIST_ITEMS_SURFACE_POS.y) / mdItemContainer[0]->mSize.y;

			if (maxItems > mdItemContainer.size())
				return;

			glm::mat4 model;
			model = glm::translate(model, glm::vec3(Data::_PLAYLIST_SCROLL_BAR_POS, 0.9f));
			model = glm::scale(model, glm::vec3(Data::_PLAYLIST_SCROLL_BAR_SIZE.x,
												Data::_PLAYLIST_SCROLL_BAR_SIZE.y * stretchPlaylistMultplier / mdItemContainer.size() * maxItems, 
												1.f));
			Shader::shaderDefault->setVec3("color", Color::Black);
			Shader::shaderDefault->setMat4("model", model);
			glBindTexture(GL_TEXTURE_2D, 0);
			Shader::Draw();
		}
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
		Shader::Draw();

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(Data::_EXIT_BUTTON_POS, 0.3f));
		model = glm::scale(model, glm::vec3(Data::_EXIT_BUTTON_SIZE, 1.f));;
		Shader::shaderDefault->setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, exit_icon);
		Shader::Draw();
	}

	void MP::StartMainWindow()
	{
		InitializeConfig();


		playlist.SetRollTime(200);

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

		music_progress_bar		= mdLoadTexture("assets/music_progress_bar.png");

		deltaVolumePos = (s32)(mdEngine::MP::Playlist::GetVolume() * 100.f * 0.9f);

		auto it = std::find_if(mdButtonsContainer.begin(), mdButtonsContainer.end(),
			[&](std::pair<Input::ButtonType, Button*> const& ref) { return ref.first == Input::ButtonType::SliderPlaylist; });

		if (it != mdButtonsContainer.end())
			playlistBarSlider = it->second;

	}

	void MP::UpdateMainWindow()
	{
		playlist.Update();

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

		//RenderPlaylistScrollBar();

		RenderWindowControlButtons();


	}

	void MP::CloseMainWindow()
	{
		delete[] textTexture;
	}

}
}