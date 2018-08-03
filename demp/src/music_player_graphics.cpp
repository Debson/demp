#include "music_player_graphics.h"

#include <gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include "md_load_texture.h"
#include "realtime_system_application.h"
#include "md_shader.h"
#include "md_shape.h"
#include "music_player_ui_input.h"
#include "music_player_playlist.h"
#include "graphics.h"
#include "md_text.h"
#include "music_player.h"

using namespace mdEngine::Graphics;
using namespace mdEngine::MP;
using namespace mdEngine::MP::UI;
using namespace mdEngine::MP::UI::Data;

namespace mdEngine
{
namespace Graphics
{
	namespace MP
	{
		PlaylistObject playlist;

		GLuint main_background = 0, main_foreground = 0;
		GLuint exit_background, exit_icon, minimize_icon, stay_on_top_icon;
		GLuint volume_bar, volume_speaker, volume_speaker_muted;
		GLuint play_button, stop_button, next_button, previous_button, shuffle_button, repeat_button, dot_icon, playlist_button;
		GLuint music_progress_bar;

		b8 texturesLoaded = false;
		GLuint * textTexture;

		b8 shuffleActive(false);
		b8 repeatActive(false);
		b8 playActive(false);
		b8 volumeMuted(false);
		b8 volumeSliderActive(false);
		b8 musicSliderActive(false);
		b8 playlistActive(false);

		b8 playlistToggled(false);


		s32 playlistCurrentPos = 0;
		s32 playlistIndex = 0;
		f32 stretchPlaylistMultplier = 0.f;
		f32 stretchMultiplier = 1.f;
		f32 peekStretchMultiplier = 2.f;

		f32 stretchOffsetY = 0.f;
		s32 lastMousePos = 0;
		s32 deltaVolumePos = 0;
		s32 deltaMusicPos = 0;

		glm::vec3 white(1.f);
		glm::vec3 grey(0.85f);
		glm::vec3 dark_grey(0.5f);
		glm::vec3 green(0.f, 0.7f, 0.f);
		glm::vec3 color(white);

		/* TEST */

		GLuint test;

		void LoadTextTexture();
			
		void RenderPlaylistWindow();

		void RenderVolume();

		void RenderMusicProgressBar();

		void RenderMusicUI();

		void RenderPlaylistItems();

		void RenderWindowControlButtons();

		void StartTest();
	}

	MP::PlaylistObject::PlaylistObject()
	{
		currTime = 0;
		prevTime = 0;
		enabled = false;
		toggled = false;
		unwindTime = 0;
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

	void MP::PlaylistObject::SetRollTime(s16 time)
	{
		timer.targetTime = time;
	}

	f32 MP::PlaylistObject::GetRollProgress()
	{
		return timer.progress();
	}

	void MP::LoadTextTexture()
	{

	}

	void MP::RenderPlaylistWindow()
	{
		glm::mat4 model;

		f32 scaleY = (float)Window::windowProperties.mApplicationHeight / _DEFAULT_WINDOW_SIZE.y;

		if (playlist.IsRolling() && playlist.IsEnabled() == true)
		{
			stretchMultiplier = (1.f + playlist.GetRollProgress()) * scaleY;
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
			stretchPlaylistMultplier = 2.f * scaleY - 1.f;;
		}


		/* Main background*/
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(_MAIN_BACKGROUND_POS.x, _MAIN_BACKGROUND_POS.y, 0.f));
		model = glm::scale(model, glm::vec3(_MAIN_BACKGROUND_SIZE.x, _MAIN_BACKGROUND_SIZE.y * stretchMultiplier, 1.f));;
		Shader::shaderDefault->setMat4("model", model);
		Shader::shaderDefault->setBool("cut", true);
		glBindTexture(GL_TEXTURE_2D, main_background);
		Shader::Draw();
		Shader::shaderDefault->setBool("cut", false);

		// Main foreground
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(_MAIN_FOREGROUND_POS, 0.1f));
		model = glm::scale(model, glm::vec3(_MAIN_FOREGROUND_SIZE, 1.f));
		Shader::shaderDefault->setMat4("model", model);
		Shader::shaderDefault->setBool("cut", true);
		glBindTexture(GL_TEXTURE_2D, main_foreground);
		Shader::Draw();;
		Shader::shaderDefault->setBool("cut", false);

