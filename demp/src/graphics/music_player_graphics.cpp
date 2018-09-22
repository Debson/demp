#include "music_player_graphics.h"

#include <iostream>
#include <algorithm>

#include <gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "../app/realtime_system_application.h"
#include "graphics.h"
#include "music_player_graphics_playlist.h"
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
		Interface::Button* m_PlaylistBarSlider;
		Interface::Button* m_AddFileButtonRef;
		Interface::TextBox m_AddFileTextBox;

		GLuint main_background = 0, main_foreground = 0;
		GLuint ui_buttons_background, ui_buttons_background_left,
			   exit_icon, minimize_icon, stay_on_top_icon, settings_icon, 
			   exit_background, minimize_background, stay_on_top_background, settings_background,
			   exit_background_glow, minimize_background_glow, stay_on_top_background_glow, settings_background_glow;

		GLuint volume_bar, volume_speaker, volume_speaker_muted, volume_speaker_low, volume_speaker_medium;
		GLuint play_button, stop_button, next_button, previous_button, shuffle_button, repeat_button, dot_icon, playlist_button,
			   playlist_add_file;
		GLuint music_progress_bar;



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

		b8 playlistOpened(false);

		b8 playlistWasEmpty(false);

		b8 scrollBarAtBottom(false);

		b8 windowResizeStarted(false);

		s32 visibleSeparatorsCount;

		//
		b8 musicInfoScrollTextLoaded(false);
		b8 musicInfoScrollTextRewind(false);
		b8 musicInfoScrollTextIsMoved(false);
		s8 musicInfoScrollTextDirection = 1;
		b8 musicInfoScrollPingPong(false);


		f32 playlistPositionOffset;
		f32 playlistPositionOffsetPrevious;
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

		Text::TextObject	musicInfoScrollText[2];
		Interface::Button*	musicInfoScrollTextButton[2];

		Time::Timer musicInfoScrollTextTimer;

		
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

		void InitializeText();

		void InitializeTextBoxes();

		void RenderPlaylistWindow();

		void RenderVolume();

		void RenderMusicProgressBar();

		void RenderMusicUI();

		void RenderScrollBar(f32* playlistOffset, f32 displayedItems, f32 maxItems);

		void RenderPlaylistItems();

		void RenderPlaylistAddButtons();

		void RenderSettingsButtons();

		void RenderWindowControlButtons();

		void RenderPlaylistInfo();

		void RenderMusicScrollInfo();

	}



	void MP::InitializeText()
	{
		durationText			= Text::TextObject(Data::_MUSIC_PLAYER_NUMBER_FONT, Color::Grey);
		itemsSizeText			= Text::TextObject(Data::_MUSIC_PLAYER_NUMBER_FONT, Color::Grey);
		itemsCountText			= Text::TextObject(Data::_MUSIC_PLAYER_NUMBER_FONT, Color::Grey);
		loadedItemsCountText	= Text::TextObject(Data::_MUSIC_PLAYER_NUMBER_FONT, Color::Grey);
		musicInfoScrollText[0]	= Text::TextObject(Data::_MUSIC_PLAYER_NUMBER_FONT, Color::Grey);
		musicInfoScrollText[1]	= Text::TextObject(Data::_MUSIC_PLAYER_NUMBER_FONT, Color::Grey);

		durationText.SetTextPos(Data::_TEXT_ITEMS_DURATION_POS);
		itemsSizeText.SetTextPos(Data::_TEXT_ITEMS_SIZE_POS);
		itemsCountText.SetTextPos(Data::_TEXT_ITEMS_COUNT_POS);
		loadedItemsCountText.SetTextPos(Data::_TEXT_LOADED_ITEMS_COUNT_POS);
		musicInfoScrollText[0].SetTextPos(Data::_TEXT_MUSIC_TITLE_SCROLL_POS);
		musicInfoScrollText[1].SetTextPos(Data::_TEXT_MUSIC_TITLE_SCROLL_POS);;
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
		Parser::GetInt(file, Strings::_PLAYLIST_STATE) == 1 ? (GetPlaylistObject()->Toggle(), GetPlaylistObject()->Enable()) : GetPlaylistObject()->UnToggle();

		file = Strings::_PLAYLIST_FILE;
		GetPlaylistObject()->SetItemsDuration(Parser::GetFloat(file, Strings::_CONTENT_DURATION));
		GetPlaylistObject()->SetItemsSize(Parser::GetFloat(file, Strings::_CONTENT_SIZE));


		State::SetState(State::AudioAdded);
	}

	void MP::RenderPlaylistWindow()
	{
		static s32 toggledWindowHeight = Window::windowProperties.mApplicationHeight;

		GetPlaylistObject()->SetPos(glm::vec2(0.f, Data::_PLAYLIST_FOREGROUND_POS.y));
		GetPlaylistObject()->SetSize(glm::vec2(Window::windowProperties.mWindowWidth, Window::windowProperties.mApplicationHeight));
		GetMainPlayerObject()->SetPos(glm::vec2(0.f, 0.f));
		GetMainPlayerObject()->SetSize(Data::_DEFAULT_PLAYER_SIZE);

		//md_log(Data::_MAIN_BACKGROUND_SIZE.y);

		// If playlist button pressed, roll down/up playlist
		if (Input::isButtonPressed(Input::ButtonType::Playlist) && State::CheckState(State::PlaylistRolling) == false)
		{
			interp = 0.f;

			GetPlaylistObject()->Enable();
		}
		// Playlist rolling down
		if (GetPlaylistObject()->IsEnabled() && !GetPlaylistObject()->IsToggled())
		{
			State::SetState(State::PlaylistRolling);
			if (interp > 1.f)
			{
				interp = 1.f;
				GetPlaylistObject()->Toggle();
			}
			Data::_MAIN_BACKGROUND_SIZE.y = Math::Lerp(Data::_DEFAULT_PLAYER_SIZE.y, Window::windowProperties.mApplicationHeight, interp);
			interp += (Data::PlaylistRollMultiplier * 10.f / toggledWindowHeight) * Time::deltaTime;
		}

		// Playlist rolling up
		if (!GetPlaylistObject()->IsEnabled() && GetPlaylistObject()->IsToggled())
		{
			State::SetState(State::PlaylistRolling);
			if (interp > 1.f)
			{
				interp = 1.f;
				GetPlaylistObject()->UnToggle();
			}
			Data::_MAIN_BACKGROUND_SIZE.y = Math::Lerp(Window::windowProperties.mApplicationHeight, Data::_DEFAULT_PLAYER_SIZE.y, interp);
			interp += (Data::PlaylistRollMultiplier * 10.f / toggledWindowHeight) * Time::deltaTime;
		}

		// Playlist is enabled
		if (GetPlaylistObject()->IsToggled() && GetPlaylistObject()->IsEnabled())
		{
			State::ResetState(State::PlaylistRolling);
			Data::UpdateData();
			toggledWindowHeight = Window::windowProperties.mApplicationHeight;
		}

		if (GetPlaylistObject()->IsToggled() == false && GetPlaylistObject()->IsEnabled() == false)
		{
			State::ResetState(State::PlaylistRolling);
		}



		/* Main background*/
		Shader::shaderDefault->setBool("roundEdgesBackground", true);
		Shader::shaderDefault->setFloat("playerHeightChange", (((float)Data::_MIN_PLAYER_SIZE.y + 20.f) /
												((float)mdEngine::Window::windowProperties.mApplicationHeight)));
		//md_log(((float)Data::_MIN_PLAYER_SIZE.y + 20.f) / ((float)mdEngine::Window::windowProperties.mApplicationHeight));
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(Data::_MAIN_BACKGROUND_POS.x, Data::_MAIN_BACKGROUND_POS.y, 0.f));
		model = glm::scale(model, glm::vec3(Data::_MAIN_BACKGROUND_SIZE.x, Data::_MAIN_BACKGROUND_SIZE.y, 1.f));;
		Shader::shaderDefault->setMat4("model", model);
		Shader::shaderDefault->setBool("cut", true);
		glBindTexture(GL_TEXTURE_2D, main_background);
		Shader::Draw(Shader::shaderDefault);
		Shader::shaderDefault->setBool("cut", false);
		Shader::shaderDefault->setBool("roundEdgesBackground", false);

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
		if (GetPlaylistObject()->IsToggled() && GetPlaylistObject()->IsEnabled())
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
		if (GetPlaylistObject()->IsToggled() &&
			GetPlaylistObject()->IsEnabled())
		{
			RenderPlaylistAddButtons();
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
			deltaVolumePos = (s32)(Data::VolumeLevel * (float)Data::_VOLUME_BAR_SIZE.x);

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
				State::SetState(State::VolumeChanged);
				s32 mouseX, mouseY;
				App::Input::GetMousePosition(&mouseX, &mouseY);
				deltaVolumePos = mouseX - Data::_VOLUME_BAR_POS.x;
				if (deltaVolumePos > Data::_VOLUME_BAR_SIZE.x)
					deltaVolumePos = Data::_VOLUME_BAR_SIZE.x;
				if (deltaVolumePos < 0)
					deltaVolumePos = 0;

				mdEngine::MP::Playlist::SetVolume(deltaVolumePos / (float)Data::_VOLUME_BAR_SIZE.x);
			}

			GLuint volumeSpeakerTex = 0;
			if (Data::VolumeLevel > 0.65)
			{
				volumeSpeakerTex = volume_speaker;
			}
			else if (Data::VolumeLevel > 0.3)
			{
				volumeSpeakerTex = volume_speaker_medium;
			}
			else
			{
				volumeSpeakerTex = volume_speaker_low;
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
				glBindTexture(GL_TEXTURE_2D, volumeSpeakerTex);
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
				glBindTexture(GL_TEXTURE_2D, volumeSpeakerTex);
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

		//if (Input::isButtonPressed(Input::ButtonType::Shuffle))
		
		shuffleActive = Playlist::IsShuffleEnabled();

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
		//md_log(Playlist::IsChannelPlaying());
		playActive = Playlist::IsChannelPlaying();
		//if (Input::isButtonPressed(Input::ButtonType::Play) && mdEngine::MP::Playlist::IsLoaded() == true)

		if (Input::isButtonPressed(Input::ButtonType::Previous) || Input::isButtonPressed(Input::ButtonType::Next) &&
			playActive == false)
		{
			playActive = true;
		}

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
		if (GetPlaylistObject()->IsToggled() &&
			GetPlaylistObject()->IsEnabled())
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

	void MP::RenderMusicScrollInfo()
	{
		if (GetPlaylistObject()->GetPlayingID() >= 0 &&
			State::CheckState(State::AudioChanged) ||
			State::CheckState(State::AudioChosen))
		{
			musicInfoScrollText[0].SetTextString(GetPlaylistObject()->GetMusicInfoScrollString());
			musicInfoScrollText[0].SetTextScale(1.f);
			musicInfoScrollText[0].ReloadTextTexture();
			s32 offsetX = (Data::_MAIN_FOREGROUND_SIZE.x - musicInfoScrollText[0].GetTextSize().x) / 2;
			musicInfoScrollText[0].SetTextPos(glm::vec2(Data::_MAIN_FOREGROUND_POS.x - musicInfoScrollText[0].GetTextSize().x,
													    musicInfoScrollText[0].GetTextPos().y));
			musicInfoScrollText[0].SetTextColor(Color::Orange);

			musicInfoScrollText[1] = musicInfoScrollText[0];
			musicInfoScrollText[1].SetTextPos(glm::vec2(Data::_DEFAULT_PLAYER_SIZE.x, musicInfoScrollText[0].GetTextPos().y));

			musicInfoScrollTextButton[0]->SetButtonSize(musicInfoScrollText[0].GetTextSize());
			musicInfoScrollTextButton[1]->SetButtonSize(musicInfoScrollText[1].GetTextSize());

			musicInfoScrollTextRewind = true;
			musicInfoScrollTextLoaded = false;
		}
	
		if (musicInfoScrollTextRewind == true)
		{
			musicInfoScrollText[0].SetTextPos(glm::vec2(musicInfoScrollText[0].GetTextPos().x + Data::MusicInfoScrollingSpeedRewind * Time::deltaTime,
				musicInfoScrollText[0].GetTextPos().y));

			s32 rewindOffsetX = 5;
			if (musicInfoScrollText[0].GetTextPos().x > Data::_MAIN_FOREGROUND_POS.x + rewindOffsetX &&
				musicInfoScrollTextTimer.started == false)
			{
				musicInfoScrollTextTimer.Start();
				musicInfoScrollTextRewind = false;
			}


		}


		f32 scrollingSpeed = 0;
		if (musicInfoScrollTextTimer.finished == true)
		{
			musicInfoScrollTextLoaded = true;
			musicInfoScrollTextTimer.Reset();
		}

		if (musicInfoScrollTextLoaded == true)
		{
			if (musicInfoScrollTextIsMoved == false)
				scrollingSpeed = Data::MusicInfoScrollingSpeed;
			else
				scrollingSpeed = 0;

			musicInfoScrollText[musicInfoScrollPingPong].SetTextPos(glm::vec2(musicInfoScrollText[musicInfoScrollPingPong].GetTextPos().x - scrollingSpeed * musicInfoScrollTextDirection * Time::deltaTime,
																			  musicInfoScrollText[musicInfoScrollPingPong].GetTextPos().y));


			// User can set scrolling direction by moving the text in that direction
			if (musicInfoScrollTextDirection == 1)
			{
				if (musicInfoScrollText[musicInfoScrollPingPong].GetTextPos().x + musicInfoScrollText[musicInfoScrollPingPong].GetTextSize().x < Data::_MAIN_FOREGROUND_POS.x)
				{
					musicInfoScrollText[musicInfoScrollPingPong].SetTextPos(glm::vec2(Data::_DEFAULT_PLAYER_SIZE.x - Data::_MAIN_FOREGROUND_POS.x,
																					  musicInfoScrollText[musicInfoScrollPingPong].GetTextPos().y));

					musicInfoScrollPingPong = !musicInfoScrollPingPong;
				};
			}
			else
			{
				if (musicInfoScrollText[musicInfoScrollPingPong].GetTextPos().x > Data::_DEFAULT_PLAYER_SIZE.x - Data::_MAIN_FOREGROUND_POS.x)
				{
					musicInfoScrollText[musicInfoScrollPingPong].SetTextPos(glm::vec2(Data::_MAIN_FOREGROUND_POS.x - musicInfoScrollText[musicInfoScrollPingPong].GetTextSize().x,
																					  musicInfoScrollText[musicInfoScrollPingPong].GetTextPos().y));

					musicInfoScrollPingPong = !musicInfoScrollPingPong;
				}
			}

			// Render second text to the start or end of the first text, to make effect on infinite text string
			if (musicInfoScrollText[musicInfoScrollPingPong].GetTextPos().x - Data::MusicInfoScrollTextDistDiff > Data::_MAIN_FOREGROUND_POS.x)
			{
				musicInfoScrollText[!musicInfoScrollPingPong].SetTextPos(glm::vec2(musicInfoScrollText[musicInfoScrollPingPong].GetTextPos().x - musicInfoScrollText[!musicInfoScrollPingPong].GetTextSize().x - Data::MusicInfoScrollTextDistDiff,
																				   musicInfoScrollText[musicInfoScrollPingPong].GetTextPos().y));
			}
			else if (musicInfoScrollText[musicInfoScrollPingPong].GetTextPos().x + musicInfoScrollText[musicInfoScrollPingPong].GetTextSize().x + Data::MusicInfoScrollTextDistDiff < Data::_DEFAULT_PLAYER_SIZE.x - Data::_MAIN_FOREGROUND_POS.x)
			{
				musicInfoScrollText[!musicInfoScrollPingPong].SetTextPos(glm::vec2(musicInfoScrollText[musicInfoScrollPingPong].GetTextPos().x + musicInfoScrollText[musicInfoScrollPingPong].GetTextSize().x + Data::MusicInfoScrollTextDistDiff,
																				   musicInfoScrollText[musicInfoScrollPingPong].GetTextPos().y));
			}
			else
			{
				// Prevent the second text from staying on the screen when it shouldn't
				musicInfoScrollText[!musicInfoScrollPingPong].SetTextPos(glm::vec2(Data::_DEFAULT_PLAYER_SIZE.x, musicInfoScrollText[!musicInfoScrollPingPong].GetTextPos().y));
			}

		}

		// Check if user grabbed the text
		if ((musicInfoScrollTextButton[0]->isPressed == true || musicInfoScrollTextButton[1]->isPressed == true))
		{
			musicInfoScrollTextRewind = false;
			musicInfoScrollTextLoaded = true;
			if (musicInfoScrollTextTimer.finished == false)
				musicInfoScrollTextTimer.Reset();
			musicInfoScrollTextIsMoved = true;
		}

		if (musicInfoScrollTextIsMoved == true)
		{
			s32 relX, relY;
			App::Input::GetRelavtiveMousePosition(&relX, &relY);

			musicInfoScrollText[musicInfoScrollPingPong].SetTextPos(glm::vec2(musicInfoScrollText[musicInfoScrollPingPong].GetTextPos().x + relX,
																			  musicInfoScrollText[musicInfoScrollPingPong].GetTextPos().y));


			if (relX < 0)
				musicInfoScrollTextDirection = 1;
			else
				musicInfoScrollTextDirection = -1;

			// Update text positions. Have to check it here again to reduce bug, in which second text sometimes wasn't in approperiate position
			if (musicInfoScrollText[musicInfoScrollPingPong].GetTextPos().x - 40.f > Data::_MAIN_FOREGROUND_POS.x)
			{
				musicInfoScrollText[!musicInfoScrollPingPong].SetTextPos(glm::vec2(musicInfoScrollText[musicInfoScrollPingPong].GetTextPos().x - musicInfoScrollText[!musicInfoScrollPingPong].GetTextSize().x - Data::MusicInfoScrollTextDistDiff,
																				   musicInfoScrollText[musicInfoScrollPingPong].GetTextPos().y));
			}
			else if (musicInfoScrollText[musicInfoScrollPingPong].GetTextPos().x + musicInfoScrollText[musicInfoScrollPingPong].GetTextSize().x + Data::MusicInfoScrollTextDistDiff < Data::_DEFAULT_PLAYER_SIZE.x - Data::_MAIN_FOREGROUND_POS.x)
			{
				musicInfoScrollText[!musicInfoScrollPingPong].SetTextPos(glm::vec2(musicInfoScrollText[musicInfoScrollPingPong].GetTextPos().x + musicInfoScrollText[musicInfoScrollPingPong].GetTextSize().x + Data::MusicInfoScrollTextDistDiff,
																				   musicInfoScrollText[musicInfoScrollPingPong].GetTextPos().y));
			}
		}
		
		if (App::Input::IsKeyDown(App::KeyCode::MouseLeft) == false)
		{
			musicInfoScrollTextIsMoved = false;
		}

		// Cut text that is out of bounds of main player foreground
		Shader::shaderDefault->use();
		Shader::shaderDefault->setBool("playlistCutX", true);
		Shader::shaderDefault->setVec2("playlistBoundsX", Data::_MAIN_FOREGROUND_POS.x,
														  Data::_DEFAULT_PLAYER_SIZE.x - Data::_MAIN_FOREGROUND_POS.x);
		musicInfoScrollText[0].DrawString();
		musicInfoScrollText[1].DrawString();
		Shader::shaderDefault->setBool("playlistCutX", false);
		Shader::shaderDefault->setVec3("color", Color::White);

		// Update texts hitboxes
		musicInfoScrollTextButton[musicInfoScrollPingPong]->SetButtonPos(musicInfoScrollText[musicInfoScrollPingPong].GetTextPos());
		musicInfoScrollTextButton[!musicInfoScrollPingPong]->SetButtonPos(musicInfoScrollText[!musicInfoScrollPingPong].GetTextPos());

		// update timer
		musicInfoScrollTextTimer.Update();
	}

	void MP::RenderScrollBar(f32* playlistOffset, f32 displayedItems, f32 maxItems)
	{
		auto sepCon = Interface::Separator::GetContainer();
		auto audioCon = Audio::Object::GetAudioObjectContainer();
		f32 separatorH = Data::_PLAYLIST_SEPARATOR_SIZE.y;
		f32 itemH = Data::_PLAYLIST_ITEM_SIZE.y;
		s32 bottomPlaylistBorder = Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y;
	
		f32 newDis = displayedItems - (visibleSeparatorsCount * separatorH) / itemH;
		if (newDis < Audio::Object::GetSize())
		{
			//displayedItems += 3;
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

				if(relY != 0)
					State::SetState(State::PlaylistMovement);

				f32 diff = abs(audioCon.back()->GetPlaylistItemPos().y - audioCon.front()->GetPlaylistItemPos().y) / scrollSurface;
				*playlistOffset += ((relY * (maxItems + separatorsOffset / itemH)) / (displayedItems));


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

	void MP::RenderPlaylistItems()
	{
		if (GetPlaylistObject()->IsToggled() == false &&
			GetPlaylistObject()->IsEnabled() == false)
		{
			playlistAddFileActive = false;
		}

		if (GetPlaylistObject()->IsToggled() &&
			GetPlaylistObject()->IsEnabled() &&
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

			s32 upperBoundOfVisible = Data::_PLAYLIST_ITEMS_SURFACE_POS.y - 2 * itemH;
			s32 lowerBoundOfVisible = Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y + 2 * itemH;

			f32 displayedItems = float(Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y - Data::_PLAYLIST_ITEMS_SURFACE_POS.y) / itemH;
			
			if (maxPosToRender > audioCon.size() - 1)
				maxPosToRender = audioCon.size() - 1;
			s32 passedSeparatorsCount = 0;
			f32 scrollStep = Data::PlaylistScrollStep;
			/*if(playlistFirstEnter == false)
			{
				for (auto & i : *sepCon)
				{
					if (i.second->GetPlaylistItemPos().y < audioCon[minPosToRender]->GetButtonPos().y &&
						playlistPositionOffset > 0)
					{
						i.second->Visible(false);
						passedSeparatorsCount++;
					}
					else if (i.second->GetPlaylistItemPos().y < audioCon[maxPosToRender]->GetButtonPos().y)
					{
						i.second->Visible(true);
					}
					else
					{
						i.second->Visible(false);
					}
				}
			}*/




			if (mdEngine::App::Input::IsScrollForwardActive() && GetPlaylistObject()->hasFocus())
			{
				playlistPositionOffset -= scrollStep;
				State::SetState(State::PlaylistMovement);
			}
			if (mdEngine::App::Input::IsScrollBackwardActive() && GetPlaylistObject()->hasFocus())
			{
				playlistPositionOffset += scrollStep;
				State::SetState(State::PlaylistMovement);
			}

			// Render playlist scroll bar
			//RenderScrollBar(&playlistPositionOffset, displayedItems, audioCon.size());

			if (State::CheckState(State::PlaylistMovement) == true)
			{
				if (displayedItems > audioCon.size() + (sepCon->size() * separatorH) / itemH)
				{
					playlistPositionOffset = 0;
				}
			}
		
			if ((State::CheckState(State::AudioChanged) == true ||
				State::CheckState(State::InitialLoadFromFile) == true) &&
				GetPlaylistObject()->GetPlayingID() >= 0)
			{
				s32 playingID = GetPlaylistObject()->GetPlayingID();

				s32 indexOfLast = minPosToRender + displayedItems - 2;
				s32 indexOfFirst = minPosToRender + 3;
				if (indexOfLast == audioCon.size() - 2 ||
					indexOfFirst == 0)
				{
					State::ResetState(State::AudioChanged);
					State::ResetState(State::InitialLoadFromFile);
					loadItemsTextures = true;
				}

				if (indexOfLast > audioCon.size() - 1)
					indexOfLast > audioCon.size() - 1;

				assert(audioCon[playingID] != NULL);
				while (audioCon[playingID]->GetPlaylistItemPos().y > Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y - 2 * itemH)
				{
					playlistPositionOffset += itemH;
				}

				while(audioCon[playingID]->GetPlaylistItemPos().y < Data::_PLAYLIST_ITEMS_SURFACE_POS.y + 1 * itemH &&
						playlistPositionOffset > 0)
				{
					playlistPositionOffset -= itemH;
				}

				State::ResetState(State::AudioChanged);
				State::ResetState(State::InitialLoadFromFile);
				loadItemsTextures = true;
			}
			else
				State::ResetState(State::InitialLoadFromFile);


			if (playlistPositionOffset < 0)
			{
				playlistPositionOffset = 0;
			}

			/*if(displayedItems > audioCon.size())
				playlistPositionOffset = 0;*/

			if (abs(playlistPositionOffsetPrevious - playlistPositionOffset) > 0)
			{
				// Reduce loop iterations(now most of the time O(1))

				s32 diff = 0;
				if (abs(playlistPositionOffsetPrevious - playlistPositionOffset) > itemH)
				{
					diff = abs(playlistPositionOffsetPrevious - playlistPositionOffset) / itemH + 2;
				}
				else
				{
					diff = 2;
				}

				for (s32 i = (minPosToRender - diff >= 0 ? minPosToRender - diff : 0);
					     i < audioCon.size(); 
					     i++)
				{
					if (audioCon[i]->GetPlaylistItemPos().y <= upperBoundOfVisible &&
						audioCon[i]->GetPlaylistItemPos().y + itemH + separatorH >= upperBoundOfVisible)
					{
						minPosToRender = audioCon[i]->GetID();
						break;
					}
				}

				playlistPositionOffsetPrevious = playlistPositionOffset;

				loadItemsTextures = true;
			}

			//displayedItems -= (visibleSeparatorsCount * separatorH) / itemH;

			RenderScrollBar(&playlistPositionOffset, displayedItems, audioCon.size());

			if (playlistPositionOffset < 0)
			{
				playlistPositionOffset = 0;
			}

			if (State::CheckState(State::ContainersResized) == false)
			{
				if (audioCon.back()->GetPlaylistItemPos().y + itemH < Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y &&
					playlistPositionOffset > 0)
				{
					playlistPositionOffset = audioCon.back()->GetButtonPos().y - Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y + itemH;
				}
			}

			// Render playlist scroll bar after playlist position offset is checked

			/*  Sometimes method that is used to obtain playlist position is not accurate, when playlist scroll bar
				is moved really quick. Make sure that playlist position is accurate.
				TRY TO FIX THAT IN MORE EFFICIENT WAY
			*/

			if (minPosToRender > audioCon.size() - 1)
				minPosToRender = audioCon.size() - 1;
			assert(audioCon[minPosToRender] != NULL);
			while (playlistCursorPosition < audioCon[minPosToRender]->GetPlaylistItemPos().y &&
				minPosToRender > 0)
			{
				minPosToRender--;
			}
			assert(audioCon[minPosToRender] != NULL);
			while (playlistCursorPosition > audioCon[minPosToRender]->GetPlaylistItemPos().y + audioCon[minPosToRender]->GetButtonSize().y &&
				minPosToRender < audioCon.size() - 1)
			{
				minPosToRender++;
			}


			if (minPosToRender < 0)
				minPosToRender = 0;
			if (playlistPositionOffset < 2 * itemH)
				minPosToRender = 0;

			// Find the max position that will be visible considering playlist separators size
			maxPosToRender = minPosToRender + displayedItems + 2;
			if (maxPosToRender > audioCon.size() - 1)
				maxPosToRender = audioCon.size() - 1;

			if (audioCon[maxPosToRender] == NULL || audioCon[minPosToRender] == NULL)
			{
				maxPosToRender = displayedItems + 3;
			}
			else
			{
				assert(audioCon[minPosToRender] != NULL);
				assert(audioCon[maxPosToRender] != NULL);
				f32 posDifference = audioCon[maxPosToRender]->GetPlaylistItemPos().y - audioCon[minPosToRender]->GetPlaylistItemPos().y;
				displayedItems = posDifference / itemH;
				displayedItems += 2;
				maxPosToRender = minPosToRender + displayedItems + 2;
			}
			if (maxPosToRender > audioCon.size())
				maxPosToRender = audioCon.size();



			//md_log_compare(minPosToRender, maxPosToRender);

			b8 loadTextureFirstEnter(false);
			if (State::CheckState(State::AudioAdded) == true ||
				State::CheckState(State::AudioDeleted) == true ||
				State::CheckState(State::Window::Resized) == true ||
				playlistOpened == false)
			{
				loadItemsPositions = true;
				playlistFirstEnter = false;
				loadTextureFirstEnter = true;
				playlistOpened = true;
				//State::ResetMusicPlayerState();
				//State::ResetState(State::Window::Resized);
			}


			if (loadItemsPositions == true)
			{
				glm::vec2 startPos = glm::vec2(Data::_PLAYLIST_ITEMS_SURFACE_POS.x,
											   Data::_PLAYLIST_ITEMS_SURFACE_POS.y);

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

				visibleSeparatorsCount = 0;
				for (auto & i : *sepCon)
				{
					if (i.second->GetPlaylistItemPos().y <= lowerBoundOfVisible &&
						i.second->GetPlaylistItemPos().y >= upperBoundOfVisible)
					{
						i.second->InitTextTexture();
						visibleSeparatorsCount++;
					}
				}

				loadItemsPositions = false;
				loadItemsTextures = true;
			}

			if (loadItemsTextures == true)
			{

				std::vector<s32> indexes;
				//md_log_compare(minPosToRender, maxPosToRender);

				if (State::CheckState(State::AudioAdded) == true && Audio::Object::GetSize() > 0 && 
					State::CheckState(State::PathLoadedFromFileVolatile) == false)
				{
					s32 diff = 0;
					s32 count = maxPosToRender - minPosToRender;
					for (s32 i = minPosToRender; i < Audio::GetDroppedOnIndex(); i++)
					{
						std::cout << i << ", ";
						diff++;
					}

					for (s32 i = maxPosToRender + Audio::GetFilesAddedCount(); i < maxPosToRender + Audio::GetFilesAddedCount() + count - diff; i++)
					{
						if (i > Audio::Object::GetSize() - 1)
							break;
						std::cout << i << ", ";
					}

					std::cout << std::endl;
				}

				for (s32 i = minPosToRender; i < maxPosToRender; i++)
				{
					indexes.push_back(i);
					audioCon[i]->ReloadTextTexture();
				}

				visibleSeparatorsCount = 0;
				for (auto & i : *sepCon)
				{
					if (i.second->GetPlaylistItemPos().y <= lowerBoundOfVisible &&
						i.second->GetPlaylistItemPos().y >= upperBoundOfVisible)
					{
						i.second->InitTextTexture();
						i.second->Visible(true);
						visibleSeparatorsCount++;
					}
					else
					{
						i.second->DeleteTexture();
						i.second->Visible(false);
					}
				}

				GetPlaylistObject()->SetIndexesToRender(indexes);
				loadItemsTextures = false;
			}

			// ACTUAL RENDERING

			Shader::shaderDefault->use();
			Shader::shaderDefault->setBool("playlistCutY", true);
			Shader::shaderDefault->setVec2("playlistBoundsY", Data::_PLAYLIST_ITEMS_SURFACE_POS.y, Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y);
			Shader::shaderBorder->use();
			Shader::shaderBorder->setVec2("playlistBoundsY", Data::_PLAYLIST_ITEMS_SURFACE_POS.y, Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y);

			//md_log_compare(minPosToRender, maxPosToRender);
			std::vector<s32> indexes;
			// Render ALL playlist separators
			for (auto & i : *sepCon)
			{
				if (i.second->IsVisible() == true)
				{
					i.second->DrawItem(main_foreground);
				}
			}

			// Render VISIBLE playlist items
			for (s32 i = minPosToRender; i < maxPosToRender; i++)
			{
				if (audioCon[i] == NULL)
					break;

				if (audioCon[i]->HasTexture() == false)
				{
					Shader::shaderDefault->setVec3("color", Color::White);
					continue;
				}
				audioCon[i]->DrawItem(main_foreground);
			}

			
			for (auto & i : GetPlaylistObject()->multipleSelect)
			{
				if (*i < 0 && *i > Audio::Object::GetSize() - 1)
					break;
				audioCon[*i]->DrawDottedBorder();
			}

			Shader::shaderDefault->use();
			Shader::shaderDefault->setBool("playlistCutY", false);
			Shader::shaderDefault->setVec3("color", Color::White);
		}
		else
		{
			playlistPositionOffset = 0;

			playlistFirstEnter = true;
			playlistOpened = false;
		}
	}

	void MP::RenderPlaylistAddButtons()
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

		model = glm::translate(model, glm::vec3(glm::vec2(Data::_PLAYLIST_ADD_BUTTON_POS.x, Window::windowProperties.mApplicationHeight - 35.f), 0.6));
		model = glm::scale(model, glm::vec3(Data::_PLAYLIST_ADD_BUTTON_SIZE, 1.0));
		Shader::shaderDefault->setMat4("model", model);
		Shader::shaderDefault->setVec3("color", color);
		glBindTexture(GL_TEXTURE_2D, playlist_add_file);
		Shader::Draw(Shader::shaderDefault);

		Shader::shaderDefault->setVec3("color", Color::White);
	}

	void MP::RenderSettingsButtons()
	{
		// UI left
		Shader::shaderDefault->setVec3("color", Color::White);

		glm::mat4 model;
		model = glm::translate(model, glm::vec3(Data::_UI_BUTTONS_BACKGROUND_LEFT_POS, 0.2f));
		model = glm::scale(model, glm::vec3(Data::_UI_BUTTONS_BACKGROUND_LEFT_SIZE, 1.f));;
		Shader::shaderDefault->setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, ui_buttons_background_left);
		Shader::Draw(Shader::shaderDefault);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(Data::_SETTINGS_BUTTON_BACKGROUND_POS, 0.3f));
		model = glm::scale(model, glm::vec3(Data::_SETTINGS_BUTTON_BACKGROUND_SIZE, 1.f));;
		Shader::shaderDefault->setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, settings_background);
		Shader::Draw(Shader::shaderDefault);


		if (Input::hasFocus(Input::ButtonType::Options) == true && 
			Input::isButtonPressed(Input::ButtonType::Options) == false)
		{
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(Data::_SETTINGS_BUTTON_POS, 0.4f));
			model = glm::scale(model, glm::vec3(Data::_SETTINGS_BUTTON_SIZE, 1.f));;
			Shader::shaderDefault->setMat4("model", model);
			glBindTexture(GL_TEXTURE_2D, settings_background_glow);
			Shader::Draw(Shader::shaderDefault);
		}

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(Data::_SETTINGS_BUTTON_POS, 0.4f));
		model = glm::scale(model, glm::vec3(Data::_SETTINGS_BUTTON_SIZE, 1.f));;
		Shader::shaderDefault->setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, settings_icon);
		Shader::Draw(Shader::shaderDefault);
	}

	void MP::RenderWindowControlButtons()
	{
		glm::mat4 model;
		/* UI Window buttons*/
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(Data::_UI_BUTTONS_BACKGROUND_RIGHT_POS, 0.2f));
		model = glm::scale(model, glm::vec3(Data::_UI_BUTTONS_BACKGROUND_RIGHT_SIZE, 1.f));;
		Shader::shaderDefault->setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, ui_buttons_background);
		Shader::Draw(Shader::shaderDefault);


		model = glm::mat4();
		model = glm::translate(model, glm::vec3(Data::_EXIT_BUTTON_BACKGROUND_POS, 0.3f));
		model = glm::scale(model, glm::vec3(Data::_EXIT_BUTTON_BACKGROUND_SIZE, 1.f));
		Shader::shaderDefault->setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, exit_background);
		Shader::Draw(Shader::shaderDefault);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(Data::_STAY_ON_TOP_BUTTON_BACKGROUND_POS, 0.3f));
		model = glm::scale(model, glm::vec3(Data::_STAY_ON_TOP_BUTTON_BACKGROUND_SIZE, 1.f));
		Shader::shaderDefault->setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, stay_on_top_background);
		Shader::Draw(Shader::shaderDefault);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(Data::_MINIMIZE_BUTTON_BACKGROUND_POS, 0.3f));
		model = glm::scale(model, glm::vec3(Data::_MINIMIZE_BUTTON_BACKGROUND_SIZE, 1.f));
		Shader::shaderDefault->setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, minimize_background);
		Shader::Draw(Shader::shaderDefault);





		// Buttons glow
		b8 buttonHasFocus(false);
		GLuint tex = 0;
		model = glm::mat4();
		if (Input::hasFocus(Input::ButtonType::Exit) == true)
		{
			glm::vec2 newSize = Data::_EXIT_BUTTON_SIZE * Data::_UI_BUTTONS_GLOW_SCALE;
			model = glm::translate(model, glm::vec3(glm::vec2(Data::_EXIT_BUTTON_POS.x - (newSize.x - Data::_EXIT_BUTTON_SIZE.x ) / 2.f,
															  Data::_EXIT_BUTTON_POS.y - (newSize.y - Data::_EXIT_BUTTON_SIZE.y) / 2.f), 
													0.35f));

			model = glm::scale(model, glm::vec3(newSize, 1.f));
			tex = exit_background_glow;
			buttonHasFocus = true;
		}
		else if (Input::hasFocus(Input::ButtonType::StayOnTop) == true)
		{
			glm::vec2 newSize = Data::_STAY_ON_TOP_BUTTON_SIZE * Data::_UI_BUTTONS_GLOW_SCALE;
			model = glm::translate(model, glm::vec3(glm::vec2(Data::_STAY_ON_TOP_BUTTON_POS.x - (newSize.x - Data::_STAY_ON_TOP_BUTTON_SIZE.x) / 2.f,
															  Data::_STAY_ON_TOP_BUTTON_POS.y - (newSize.y - Data::_STAY_ON_TOP_BUTTON_SIZE.y) / 2.f),
													0.35f));
			model = glm::scale(model, glm::vec3(newSize, 1.f));
			tex = stay_on_top_background_glow;
			buttonHasFocus = true;
		}
		else if (Input::hasFocus(Input::ButtonType::Minimize) == true)
		{
			glm::vec2 newSize = Data::_MINIMIZE_BUTTON_SIZE * Data::_UI_BUTTONS_GLOW_SCALE;
			model = glm::translate(model, glm::vec3(glm::vec2(Data::_MINIMIZE_BUTTON_POS.x - (newSize.x - Data::_MINIMIZE_BUTTON_SIZE.x) / 2.f,
															  Data::_MINIMIZE_BUTTON_POS.y - (newSize.y - Data::_MINIMIZE_BUTTON_SIZE.y) / 2.f),
													0.35f));
			model = glm::scale(model, glm::vec3(newSize, 1.f));
			tex = minimize_background_glow;
			buttonHasFocus = true;
		}

		if (buttonHasFocus == true)
		{
			Shader::shaderDefault->setMat4("model", model);
			glBindTexture(GL_TEXTURE_2D, tex);
			Shader::Draw(Shader::shaderDefault);
		}
		
		// Actual buttons
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(Data::_EXIT_BUTTON_POS, 0.4f));
		model = glm::scale(model, glm::vec3(Data::_EXIT_BUTTON_SIZE, 1.f));;
		Shader::shaderDefault->setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, exit_icon);
		Shader::Draw(Shader::shaderDefault);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(Data::_STAY_ON_TOP_BUTTON_POS, 0.4f));
		model = glm::scale(model, glm::vec3(Data::_STAY_ON_TOP_BUTTON_SIZE, 1.f));;
		Shader::shaderDefault->setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, stay_on_top_icon);
		Shader::Draw(Shader::shaderDefault);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(Data::_MINIMIZE_BUTTON_POS, 0.4f));
		model = glm::scale(model, glm::vec3(Data::_MINIMIZE_BUTTON_SIZE, 1.f));;
		Shader::shaderDefault->setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, minimize_icon);
		Shader::Draw(Shader::shaderDefault);
	}

	void MP::StartMainWindow()
	{
		main_background				= mdLoadTexture("assets/main.png");
		main_foreground				= mdLoadTexture("assets/main.png");

		ui_buttons_background		= mdLoadTexture("assets/ui_buttons_background.png");
		ui_buttons_background_left  = mdLoadTexture("assets/ui_buttons_background_left.png");
		exit_background				= mdLoadTexture("assets/exit_background.png");
		stay_on_top_background		= mdLoadTexture("assets/stay_on_top_background.png");
		minimize_background			= mdLoadTexture("assets/minimize_background.png");
		settings_background			= mdLoadTexture("assets/settings_background.png");
		exit_icon					= mdLoadTexture("assets/exit_icon.png");
		stay_on_top_icon			= mdLoadTexture("assets/stay_on_top_icon.png");
		minimize_icon				= mdLoadTexture("assets/minimize_icon.png");
		settings_icon				= mdLoadTexture("assets/settings_icon.png");
		exit_background_glow		= mdLoadTexture("assets/exit_icon_glow.png");
		minimize_background_glow	= mdLoadTexture("assets/minimize_icon_glow.png");
		stay_on_top_background_glow = mdLoadTexture("assets/stay_on_top_icon_glow.png");
		settings_background_glow	= mdLoadTexture("assets/settings_icon_glow.png");
		

		volume_bar				= mdLoadTexture("assets/volume_bar.png");
		volume_speaker			= mdLoadTexture("assets/volume_speaker.png");
		volume_speaker_muted	= mdLoadTexture("assets/volume_speaker_muted.png");
		volume_speaker_low		= mdLoadTexture("assets/volume_speaker_low.png");
		volume_speaker_medium	= mdLoadTexture("assets/volume_speaker_medium.png");;


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

		musicInfoScrollTextTimer = Time::Timer(Data::MusicInfoScrollStopTimer);
		musicInfoScrollTextButton[0] = new Interface::Button(Input::ButtonType::Other, glm::vec2(), glm::vec2());
		musicInfoScrollTextButton[1] = new Interface::Button(Input::ButtonType::Other, glm::vec2(), glm::vec2());

		InitializeText();

		InitializeTextBoxes();

		Interface::PlaylistSeparator::SetPlaylistOffsetY(&playlistPositionOffset);
		Audio::AudioObject::SetPlaylistOffsetY(&playlistPositionOffset);
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
			GetPlaylistObject()->SetItemsDuration(0.f);
			GetPlaylistObject()->SetItemsSize(0.f);

			durationText.SetTextString(utf8_to_utf16(GetPlaylistObject()->GetItemsDurationString()));
			itemsSizeText.SetTextString(utf8_to_utf16(GetPlaylistObject()->GetItemsSizeString()));
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


			GetPlaylistObject()->SetItemsDuration(duration);
			GetPlaylistObject()->SetItemsSize(size);

			durationText.SetTextString(utf8_to_utf16(GetPlaylistObject()->GetItemsDurationString()));
			itemsSizeText.SetTextString(utf8_to_utf16(GetPlaylistObject()->GetItemsSizeString()));
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

			GetPlaylistObject()->SetItemsDuration(m_Playlist.GetItemsDuration());
			GetPlaylistObject()->SetItemsSize(m_Playlist.GetItemsSize());

			durationText.SetTextString(utf8_to_utf16(GetPlaylistObject()->GetItemsDurationString()));
			itemsSizeText.SetTextString(utf8_to_utf16(GetPlaylistObject()->GetItemsSizeString()));
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

		RenderMusicScrollInfo();

		RenderPlaylistItems();

		RenderWindowControlButtons();

		RenderSettingsButtons();

		Input::SetButtonExtraState(volumeSliderActive || musicSliderActive || playlistSliderActive || 
								   UI::fileBrowserActive || playlistAddFileActive);

	}

	void MP::CloseMainWindow()
	{
		durationText.DeleteTexture();
		itemsSizeText.DeleteTexture();
		itemsCountText.DeleteTexture();
		loadedItemsCountText.DeleteTexture();
		musicInfoScrollText[0].DeleteTexture();
		musicInfoScrollText[1].DeleteTexture();

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
			std::cout << audioCon[i]->GetPlaylistItemPos().x;
			std::cout << " || ";
			std::cout << "Pos Y: ";
			std::cout << audioCon[i]->GetPlaylistItemPos().y;
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
			std::cout << i->GetPlaylistItemPos().x;
			std::cout << " || ";
			std::cout << "Pos Y: ";
			std::cout << i->GetPlaylistItemPos().y;
			std::cout << "\n";

		}
	}

	void MP::PrintIndexesToRender()
	{
		for (auto i : GetPlaylistObject()->GetIndexesToRender())
		{
			std::cout << i << " : ";
		}
		std::cout << "\n";
		std::cout << minPos << "    " << maxPos << std::endl;
	}
#endif
}
}