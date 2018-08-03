#include "music_player_ui.h"

#include <iostream>
#include <vector>
#include <locale.h>


#ifdef _DEBUG_
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "md_load_texture.h"
#include "md_shape.h"
#endif


#include "../external/imgui/imgui.h"
#include "music_player_playlist.h"
#include "realtime_system_application.h"
#include "application_window.h"
#include "realtime_application.h"
#include "md_shader.h"
#include "input.h"
#include "graphics.h"
#include "md_text.h"
#include "music_player_string.h"


using namespace mdEngine::Graphics;

namespace mdEngine
{
namespace MP
{
	namespace UI
	{
		std::vector<Movable*> mdMovableContainer;
		std::vector<PlaylistItem*> mdItemContainer;
		std::vector<Resizable*> mdResizableContainer;
		std::vector<std::pair<Input::ButtonType, Button*>> mdButtonsContainer;

#ifdef _DEBUG_
		GLuint mdDebugTex;
		b8 renderDebug = false;
		glm::vec3 buttonColor(0.f, 1.f, 0.f);
#endif
		
		b8 music_repeat = false;
		b8 music_shuffle = false;
		s32 music_position = 0;
		
		s32 volume_scroll_step = 2;
		s32 volume_fade_time = 500;

		f32 mdDefaultWidth;
		f32 mdDefaultHeight;
		f32 mdCurrentWidth;
		f32 mdCurrentHeight;
		f32 mdProjectionHeight;

		std::string title = "none";

		ImVec4 ClearColor = ImVec4(1.f, 254.f/255.f, 1.f, 1.f);

		std::locale l("");
		void HandleInput();

		void DebugStart();

		void DebugRender();

		namespace Data
		{
			TTF_Font* _MUSIC_PLAYER_FONT;

			glm::vec2 _MIN_PLAYER_SIZE;

			glm::vec2 _DEFAULT_PLAYER_POS;
			glm::vec2 _DEFAULT_PLAYER_SIZE;

			glm::vec2 _DEFAULT_WINDOW_POS;
			glm::vec2 _DEFAULT_WINDOW_SIZE;

			glm::vec2 _MAIN_BACKGROUND_POS;
			glm::vec2 _MAIN_BACKGROUND_SIZE;

			glm::vec2 _MAIN_FOREGROUND_POS;
			glm::vec2 _MAIN_FOREGROUND_SIZE;

			glm::vec2 _PLAYLIST_FOREGROUND_POS;
			glm::vec2 _PLAYLIST_FOREGROUND_SIZE;

			glm::vec2 _VOLUME_BAR_POS;
			glm::vec2 _VOLUME_BAR_SIZE;

			glm::vec2 _MUSIC_PROGRESS_BAR_POS;
			glm::vec2 _MUSIC_PROGRESS_BAR_SIZE;

			glm::vec2 _VOLUME_SPEAKER_POS;
			glm::vec2 _VOLUME_SPEAKER_SIZE;

			glm::vec2 _MUSIC_PROGRESS_BAR_DOT_POS;
			glm::vec2 _VOLUME_BAR_DOT_POS;
			glm::vec2 _SLIDER_DOT_SIZE;

			glm::vec2 _UI_WINDOW_BAR_POS;
			glm::vec2 _UI_WINDOW_BAR_SIZE;

			glm::vec2 _EXIT_BUTTON_POS;
			glm::vec2 _EXIT_BUTTON_SIZE;

			glm::vec2 _MINIMIZE_BUTTON_POS;
			glm::vec2 _MINIMIZE_BUTTON_SIZE;

			glm::vec2 _STAY_ON_TOP_BUTTON_POS;
			glm::vec2 _STAY_ON_TOP_BUTTON_SIZE;

			glm::vec2 _PLAY_BUTTON_POS;
			glm::vec2 _PLAY_BUTTON_SIZE;

			glm::vec2 _NEXT_BUTTON_POS;
			glm::vec2 _NEXT_BUTTON_SIZE;

			glm::vec2 _PREVIOUS_BUTTON_POS;
			glm::vec2 _PREVIOUS_BUTTON_SIZE;

			glm::vec2 _SHUFFLE_BUTTON_POS;
			glm::vec2 _SHUFFLE_BUTTON_SIZE;

			glm::vec2 _REPEAT_BUTTON_POS;
			glm::vec2 _REPEAT_BUTTON_SIZE;