		// Playlist foreground

		if (stretchPlaylistMultplier > 0.05)
		{
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(_PLAYLIST_FOREGROUND_POS, 0.4f));
			model = glm::scale(model, glm::vec3(_PLAYLIST_FOREGROUND_SIZE.x, _PLAYLIST_FOREGROUND_SIZE.y * stretchPlaylistMultplier, 1.f));
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
			model = glm::translate(model, glm::vec3(_PLAYLIST_BUTTON_POS, 0.4));
			lastMousePos = deltaVolumePos;
			model = glm::scale(model, glm::vec3(_PLAYLIST_BUTTON_SIZE, 1.f));
			Shader::shaderDefault->setMat4("model", model);
			glBindTexture(GL_TEXTURE_2D, playlist_button);
			Shader::Draw();
		}
		else
		{
			color = grey;
			Shader::shaderDefault->setVec3("color", color);
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(_PLAYLIST_BUTTON_POS, 0.4));
			lastMousePos = deltaVolumePos;
			model = glm::scale(model, glm::vec3(_PLAYLIST_BUTTON_SIZE, 1.f));
			Shader::shaderDefault->setMat4("model", model);
			glBindTexture(GL_TEXTURE_2D, playlist_button);
			Shader::Draw();
			color = white;
			Shader::shaderDefault->setVec3("color", color);

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
			deltaVolumePos = (s32)(mdEngine::MP::Playlist::GetVolume() * (float)_VOLUME_BAR_SIZE.x);

			if (Input::isButtonPressed(Input::ButtonType::Volume))
				volumeMuted = !volumeMuted;

			if (Input::hasFocus(Input::ButtonType::SliderVolume) && App::Input::IsKeyDown(App::KeyCode::MouseLeft) && 
								musicSliderActive == false && Window::windowProperties.mWindowEvent != App::WindowEvent::kResize)
				volumeSliderActive = true;
			else if (App::Input::IsKeyDown(App::KeyCode::MouseLeft) == false)
				volumeSliderActive = false;


			if (volumeSliderActive == true)
			{
				s32 mouseX, mouseY;
				App::Input::GetMousePosition(&mouseX, &mouseY);
				deltaVolumePos = mouseX - _VOLUME_BAR_POS.x;
				if (deltaVolumePos > _VOLUME_BAR_SIZE.x)
					deltaVolumePos = _VOLUME_BAR_SIZE.x;
				if (deltaVolumePos < 0)
					deltaVolumePos = 0;

				mdEngine::MP::Playlist::SetVolume(deltaVolumePos / (float)_VOLUME_BAR_SIZE.x);
			}


			/* xD */
			if (volumeMuted == true || deltaVolumePos <= 0 && Input::hasFocus(Input::ButtonType::Volume))
			{
				mdEngine::MP::Playlist::MuteVolume(true);
				model = glm::mat4();
				model = glm::translate(model, glm::vec3(_VOLUME_SPEAKER_POS, 0.3f));
				model = glm::scale(model, glm::vec3(_VOLUME_SPEAKER_SIZE, 1.f));
				Shader::shaderDefault->setMat4("model", model);
				glBindTexture(GL_TEXTURE_2D, volume_speaker_muted);
				Shader::Draw();
			}
			else if (volumeMuted || deltaVolumePos <= 0)
			{
				color = grey;
				Shader::shaderDefault->setVec3("color", color);
				mdEngine::MP::Playlist::MuteVolume(true);
				model = glm::mat4();
				model = glm::translate(model, glm::vec3(_VOLUME_SPEAKER_POS, 0.3f));
				model = glm::scale(model, glm::vec3(_VOLUME_SPEAKER_SIZE, 1.f));
				Shader::shaderDefault->setMat4("model", model);
				glBindTexture(GL_TEXTURE_2D, volume_speaker_muted);
				Shader::Draw();
				color = white;
				Shader::shaderDefault->setVec3("color", color);
			}
			else if (volumeMuted == false && Input::hasFocus(Input::ButtonType::Volume))
			{
				mdEngine::MP::Playlist::MuteVolume(false);
				model = glm::mat4();
				model = glm::translate(model, glm::vec3(_VOLUME_SPEAKER_POS, 0.3f));
				model = glm::scale(model, glm::vec3(_VOLUME_SPEAKER_SIZE, 1.f));
				Shader::shaderDefault->setMat4("model", model);
				glBindTexture(GL_TEXTURE_2D, volume_speaker);
				Shader::Draw();
			}
			else if (volumeMuted == false)
			{
				color = grey;
				Shader::shaderDefault->setVec3("color", color);
				mdEngine::MP::Playlist::MuteVolume(false);
				model = glm::mat4();
				model = glm::translate(model, glm::vec3(_VOLUME_SPEAKER_POS, 0.3f));
				model = glm::scale(model, glm::vec3(_VOLUME_SPEAKER_SIZE, 1.f));
				Shader::shaderDefault->setMat4("model", model);
				glBindTexture(GL_TEXTURE_2D, volume_speaker);
				Shader::Draw();
				color = white;
				Shader::shaderDefault->setVec3("color", color);
			}


