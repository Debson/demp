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

		Shader mdWindowShader;
		Shape * mdQuad = NULL;
		glm::mat4 projection;

		GLuint main_background, main_foreground;
		GLuint exit_background, exit_icon, minimize_icon, stay_on_top_icon;
		GLuint volume_bar, volume_speaker, volume_speaker_muted;
		GLuint play_button, stop_button, next_button, previous_button, shuffle_button, repeat_button, dot_icon, playlist_button;
		GLuint music_progress_bar;

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


	int i = 0;
	int pos = 0;
	void MP::RenderTest()
	{
		glm::vec3 color(1.f);
		glm::mat4 model;
		mdWindowShader.use();
		mdWindowShader.setVec3("color", color);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(0, 0, 0.1f));
		//model = glm::scale(model, glm::vec3(mdCurrentWidth, mdCurrentHeight, 1.0f));
		mdWindowShader.setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, main_foreground);
		mdQuad->Draw(mdWindowShader);
		

		if (mdEngine::App::Input::IsScrollForwardActive())
			pos--;
		if (mdEngine::App::Input::IsScrollBackwardActive())
			pos++;

		if (pos < 0)
			pos = 0;

		i = pos;

		while (i < 10 + pos)
		{
				model = glm::mat4();
				model = glm::translate(model, glm::vec3(mdItemContainer[i]->pos.x, mdItemContainer[i]->pos.y - pos * 25.f, 0.5f));
				model = glm::scale(model, glm::vec3(mdItemContainer[i]->size, 1.0f));
				mdWindowShader.setMat4("model", model);
				mdWindowShader.setVec3("color", mdItemContainer[i]->color);
				glBindTexture(GL_TEXTURE_2D, main_foreground);
				mdQuad->Draw(mdWindowShader);
				i++;
		
		}

		glm::vec3 white(1.f);
		mdWindowShader.setVec3("color", white);
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(120, 210, 0.6f));
//		model = glm::scale(model, glm::vec3(mdCurrentWidth - 200.f, mdCurrentHeight - 400.f, 1.0f));
		mdWindowShader.setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, main_foreground);
		mdQuad->Draw(mdWindowShader);


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
			stretchPlaylistMultplier = scaleY;
		}


		/* Main background*/
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(_MAIN_BACKGROUND_POS.x, _MAIN_BACKGROUND_POS.y, 0.f));
		model = glm::scale(model, glm::vec3(_MAIN_BACKGROUND_SIZE.x, _MAIN_BACKGROUND_SIZE.y * stretchMultiplier, 1.f));;
		mdWindowShader.setMat4("model", model);
		mdWindowShader.setBool("cut", true);
		glBindTexture(GL_TEXTURE_2D, main_background);
		mdQuad->Draw(mdWindowShader);
		mdWindowShader.setBool("cut", false);

		// Main foreground
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(_MAIN_FOREGROUND_POS, 0.1f));
		model = glm::scale(model, glm::vec3(_MAIN_FOREGROUND_SIZE, 1.f));
		mdWindowShader.setMat4("model", model);
		mdWindowShader.setBool("cut", true);
		glBindTexture(GL_TEXTURE_2D, main_foreground);
		mdQuad->Draw(mdWindowShader);
		mdWindowShader.setBool("cut", false);

		// Playlist foreground

		if (stretchPlaylistMultplier > 0.05)
		{
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(_PLAYLIST_FOREGROUND_POS, 0.4f));
			model = glm::scale(model, glm::vec3(_PLAYLIST_FOREGROUND_SIZE.x, _PLAYLIST_FOREGROUND_SIZE.y * stretchPlaylistMultplier, 1.f));
			mdWindowShader.setMat4("model", model);
			mdWindowShader.setBool("cut", true);
			glBindTexture(GL_TEXTURE_2D, main_foreground);
			mdQuad->Draw(mdWindowShader);
			mdWindowShader.setBool("cut", false);
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
			mdWindowShader.setMat4("model", model);
			glBindTexture(GL_TEXTURE_2D, playlist_button);
			mdQuad->Draw(mdWindowShader);
		}
		else
		{
			color = grey;
			mdWindowShader.setVec3("color", color);
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(_PLAYLIST_BUTTON_POS, 0.4));
			lastMousePos = deltaVolumePos;
			model = glm::scale(model, glm::vec3(_PLAYLIST_BUTTON_SIZE, 1.f));
			mdWindowShader.setMat4("model", model);
			glBindTexture(GL_TEXTURE_2D, playlist_button);
			mdQuad->Draw(mdWindowShader);
			color = white;
			mdWindowShader.setVec3("color", color);

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
				mdWindowShader.setMat4("model", model);
				glBindTexture(GL_TEXTURE_2D, volume_speaker_muted);
				mdQuad->Draw(mdWindowShader);
			}
			else if (volumeMuted || deltaVolumePos <= 0)
			{
				color = grey;
				mdWindowShader.setVec3("color", color);
				mdEngine::MP::Playlist::MuteVolume(true);
				model = glm::mat4();
				model = glm::translate(model, glm::vec3(_VOLUME_SPEAKER_POS, 0.3f));
				model = glm::scale(model, glm::vec3(_VOLUME_SPEAKER_SIZE, 1.f));
				mdWindowShader.setMat4("model", model);
				glBindTexture(GL_TEXTURE_2D, volume_speaker_muted);
				mdQuad->Draw(mdWindowShader);
				color = white;
				mdWindowShader.setVec3("color", color);
			}
			else if (volumeMuted == false && Input::hasFocus(Input::ButtonType::Volume))
			{
				mdEngine::MP::Playlist::MuteVolume(false);
				model = glm::mat4();
				model = glm::translate(model, glm::vec3(_VOLUME_SPEAKER_POS, 0.3f));
				model = glm::scale(model, glm::vec3(_VOLUME_SPEAKER_SIZE, 1.f));
				mdWindowShader.setMat4("model", model);
				glBindTexture(GL_TEXTURE_2D, volume_speaker);
				mdQuad->Draw(mdWindowShader);
			}
			else if (volumeMuted == false)
			{
				color = grey;
				mdWindowShader.setVec3("color", color);
				mdEngine::MP::Playlist::MuteVolume(false);
				model = glm::mat4();
				model = glm::translate(model, glm::vec3(_VOLUME_SPEAKER_POS, 0.3f));
				model = glm::scale(model, glm::vec3(_VOLUME_SPEAKER_SIZE, 1.f));
				mdWindowShader.setMat4("model", model);
				glBindTexture(GL_TEXTURE_2D, volume_speaker);
				mdQuad->Draw(mdWindowShader);
				color = white;
				mdWindowShader.setVec3("color", color);
			}


			/* Volume bar under volume slider */
			color = dark_grey;
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(_VOLUME_BAR_POS, 0.3f));
			model = glm::scale(model, glm::vec3(_VOLUME_BAR_SIZE, 1.f));
			mdWindowShader.setMat4("model", model);
			mdWindowShader.setVec3("color", color);
			glBindTexture(GL_TEXTURE_2D, volume_bar);
			mdQuad->Draw(mdWindowShader);
			color = white;
			mdWindowShader.setVec3("color", color);


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
				mdWindowShader.setMat4("model", model);
				glBindTexture(GL_TEXTURE_2D, dot_icon);
				mdQuad->Draw(mdWindowShader);

				color = green;
				model = glm::mat4();
				model = glm::translate(model, glm::vec3(_VOLUME_BAR_POS, 0.35f));
				model = glm::scale(model, glm::vec3(deltaVolumePos, _VOLUME_BAR_SIZE.y, 1.f));
				mdWindowShader.setMat4("model", model);
				mdWindowShader.setVec3("color", color);
				glBindTexture(GL_TEXTURE_2D, volume_bar);
				mdQuad->Draw(mdWindowShader);
				color = white;
				mdWindowShader.setVec3("color", color);
			}
			else if (volumeMuted == false)
			{
				color = grey;
				model = glm::mat4();
				model = glm::translate(model, glm::vec3(_VOLUME_BAR_POS, 0.35f));
				model = glm::scale(model, glm::vec3(deltaVolumePos, _VOLUME_BAR_SIZE.y, 1.f));
				mdWindowShader.setMat4("model", model);
				mdWindowShader.setVec3("color", color);
				glBindTexture(GL_TEXTURE_2D, volume_bar);
				mdQuad->Draw(mdWindowShader);
				color = white;
				mdWindowShader.setVec3("color", color);
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
		mdWindowShader.setMat4("model", model);
		mdWindowShader.setVec3("color", color);
		glBindTexture(GL_TEXTURE_2D, music_progress_bar);
		mdQuad->Draw(mdWindowShader);
		color = white;
		mdWindowShader.setVec3("color", color);

		if (Input::hasFocus(Input::ButtonType::SliderMusic) || musicSliderActive)
		{
			color = green;
			mdWindowShader.setVec3("color", color);
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(_MUSIC_PROGRESS_BAR_POS, 0.35f));
			model = glm::scale(model, glm::vec3(deltaMusicPos, _MUSIC_PROGRESS_BAR_SIZE.y, 1.f));
			mdWindowShader.setMat4("model", model);
			glBindTexture(GL_TEXTURE_2D, music_progress_bar);
			mdQuad->Draw(mdWindowShader);
			color = white;
			mdWindowShader.setVec3("color", color);


			model = glm::mat4();
			model = glm::translate(model, glm::vec3(_MUSIC_PROGRESS_BAR_POS.x + deltaMusicPos - _SLIDER_DOT_SIZE.x / 2.f, _MUSIC_PROGRESS_BAR_DOT_POS.y, 0.4));
			lastMousePos = deltaVolumePos;
			model = glm::scale(model, glm::vec3(_SLIDER_DOT_SIZE, 1.f));
			mdWindowShader.setMat4("model", model);
			glBindTexture(GL_TEXTURE_2D, dot_icon);
			mdQuad->Draw(mdWindowShader);


		}
		else
		{
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(_MUSIC_PROGRESS_BAR_POS, 0.35f));
			model = glm::scale(model, glm::vec3(deltaMusicPos, _MUSIC_PROGRESS_BAR_SIZE.y, 1.f));
			mdWindowShader.setMat4("model", model);
			glBindTexture(GL_TEXTURE_2D, music_progress_bar);
			mdQuad->Draw(mdWindowShader);
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
			mdWindowShader.setVec3("color", color);
			mdWindowShader.setMat4("model", dotModel);
			glBindTexture(GL_TEXTURE_2D, dot_icon);
			mdQuad->Draw(mdWindowShader);

		}
		mdWindowShader.setVec3("color", color);
		mdWindowShader.setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, shuffle_button);
		mdQuad->Draw(mdWindowShader);


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
		mdWindowShader.setVec3("color", color);
		mdWindowShader.setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, previous_button);
		mdQuad->Draw(mdWindowShader);


		// Play

		if (Input::isButtonPressed(Input::ButtonType::Play) && Playlist::IsLoaded() == true)
			playActive = !playActive;

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
			mdWindowShader.setVec3("color", color);

			mdWindowShader.setMat4("model", model);
			glBindTexture(GL_TEXTURE_2D, play_button);
			mdQuad->Draw(mdWindowShader);
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
			mdWindowShader.setVec3("color", color);

			mdWindowShader.setMat4("model", model);
			glBindTexture(GL_TEXTURE_2D, stop_button);
			mdQuad->Draw(mdWindowShader);
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
		mdWindowShader.setVec3("color", color);
		mdWindowShader.setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, next_button);
		mdQuad->Draw(mdWindowShader);


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
			mdWindowShader.setVec3("color", color);
			mdWindowShader.setMat4("model", dotModel);
			glBindTexture(GL_TEXTURE_2D, dot_icon);
			mdQuad->Draw(mdWindowShader);
		}
		mdWindowShader.setVec3("color", color);
		mdWindowShader.setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, repeat_button);
		mdQuad->Draw(mdWindowShader);

		color = white;
		mdWindowShader.setVec3("color", color);

		
	}

	void MP::RenderPlaylistItems()
	{
		glm::mat4 model;

		if (playlist.IsToggled())
		{
			if (mdEngine::App::Input::IsScrollForwardActive())
				playlistCurrentPos--;
			if (mdEngine::App::Input::IsScrollBackwardActive())
				playlistCurrentPos++;

			if (playlistCurrentPos < 0)
				playlistCurrentPos = 0;

			playlistIndex = playlistCurrentPos;

			s32 maxItems = (Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y - Data::_PLAYLIST_ITEMS_SURFACE_POS.y ) / PlaylistItem::size.y;


			while (playlistIndex < maxItems + playlistCurrentPos)
			{

				model = glm::mat4();
				model = glm::translate(model, glm::vec3(mdItemContainer[playlistIndex]->pos.x,
					mdItemContainer[playlistIndex]->pos.y - playlistCurrentPos * (mdItemContainer[i]->size.y + mdItemContainer[i]->offsetY), 0.5f));
				model = glm::scale(model, glm::vec3(mdItemContainer[playlistIndex]->size, 1.0f));
				mdWindowShader.setMat4("model", model);
				mdWindowShader.setVec3("color", mdItemContainer[playlistIndex]->color);
				glBindTexture(GL_TEXTURE_2D, main_foreground);
				mdQuad->Draw(mdWindowShader);
				playlistIndex++;

			}

			mdWindowShader.setVec3("color", white);
		}
	}

	void MP::RenderWindowControlButtons()
	{
		glm::mat4 model;
		/* UI Window buttons*/
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(500.f - 70.f, 5.f, 0.2f));
		model = glm::scale(model, glm::vec3(40.f, 15.0f, 1.f));;
		mdWindowShader.setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, exit_background);
		mdQuad->Draw(mdWindowShader);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(Data::_EXIT_BUTTON_POS, 0.3f));
		model = glm::scale(model, glm::vec3(Data::_EXIT_BUTTON_SIZE, 1.f));;
		mdWindowShader.setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, exit_icon);
		mdQuad->Draw(mdWindowShader);
	}

	void MP::StartTest()
	{
		PlaylistItem::startPos = Data::_PLAYLIST_ITEMS_SURFACE_POS;
		for(int i = 0; i < 100; i++)
			new PlaylistItem();
	}

	void MP::StartMainWindow()
	{
		mdQuad			= Shape::QUAD();
		mdWindowShader	= Shader("shaders/window.vert", "shaders/window.frag", nullptr);

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

		test = mdLoadTexture("assets/test.jpg");

		deltaVolumePos = (s32)(mdEngine::MP::Playlist::GetVolume() * 100.f * 0.9f);


		mdWindowShader.use();
		mdWindowShader.setInt("image", 0);
		mdWindowShader.setMat4("projection", projection);

		//mdEngine::UpdateViewport(500, 700);
		StartTest();
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
		//std::cout << mdProjectionHeight << std::endl;
		projection = glm::ortho(0.f, static_cast<float>(Window::windowProperties.mWindowWidth),
									 static_cast<float>(Window::windowProperties.mWindowHeight), 0.f, -1.0f, 1.f);

		mdWindowShader.use();
		mdWindowShader.setMat4("projection", projection);
		mdWindowShader.setVec3("color", color);
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