			glm::vec2 _DOT_BUTTON_STATE_SIZE;

			glm::vec2 _PLAYLIST_BUTTON_POS;
			glm::vec2 _PLAYLIST_BUTTON_SIZE;

			glm::vec2 _PLAYLIST_ITEMS_SURFACE_POS;
			glm::vec2 _PLAYLIST_ITEMS_SURFACE_SIZE;

			glm::vec2 _PLAYLIST_ITEM_SIZE;

			void InitializeData();

			void UpdateData();
		}

	}

	UI::Movable::Movable(glm::vec2 size, glm::vec2 pos) : mSize(size), mPos(pos)
	{ 
		mdMovableContainer.push_back(this);
	}

	UI::Movable::Movable() { }

	UI::Movable::~Movable() { delete this; }

	UI::Resizable::Resizable(glm::vec2 size, glm::vec2 pos) : size(size), pos(pos) 
	{ 
		mdResizableContainer.push_back(this);
	}

	UI::Resizable::Resizable () { }

	UI::Resizable::~Resizable() { }

	UI::Button::Button(Input::ButtonType type, glm::vec2 size, glm::vec2 pos) : mSize(size), mPos(pos)
	{
		mdButtonsContainer.push_back(std::make_pair(type, this));
	}

	UI::Button::Button() { }

	UI::Button::~Button() { delete this; }

	UI::PlaylistItem::PlaylistItem() { }

	UI::PlaylistItem::~PlaylistItem() { delete this; }

	void UI::PlaylistItem::InitFont()
	{
		mFont = Data::_MUSIC_PLAYER_FONT;
		if (mFont == NULL)
		{
			std::cout << "ERROR: initfont!\n";
		}

		mTextScale = 1.0f;

	}

	void UI::PlaylistItem::InitItem()
	{
		mColor = glm::vec3(1.f);

		mStartPos = Data::_PLAYLIST_ITEMS_SURFACE_POS;
		mSize = Data::_PLAYLIST_ITEM_SIZE;

		mPos = glm::vec2(mStartPos.x, mStartPos.y + mCount * (mSize.y + mOffsetY));

		mTextColor = { 255, 255, 255 };

		mID = mCount;
		mTitle = Playlist::GetTitle(mID);

		u16 len = wcslen(mTitle.c_str());
		mTitleC = new char[len + 1];
		mTitleC[len] = '\0';
		wcstombs(mTitleC, mTitle.c_str(), len);

		TTF_SizeText(mFont, mTitleC, &mTextSize.x, &mTextSize.y);

		mdItemContainer.push_back(this);
		mdButtonsContainer.push_back(std::make_pair(Input::ButtonType::None, this));

		mCount++;
	}

	void UI::PlaylistItem::UpdateItem()
	{
		mTitle = Playlist::GetTitle(mID);
		u16 len = wcslen(mTitle.c_str());
		mTitleC = new char[len + 1];
		mTitleC[len] = '\0';
		wcstombs(mTitleC, mTitle.c_str(), len);

		TTF_SizeText(mFont, mTitleC, &mTextSize.x, &mTextSize.y);

	}

	void UI::PlaylistItem::SetColor(glm::vec3 color)
	{
		mTextColor = { u8(color.x * 255.f), u8(color.y * 255.f), u8(color.z * 255.f) };
	}

	std::wstring UI::PlaylistItem::GetTitle()
	{
		std::wstring title = Playlist::GetTitle(mID);
		s16 len = wcslen(title.c_str());
		f32 textSize = mTextSize.x * mTextScale;
		if (textSize > this->mSize.x)
		{
			float charSize = textSize / (float)len;
			u16 i = 0;
			u16 pos = 0;
			while (i * charSize < this->mSize.x)
				i++;

			title = title.substr(0, i - 1);
			title += L"...";

			len = wcslen(title.c_str());
			mTitleC = new char[len + 1];
			mTitleC[len] = '\0';
			wcstombs(mTitleC, title.c_str(), len);
			TTF_SizeText(mFont, mTitleC, &mTextSize.x, &mTextSize.y);
		}
		

		return title;
	}

	b8 UI::PlaylistItem::IsPlaying()
	{
		return MP::Playlist::RamLoadedMusic.mID == mID;
	}