			/* Volume bar under volume slider */
			color = dark_grey;
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(_VOLUME_BAR_POS, 0.3f));
			model = glm::scale(model, glm::vec3(_VOLUME_BAR_SIZE, 1.f));
			Shader::shaderDefault->setMat4("model", model);
			Shader::shaderDefault->setVec3("color", color);
			glBindTexture(GL_TEXTURE_2D, volume_bar);
			Shader::Draw();
			color = white;
			Shader::shaderDefault->setVec3("color", color);


			if (Input::hasFocus(Input::ButtonType::SliderVolume) || volumeSliderActive == true)
			{
				if (volumeMuted == true)
				{
					model = glm::mat4();
					model = glm::translate(model, glm::vec3(_VOLUME_BAR_POS.x - _SLIDER_DOT_SIZE.x / 2.f, _VOLUME_BAR_DOT_POS.y, 0.4));
					if (lastMousePos != deltaVolumePos)
						volumeMuted = false;
				}
				else
				{
					model = glm::mat4();
					model = glm::translate(model, glm::vec3(_VOLUME_BAR_POS.x + deltaVolumePos - _SLIDER_DOT_SIZE.x / 2.f, _VOLUME_BAR_DOT_POS.y, 0.4));
					lastMousePos = deltaVolumePos;
				}

				model = glm::scale(model, glm::vec3(_SLIDER_DOT_SIZE, 1.f));
				Shader::shaderDefault->setMat4("model", model);
				glBindTexture(GL_TEXTURE_2D, dot_icon);
				Shader::Draw();

				color = green;
				model = glm::mat4();
				model = glm::translate(model, glm::vec3(_VOLUME_BAR_POS, 0.35f));
				model = glm::scale(model, glm::vec3(deltaVolumePos, _VOLUME_BAR_SIZE.y, 1.f));
				Shader::shaderDefault->setMat4("model", model);
				Shader::shaderDefault->setVec3("color", color);
				glBindTexture(GL_TEXTURE_2D, volume_bar);
				Shader::Draw();
				color = white;
				Shader::shaderDefault->setVec3("color", color);
			}
			else if (volumeMuted == false)
			{
				color = grey;
				model = glm::mat4();
				model = glm::translate(model, glm::vec3(_VOLUME_BAR_POS, 0.35f));
				model = glm::scale(model, glm::vec3(deltaVolumePos, _VOLUME_BAR_SIZE.y, 1.f));
				Shader::shaderDefault->setMat4("model", model);
				Shader::shaderDefault->setVec3("color", color);
				glBindTexture(GL_TEXTURE_2D, volume_bar);
				Shader::Draw();
				color = white;
				Shader::shaderDefault->setVec3("color", color);
			}
		}
	}

	void MP::RenderMusicProgressBar()
	{
		glm::mat4 model;
		f32 musicPos = Playlist::GetPosition();
		f32 musicLength = Playlist::GetMusicLength();

		if (musicPos < 0 || musicLength < 0)
			deltaMusicPos = 0;
		else
			deltaMusicPos = musicPos / musicLength * _MUSIC_PROGRESS_BAR_SIZE.x;

		if (musicSliderActive == true)
		{
			s32 mouseX, mouseY;
			App::Input::GetMousePosition(&mouseX, &mouseY);
			deltaMusicPos = mouseX - _MUSIC_PROGRESS_BAR_POS.x;
			if (deltaMusicPos > _MUSIC_PROGRESS_BAR_SIZE.x)
				deltaMusicPos = _MUSIC_PROGRESS_BAR_SIZE.x;
			if (deltaMusicPos < 0)
				deltaMusicPos = 0;

			if (App::Input::IsKeyDown(App::KeyCode::MouseLeft) == false)
				mdEngine::MP::Playlist::SetPosition(deltaMusicPos / (float)_MUSIC_PROGRESS_BAR_SIZE.x * Playlist::GetMusicLength());
		}

		if (Input::hasFocus(Input::ButtonType::SliderMusic) && App::Input::IsKeyDown(App::KeyCode::MouseLeft) && 
							volumeSliderActive == false && Window::windowProperties.mWindowEvent != App::WindowEvent::kResize)
			musicSliderActive = true;
		else if (App::Input::IsKeyDown(App::KeyCode::MouseLeft) == false)
			musicSliderActive = false;

		color = dark_grey;
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(_MUSIC_PROGRESS_BAR_POS, 0.3f));
		model = glm::scale(model, glm::vec3(_MUSIC_PROGRESS_BAR_SIZE, 1.f));
		Shader::shaderDefault->setMat4("model", model);
		Shader::shaderDefault->setVec3("color", color);
		glBindTexture(GL_TEXTURE_2D, music_progress_bar);
		Shader::Draw();
		color = white;
		Shader::shaderDefault->setVec3("color", color);

		if (Input::hasFocus(Input::ButtonType::SliderMusic) || musicSliderActive)
		{
			color = green;
			Shader::shaderDefault->setVec3("color", color);
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(_MUSIC_PROGRESS_BAR_POS, 0.35f));
			model = glm::scale(model, glm::vec3(deltaMusicPos, _MUSIC_PROGRESS_BAR_SIZE.y, 1.f));
			Shader::shaderDefault->setMat4("model", model);
			glBindTexture(GL_TEXTURE_2D, music_progress_bar);
			Shader::Draw();
			color = white;
			Shader::shaderDefault->setVec3("color", color);


			model = glm::mat4();
			model = glm::translate(model, glm::vec3(_MUSIC_PROGRESS_BAR_POS.x + deltaMusicPos - _SLIDER_DOT_SIZE.x / 2.f, _MUSIC_PROGRESS_BAR_DOT_POS.y, 0.4));
			lastMousePos = deltaVolumePos;
			model = glm::scale(model, glm::vec3(_SLIDER_DOT_SIZE, 1.f));
			Shader::shaderDefault->setMat4("model", model);
			glBindTexture(GL_TEXTURE_2D, dot_icon);
			Shader::Draw();


		}
		else
		{
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(_MUSIC_PROGRESS_BAR_POS, 0.35f));
			model = glm::scale(model, glm::vec3(deltaMusicPos, _MUSIC_PROGRESS_BAR_SIZE.y, 1.f));
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

		if (Input::hasFocus(Input::ButtonType::Shuffle) && shuffleActive == false)
		{
			focusMultiplier = 1.02f;
			color = white;
			model = glm::translate(model, glm::vec3(_SHUFFLE_BUTTON_POS.x + (_SHUFFLE_BUTTON_SIZE.x / 2.f) * (1.f - focusMultiplier),
				_SHUFFLE_BUTTON_POS.y + (_SHUFFLE_BUTTON_SIZE.y / 2.f) * (1.f - focusMultiplier), 0.3f));
			model = glm::scale(model, glm::vec3(_SHUFFLE_BUTTON_SIZE * focusMultiplier, 1.f));

		}
		else if (shuffleActive == false)
		{
			color = grey;
			model = glm::translate(model, glm::vec3(_SHUFFLE_BUTTON_POS, 0.3f));
			model = glm::scale(model, glm::vec3(_SHUFFLE_BUTTON_SIZE, 1.f));;
		}
		else
		{
			color = green;
			model = glm::translate(model, glm::vec3(_SHUFFLE_BUTTON_POS, 0.3f));
			model = glm::scale(model, glm::vec3(_SHUFFLE_BUTTON_SIZE, 1.f));;

			dotModel = glm::translate(dotModel, glm::vec3(_SHUFFLE_BUTTON_POS.x + _SHUFFLE_BUTTON_SIZE.x / 2.f - _DOT_BUTTON_STATE_SIZE.x / 2.f - dotOffsetXShuffle, _SHUFFLE_BUTTON_POS.y + 18.f, 0.3f));
			dotModel = glm::scale(dotModel, glm::vec3(_DOT_BUTTON_STATE_SIZE, 1.f));
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
			color = white;
			model = glm::translate(model, glm::vec3(_PREVIOUS_BUTTON_POS.x + (_PREVIOUS_BUTTON_SIZE.x / 2.f) * (1.f - focusMultiplier),
				_PREVIOUS_BUTTON_POS.y + (_PREVIOUS_BUTTON_SIZE.y / 2.f) * (1.f - focusMultiplier), 0.3f));
			model = glm::scale(model, glm::vec3(_PREVIOUS_BUTTON_SIZE * focusMultiplier, 1.f));

		}
		else
		{
			color = grey;
			model = glm::translate(model, glm::vec3(_PREVIOUS_BUTTON_POS, 0.3f));
			model = glm::scale(model, glm::vec3(_PREVIOUS_BUTTON_SIZE, 1.f));;
		}
		Shader::shaderDefault->setVec3("color", color);
		Shader::shaderDefault->setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, previous_button);
		Shader::Draw();


		// Play
		if (Input::isButtonPressed(Input::ButtonType::Play) && Playlist::IsLoaded() == true)
			playActive = !playActive;

		if (Input::isButtonPressed(Input::ButtonType::Previous) || Input::isButtonPressed(Input::ButtonType::Next) &&
			playActive == false)
			playActive = true;

		model = glm::mat4();
		if (Input::hasFocus(Input::ButtonType::Play) && playActive == false)
		{
			focusMultiplier = 1.07f;
			color = white;
			model = glm::translate(model, glm::vec3(_PLAY_BUTTON_POS.x + (_PLAY_BUTTON_SIZE.x / 2.f) * (1.f - focusMultiplier),
				_PLAY_BUTTON_POS.y + (_PLAY_BUTTON_SIZE.y / 2.f) * (1.f - focusMultiplier), 0.3f));
			model = glm::scale(model, glm::vec3(_PLAY_BUTTON_SIZE * focusMultiplier, 1.f));

		}
		else if (!Input::hasFocus(Input::ButtonType::Play) && playActive == false)
		{
			color = grey;
			model = glm::translate(model, glm::vec3(_PLAY_BUTTON_POS, 0.3f));
			model = glm::scale(model, glm::vec3(_PLAY_BUTTON_SIZE, 1.f));;
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
			color = white;
			model = glm::translate(model, glm::vec3(_PLAY_BUTTON_POS.x + (_PLAY_BUTTON_SIZE.x / 2.f) * (1.f - focusMultiplier),
				_PLAY_BUTTON_POS.y + (_PLAY_BUTTON_SIZE.y / 2.f) * (1.f - focusMultiplier), 0.3f));
			model = glm::scale(model, glm::vec3(_PLAY_BUTTON_SIZE * focusMultiplier, 1.f));

		}
		else if (!Input::hasFocus(Input::ButtonType::Stop) && playActive == true)
		{
			color = grey;
			model = glm::translate(model, glm::vec3(_PLAY_BUTTON_POS, 0.3f));
			model = glm::scale(model, glm::vec3(_PLAY_BUTTON_SIZE, 1.f));;
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
			color = white;
			model = glm::translate(model, glm::vec3(_NEXT_BUTTON_POS.x + (_NEXT_BUTTON_SIZE.x / 2.f) * (1.f - focusMultiplier),
				_NEXT_BUTTON_POS.y + (_NEXT_BUTTON_SIZE.y / 2.f) * (1.f - focusMultiplier), 0.3f));
			model = glm::scale(model, glm::vec3(_NEXT_BUTTON_SIZE * focusMultiplier, 1.f));

		}
		else
		{
			color = grey;
			model = glm::translate(model, glm::vec3(_NEXT_BUTTON_POS, 0.3f));
			model = glm::scale(model, glm::vec3(_NEXT_BUTTON_SIZE, 1.f));;
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
			color = white;
			model = glm::translate(model, glm::vec3(_REPEAT_BUTTON_POS.x + (_REPEAT_BUTTON_SIZE.x / 2.f) * (1.f - focusMultiplier),
				_REPEAT_BUTTON_POS.y + (_REPEAT_BUTTON_SIZE.y / 2.f) * (1.f - focusMultiplier), 0.3f));
			model = glm::scale(model, glm::vec3(_REPEAT_BUTTON_SIZE * focusMultiplier, 1.f));

		}
		else if (repeatActive == false)
		{
			color = grey;
			model = glm::translate(model, glm::vec3(_REPEAT_BUTTON_POS, 0.3f));
			model = glm::scale(model, glm::vec3(_REPEAT_BUTTON_SIZE, 1.f));;
		}
		else
		{
			color = green;
			model = glm::translate(model, glm::vec3(_REPEAT_BUTTON_POS, 0.3f));
			model = glm::scale(model, glm::vec3(_REPEAT_BUTTON_SIZE, 1.f));

			dotModel = glm::mat4();
			dotModel = glm::translate(dotModel, glm::vec3(_REPEAT_BUTTON_POS.x + _REPEAT_BUTTON_SIZE.x / 2.f - _DOT_BUTTON_STATE_SIZE.x / 2.f - dotOffsetXRepeat, _REPEAT_BUTTON_POS.y + 18.f, 0.3f));
			dotModel = glm::scale(dotModel, glm::vec3(_DOT_BUTTON_STATE_SIZE, 1.f));
			Shader::shaderDefault->setVec3("color", color);
			Shader::shaderDefault->setMat4("model", dotModel);
			glBindTexture(GL_TEXTURE_2D, dot_icon);
			Shader::Draw();
		}
		Shader::shaderDefault->setVec3("color", color);
		Shader::shaderDefault->setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, repeat_button);
		Shader::Draw();

		color = white;
		Shader::shaderDefault->setVec3("color", color);

		
	}

	void MP::RenderPlaylistItems()
	{
		glm::mat4 model;

		if (playlist.IsToggled() && mdItemContainer.size() > 0)
		{


			if (mdEngine::App::Input::IsScrollForwardActive())
			{
				playlistCurrentPos--;
				texturesLoaded = false;
			}
			if (mdEngine::App::Input::IsScrollBackwardActive())
			{
				playlistCurrentPos++;
				texturesLoaded = false;
			}




			s32 maxItems = (Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y - Data::_PLAYLIST_ITEMS_SURFACE_POS.y) / mdItemContainer[0]->mSize.y;

			s32 currentSongID = mdEngine::MP::Playlist::RamLoadedMusic.mID;


			
			if ((currentSongID > maxItems + playlistCurrentPos - 2) && currentSongID < mdItemContainer.size() - 1)
			{
				/* Basically If current song id is greater than playlist current pos, jump to penultimate
					pos in playlist. Also had to consider case, when playlist size is small */
				playlistCurrentPos = currentSongID - maxItems + 2;
				mdEngine::MP::musicPlayerState = mdEngine::MP::MusicPlayerState::kChanged;
			}
			else if(playlistCurrentPos != 0 && playlistCurrentPos >= currentSongID)
			{
				/* If current song ID is less than current playlist pos, jump to second pos in playlist */
				playlistCurrentPos = currentSongID - 1;
				mdEngine::MP::musicPlayerState = mdEngine::MP::MusicPlayerState::kChanged;
			}
			else if (currentSongID >= mdItemContainer.size() - 1 && 
					 currentSongID - maxItems >= playlistCurrentPos)
			{
				/* When current song position is the last in playlist */
				playlistCurrentPos = mdItemContainer.size() - maxItems;;
				mdEngine::MP::musicPlayerState = mdEngine::MP::MusicPlayerState::kChanged;
			}

			/* Check out of bounds */
			if (playlistCurrentPos < 0)
				playlistCurrentPos = 0;

			if (playlistCurrentPos > mdItemContainer.size() - 1)
				playlistCurrentPos = mdItemContainer.size() - 2;


			playlistIndex = playlistCurrentPos;
			

			/* Check if there is less items in container than current playlist size can display.
			   If yes, make sure that only items in container will be displayed. */
			if (mdItemContainer.size() < maxItems)
				maxItems = mdItemContainer.size();

			/* Don't go over mdItemContainer size*/
			if (maxItems + playlistCurrentPos > mdItemContainer.size())
			{
				playlistCurrentPos--;
			}

			if (mdEngine::MP::musicPlayerState != mdEngine::MP::MusicPlayerState::kStatic)
			{
				texturesLoaded = false;
				mdEngine::MP::musicPlayerState = mdEngine::MP::MusicPlayerState::kStatic;
			}



			if (texturesLoaded == false)
			{
				/* Load all textures that are in in display range */
				if(textTexture != NULL)
					glDeleteTextures(maxItems, textTexture);

				textTexture = new GLuint[maxItems];

				for (u16 i = playlistIndex, t = 0; i < maxItems + playlistCurrentPos; i++, t++)
				{
					textTexture[t] = Text::LoadText(mdItemContainer[i]->mFont,
													mdItemContainer[i]->GetTitle(),
													mdItemContainer[i]->mTextColor);
				}
				texturesLoaded = true;
			}




			u16 texIndex = 0;
			while (playlistIndex < maxItems  + playlistCurrentPos)
			{
				/* Playlist items are rendered on the same positions as positions of the first(0 - maxItems), 
				   they are only transformed by matrix, so when you want to check if specific item has focus, 
				   it will check the focus on the wrong item(0 - maxItems). To prevent that, always add mOffsetIndex,
				   do checked positions, so you will get index of actuall rendered item. */
				PlaylistItem::mOffsetIndex = playlistCurrentPos;
				PlaylistItem * item = mdItemContainer[playlistIndex];

				glm::vec3 itemColor;
				if (item->IsPlaying() == true)
					itemColor = glm::vec3(1.f, 0.f, 0.f);
				else
					itemColor = item->mColor;

				model = glm::mat4();
				model = glm::translate(model, glm::vec3(item->mPos.x, item->mPos.y - playlistCurrentPos * (item->mSize.y + item->mOffsetY), 0.5f));
				model = glm::scale(model, glm::vec3(item->mSize, 1.0f));
				Shader::shaderDefault->setMat4("model", model);
				Shader::shaderDefault->setVec3("color", itemColor);
				glBindTexture(GL_TEXTURE_2D, main_foreground);
				Shader::Draw();

				glm::vec3 color(1.f);
				model = glm::mat4();
				model = glm::translate(model, glm::vec3(item->mPos.x + 5.f, item->mPos.y - playlistCurrentPos * (item->mSize.y + item->mOffsetY) + item->mSize.y / 4.f, 0.8f));
				model = glm::scale(model, glm::vec3((float)item->mTextSize.x * item->mTextScale,
													(float)item->mTextSize.y * item->mTextScale, 1.0f));
				Shader::shaderDefault->setMat4("model", model);
				Shader::shaderDefault->setVec3("color", color);
				glBindTexture(GL_TEXTURE_2D, textTexture[texIndex]);
				Shader::Draw();


				texIndex++;
				playlistIndex++;

			}

			Shader::shaderDefault->setVec3("color", white);
		}
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

	void MP::StartTest()
	{
		
	}

	void MP::StartMainWindow()
	{


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

		Shader::shaderDefault->setVec3("color", white);
		glActiveTexture(GL_TEXTURE0);

		RenderPlaylistWindow();

		RenderVolume();

		RenderMusicProgressBar();

		RenderMusicUI();

		RenderPlaylistItems();

		RenderWindowControlButtons();

	}

	void MP::CloseMainWindow()
	{

	}

	
}
}