	s32 UI::PlaylistItem::mOffsetY = 0;
	s32 UI::PlaylistItem::mCount = 0;
	s32 UI::PlaylistItem::mOffsetIndex = 0;

	namespace UI
	{
		void DebugStart()
		{
#ifdef _DEBUG_
			mdDebugTex = mdLoadTexture("assets/debug.png");
#endif
		}

		void DebugRender()
		{
#ifdef _DEBUG_
			ImGui::Begin("_DEBUG_");
			if (ImGui::TreeNode("Player") == true)
			{
				ImGui::Text(Playlist::GetPositionInString().c_str());
				ImGui::SameLine();
				ImGui::Text("     Song ID: %d", Playlist::RamLoadedMusic.mID);
				ImGui::SameLine();
				ImGui::Text("     Volume: %d", (int)(Playlist::GetVolume() * 100));;

				music_position = Playlist::GetPosition();
				if (ImGui::SliderInt("Pos", &music_position, 0.0f, Playlist::GetMusicLength()))
				{
					Playlist::SetPosition(music_position);
				}

				ImGui::TextColored(ImVec4(0.3, 0.3, 0.3, 1.0), "Prev: %s", Playlist::GetTitle(Playlist::GetPreviousID()).c_str());
				ImGui::TextColored(ImVec4(0.7, 0.0, 0.0, 1.0), "Curr: %s", Playlist::GetTitle(Playlist::RamLoadedMusic.mID).c_str());;
				ImGui::TextColored(ImVec4(0.3, 0.3, 0.3, 1.0), "Next: %s", Playlist::GetTitle(Playlist::GetNextID()).c_str());
				ImGui::NewLine();

				if(ImGui::Checkbox("Repeat", &music_repeat))
					Playlist::RepeatMusic();
				ImGui::SameLine();
				if (ImGui::Checkbox("Shuffle", &music_shuffle))
					Playlist::ShuffleMusic();

				if (ImGui::Button("Play") == true)
				{
					Playlist::PlayMusic();
				}
				ImGui::SameLine();
				if (ImGui::Button("Pause") == true)
				{
					Playlist::PauseMusic();
				}
				ImGui::SameLine();
				if (ImGui::Button("Stop") == true)
				{
					Playlist::StopMusic();
				}
				ImGui::SameLine();
				if (ImGui::Button("Previous") == true)
				{
					Playlist::PreviousMusic();
				}
				ImGui::SameLine();
				if (ImGui::Button("Next") == true)
				{
					Playlist::NextMusic();
				}


				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Advanced") == true)
			{
				ImGui::InputInt("Scroll volume step", &volume_scroll_step, 1, 1);
				Playlist::SetScrollVolumeStep(volume_scroll_step);

				ImGui::InputInt("Volume fade time", &volume_fade_time, 20, 100);
				Playlist::SetVolumeFadeTime(volume_fade_time);



				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Other") == true)
			{
				ImGui::Text("Songs loaded: %d", Playlist::mdPathContainer.size());

				ImGui::Text("Current shuffle pos: %d", Playlist::GetCurrentShufflePos());

				ImGui::Text("Shuffle container size: %d", Playlist::GetShuffleContainerSize());

				if (ImGui::Button("Print Shuffled Pos") == true)
				{
					Playlist::PrintShuffledPositions();
				}

				if (ImGui::Button("Print Loaded Paths") == true)
				{
					Playlist::PrintLoadedPaths();
				}

				ImGui::TreePop();
			}

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();

			glm::mat4 model;
			Shader::shaderDefault->use();

			if (App::Input::IsKeyPressed(App::KeyCode::Tab))
			{
				renderDebug = !renderDebug;
			}

			if (renderDebug)
			{
				// TODO: shits broken
				//glViewport(0, mdCurrentHeight, mdCurrentWidth, mdCurrentHeight);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, mdDebugTex);
				Shader::shaderDefault->setVec3("color", buttonColor);
				for (u16 i = 0; i < mdButtonsContainer.size(); i++)
				{
					model = glm::mat4();
					model = glm::translate(model, glm::vec3(mdButtonsContainer[i].second->mPos, 1.f));
					model = glm::scale(model, glm::vec3(mdButtonsContainer[i].second->mSize, 1.f));;
					Shader::shaderDefault->setMat4("model", model);
					Shader::Draw();
				}
				for (u16 i = 0; i < mdMovableContainer.size(); i++)
				{
					model = glm::mat4();
					model = glm::translate(model, glm::vec3(mdMovableContainer[i]->mPos, 1.f));
					model = glm::scale(model, glm::vec3(mdMovableContainer[i]->mSize, 1.f));;
					Shader::shaderDefault->setMat4("model", model);
					Shader::Draw();
				}

				for (u16 i = 0; i < mdResizableContainer.size(); i++)
				{
					model = glm::mat4();
					model = glm::translate(model, glm::vec3(mdResizableContainer[i]->pos, 1.f));
					model = glm::scale(model, glm::vec3(mdResizableContainer[i]->size, 1.f));;
					Shader::shaderDefault->setMat4("model", model);
					Shader::Draw();
				}
				glm::vec3 white(1.f);
				Shader::shaderDefault->setVec3("color", white);
			}



#endif
		}

		void Data::InitializeData()
		{
			s16 musicUIOffsetX = -60;
			s16 musicUIOffsetY = 35;
			s16 musicProgressBarOffsetY = 10;

			_MUSIC_PLAYER_FONT = TTF_OpenFont("assets/font/Raleway-Regular.ttf", 14);


			_MIN_PLAYER_SIZE = glm::vec2(500.f, 500.f);

			_DEFAULT_PLAYER_POS = glm::vec2(0.f, 0.f);
			_DEFAULT_PLAYER_SIZE = glm::vec2(500.f, 350.f);

			_DEFAULT_WINDOW_POS = glm::vec2(0.f, 0.f);
			_DEFAULT_WINDOW_SIZE = glm::vec2(mdDefaultWidth, 700.f);

			_MAIN_BACKGROUND_POS = glm::vec2(0.f, 0.f);
			_MAIN_BACKGROUND_SIZE = glm::vec2(mdDefaultWidth, mdDefaultHeight);

			_MAIN_FOREGROUND_POS = glm::vec2(20.0f, 20.0f);
			_MAIN_FOREGROUND_SIZE = glm::vec2(mdDefaultWidth - 40.f, mdDefaultHeight - 75.f);

			_PLAYLIST_FOREGROUND_POS = glm::vec2(20.f, mdDefaultHeight - 20.f);
			_PLAYLIST_FOREGROUND_SIZE = glm::vec2(mdDefaultWidth - 40.f, mdDefaultHeight);

			_EXIT_BUTTON_POS = glm::vec2(mdDefaultWidth - 50.f, 5.f);
			_EXIT_BUTTON_SIZE = glm::vec2(15.f, 15.f);

			
			/* Initialize later */

			_VOLUME_BAR_POS = glm::vec2(mdDefaultWidth / 2.f - 170.f, mdDefaultHeight - 60.f - musicUIOffsetY);
			_VOLUME_BAR_SIZE = glm::vec2(90.f, 4.f);

			_VOLUME_SPEAKER_POS = glm::vec2(mdDefaultWidth / 2.f - 200.f, mdDefaultHeight - 66.f - musicUIOffsetY);
			_VOLUME_SPEAKER_SIZE = glm::vec2(15.f, 15.f);

			_MUSIC_PROGRESS_BAR_POS = glm::vec2(mdDefaultWidth / 2.f - 175.f, mdDefaultHeight - 35.f - musicProgressBarOffsetY);
			_MUSIC_PROGRESS_BAR_SIZE = glm::vec2(mdDefaultWidth / 2.f + 100.f, 5.f);


			_MUSIC_PROGRESS_BAR_DOT_POS = glm::vec2(mdDefaultWidth / 2.f - 175.f, mdDefaultHeight - 37.f - musicProgressBarOffsetY);
			_VOLUME_BAR_DOT_POS = glm::vec2(mdDefaultWidth / 2.f - 130.f, mdDefaultHeight - 63.f - musicUIOffsetY);
			_SLIDER_DOT_SIZE = glm::vec2(9.f, 9.f);

			_UI_WINDOW_BAR_POS;
			_UI_WINDOW_BAR_SIZE;

			_MINIMIZE_BUTTON_POS;
			_MINIMIZE_BUTTON_SIZE;

			_STAY_ON_TOP_BUTTON_POS;
			_STAY_ON_TOP_BUTTON_SIZE;	

			_SHUFFLE_BUTTON_POS = glm::vec2(mdDefaultWidth / 2.f - 110.f - musicUIOffsetX, mdDefaultHeight - 65.f - musicUIOffsetY);
			_SHUFFLE_BUTTON_SIZE = glm::vec2(20.f, 12.f);

			_PREVIOUS_BUTTON_POS = glm::vec2(mdDefaultWidth / 2.f - 60.f - musicUIOffsetX, mdDefaultHeight - 67.f - musicUIOffsetY);;
			_PREVIOUS_BUTTON_SIZE = glm::vec2(15.f, 15.f);

			_PLAY_BUTTON_POS = glm::vec2(mdDefaultWidth / 2.f - 20.f - musicUIOffsetX, mdDefaultHeight - 80.f - musicUIOffsetY);
			_PLAY_BUTTON_SIZE = glm::vec2(40.f, 40.f);

			_NEXT_BUTTON_POS = glm::vec2(mdDefaultWidth / 2.f + 40.f - musicUIOffsetX, mdDefaultHeight - 67.f - musicUIOffsetY);
			_NEXT_BUTTON_SIZE = glm::vec2(15.f, 15.f);

			_REPEAT_BUTTON_POS = glm::vec2(mdDefaultWidth / 2.f + 90.f - musicUIOffsetX, mdDefaultHeight - 65.f - musicUIOffsetY);;
			_REPEAT_BUTTON_SIZE = glm::vec2(20.f, 12.f);

			_DOT_BUTTON_STATE_SIZE = glm::vec2(5.f);

			_PLAYLIST_BUTTON_POS = glm::vec2(mdDefaultWidth - 40.f, mdDefaultHeight - 40.f - musicUIOffsetY);
			_PLAYLIST_BUTTON_SIZE = glm::vec2(15.f, 15.f);

			_PLAYLIST_ITEMS_SURFACE_POS = glm::vec2(mdCurrentWidth / 2.f - 150.f, mdCurrentHeight - (mdCurrentHeight - 350.f));
			_PLAYLIST_ITEMS_SURFACE_SIZE = glm::vec2(mdCurrentWidth - 400.f, mdCurrentHeight - 100.f);

			_PLAYLIST_ITEM_SIZE = glm::vec2(300.f, 30.f);
		}

		void Data::UpdateData()
		{
			// keep width as default for now
			mdCurrentWidth = mdEngine::Window::windowProperties.mWindowWidth;
			mdCurrentHeight = mdEngine::Window::windowProperties.mApplicationHeight;
			
			//std::cout << mdCurrentHeight << std::endl;
			mdDefaultWidth = 500.f;
			mdDefaultHeight = Window::windowProperties.mWindowHeight - 350.f;


			_PLAYLIST_ITEMS_SURFACE_POS = glm::vec2(mdCurrentWidth / 2.f - 150.f, mdCurrentHeight - (mdCurrentHeight - 350.f));
			_PLAYLIST_ITEMS_SURFACE_SIZE = glm::vec2(mdCurrentWidth - 400.f, mdCurrentHeight - 60.f);


		}

		void Start()
		{
			std::string locale = setlocale(LC_ALL, "");//this function sets applications locale to current system locale
			//std::cout << "default locale:" << locale;

			mdCurrentWidth = mdEngine::Window::windowProperties.mWindowWidth;
			mdCurrentHeight = mdEngine::Window::windowProperties.mApplicationHeight;

			mdDefaultWidth = 500.f;
			mdDefaultHeight = 350.f;;

			Data::InitializeData();


			new Movable(glm::vec2(mdDefaultWidth, 20), glm::vec2(0.f, 0.f));

			new Resizable(glm::vec2(mdCurrentWidth, 20), glm::vec2(0, mdCurrentHeight - 20.f));

			// Make slightly bigger hitbox for sliders
			f32 offsetX = 1.01f;
			f32 offsetY = 4.f;
			new Button(Input::ButtonType::SliderVolume, glm::vec2(Data::_VOLUME_BAR_SIZE.x * offsetX, Data::_VOLUME_BAR_SIZE.y * offsetY),
														glm::vec2(Data::_VOLUME_BAR_POS.x - (Data::_VOLUME_BAR_SIZE.x * offsetX - Data::_VOLUME_BAR_SIZE.x) / 2.f,
																  Data::_VOLUME_BAR_POS.y - (Data::_VOLUME_BAR_SIZE.y * offsetY - Data::_VOLUME_BAR_SIZE.y) / 2.f));

			new Button(Input::ButtonType::SliderMusic, glm::vec2(Data::_MUSIC_PROGRESS_BAR_SIZE.x * offsetX, Data::_MUSIC_PROGRESS_BAR_SIZE.y * offsetY),
													   glm::vec2(Data::_MUSIC_PROGRESS_BAR_POS.x - (Data::_MUSIC_PROGRESS_BAR_SIZE.x * offsetX - Data::_MUSIC_PROGRESS_BAR_SIZE.x) / 2.f,
																 Data::_MUSIC_PROGRESS_BAR_POS.y - (Data::_MUSIC_PROGRESS_BAR_SIZE.y * offsetY - Data::_MUSIC_PROGRESS_BAR_SIZE.y) / 2.f));


			new Button(Input::ButtonType::Exit, Data::_EXIT_BUTTON_SIZE, Data::_EXIT_BUTTON_POS);

			new Button(Input::ButtonType::Volume, Data::_VOLUME_SPEAKER_SIZE, Data::_VOLUME_SPEAKER_POS);

			new Button(Input::ButtonType::Shuffle, Data::_SHUFFLE_BUTTON_SIZE, Data::_SHUFFLE_BUTTON_POS);

			new Button(Input::ButtonType::Previous, Data::_PREVIOUS_BUTTON_SIZE, Data::_PREVIOUS_BUTTON_POS);

			new Button(Input::ButtonType::Play, Data::_PLAY_BUTTON_SIZE, Data::_PLAY_BUTTON_POS);

			new Button(Input::ButtonType::Stop, Data::_PLAY_BUTTON_SIZE, Data::_PLAY_BUTTON_POS);

			new Button(Input::ButtonType::Next, Data::_NEXT_BUTTON_SIZE, Data::_NEXT_BUTTON_POS);

			new Button(Input::ButtonType::Repeat, Data::_REPEAT_BUTTON_SIZE, Data::_REPEAT_BUTTON_POS);

			new Button(Input::ButtonType::Playlist, Data::_PLAYLIST_BUTTON_SIZE, Data::_PLAYLIST_BUTTON_POS);


			/*PlaylistItem * item = NULL;
			for (int i = 0; i < 100; i++)
			{
				item = new PlaylistItem();
				item->InitFont();
				item->InitItem();
			}*/



			DebugStart();
		}

		void Update()
		{

			/* Collect user input for buttons and movable */
			for(u16 i = 0; i < mdMovableContainer.size(); i++)
				App::ProcessMovable(mdMovableContainer[i]);

			for (u16 i = 0; i < mdResizableContainer.size(); i++)
				App::ProcessResizable(mdResizableContainer[i]);

			for (u16 i = 0; i < mdButtonsContainer.size(); i++)
				App::ProcessButton(mdButtonsContainer[i].second);

			HandleInput();

			Data::UpdateData();
		}

		void Render()
		{

			DebugRender();


		}

		/* Find Exit button and check if is pressed */
		void HandleInput()
		{
			if (Input::isButtonPressed(Input::ButtonType::Exit))
			{
				mdEngine::AppExit();
			}

			if (Input::isButtonPressed(Input::ButtonType::Shuffle))
			{
				Playlist::ShuffleMusic();
			}

			if (Input::isButtonPressed(Input::ButtonType::Previous))
			{
				Playlist::PreviousMusic();
			}

			if (Input::isButtonPressed(Input::ButtonType::Play) && Playlist::IsPlaying() == false)
			{
				Playlist::PlayMusic();
			}
			else if(Input::isButtonPressed(Input::ButtonType::Play))
			{
				Playlist::PauseMusic();
			}

			if (Input::isButtonPressed(Input::ButtonType::Next))
			{
				Playlist::NextMusic();
			}

			if (Input::isButtonPressed(Input::ButtonType::Repeat))
			{
				Playlist::RepeatMusic();
			}



			for (s32 i = 0; i < mdItemContainer.size(); i++)
			{
				if (mdItemContainer[i]->hasFocus == true)
				{
					//std::wcout << mdItemContainer[i + PlaylistItem::mOffsetIndex]->GetTitle() << std::endl;
				}
			}



		}
	}

}
}