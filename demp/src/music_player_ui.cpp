#include "music_player_ui.h"

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
#include "graphics.h"
#include "music_player_string.h"
#include "music_player.h"
#include "music_player_graphics.h"
#include "music_player_settings.h"


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
		std::vector<std::pair<Input::ButtonType, Button*>> mdPlaylistButtonsContainer;

#ifdef _DEBUG_
		b8 renderDebug = false;
		glm::vec3 borderColor(0.f, 1.f, 0.f);
#endif
		
		b8 music_repeat = false;
		b8 music_shuffle = false;
		s32 music_position = 0;
		Time::Timer clickDelayTimer;
		
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

	}

	UI::Movable::Movable(glm::vec2 size, glm::vec2 pos) : mSize(size), mPos(pos)
	{ 
		mdMovableContainer.push_back(this);
	}

	UI::Resizable::Resizable(glm::vec2 size, glm::vec2 pos) : size(size), pos(pos) 
	{ 
		mdResizableContainer.push_back(this);
	}

	UI::Button::Button() { }

	UI::Button::Button(Input::ButtonType type, glm::vec2 size, glm::vec2 pos) : mSize(size), mPos(pos)
	{
		mdButtonsContainer.push_back(std::make_pair(type, this));
	}

	UI::PlaylistItem::~PlaylistItem()
	{
		
	}

	void UI::PlaylistItem::InitFont()
	{
		mFont = MP::Data::_MUSIC_PLAYER_FONT;
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

		u16 len = mTitle.length();
		mTitleC.resize(len + 1);
		mTitleC = utf16_to_utf8(mTitle);

		TTF_SizeText(mFont, mTitleC.c_str(), &mTextSize.x, &mTextSize.y);

		mdItemContainer.push_back(this);
		mdPlaylistButtonsContainer.push_back(std::make_pair(Input::ButtonType::None, this));

		mCount++;
	}

	void UI::PlaylistItem::UpdateItem()
	{
		/*mTitle = Playlist::GetTitle(mID);
		u16 len = wcslen(mTitle.c_str());
		mTitleC = new char[len + 1];
		mTitleC[len] = '\0';
		wcstombs(mTitleC, mTitle.c_str(), len);

		TTF_SizeText(mFont, mTitleC, &mTextSize.x, &mTextSize.y);*/

	}

	void UI::PlaylistItem::SetColor(glm::vec3 color)
	{
		mTextColor = { u8(color.x * 255.f), u8(color.y * 255.f), u8(color.z * 255.f) };
	}

	void UI::PlaylistItem::DrawDottedBorder(s16 playPos)
	{
		if (this->mPos == glm::vec2(INVALID))
			return;

		glm::mat4 model;
		Shader::shaderBorder->use();
		Shader::shaderBorder->setVec3("color", Color::Grey);
		f32 dotXOffset = 0.01;
		f32 dotYOffset = 0.1;
		Shader::shaderBorder->setFloat("xOffset", dotXOffset);
		Shader::shaderBorder->setFloat("yOffset", dotYOffset);
		model = glm::translate(model, glm::vec3(this->mPos, 0.9));
		model = glm::scale(model, glm::vec3(this->mSize, 1.f));
		Shader::shaderBorder->setMat4("model", model);
		Shader::DrawDot();
	}
	
	std::wstring UI::PlaylistItem::GetTitle()
	{
		s16 len = wcslen(mTitle.c_str());
		f32 textSize = mTextSize.x * mTextScale;

		if (textSize > this->mSize.x)
		{
			float charSize = textSize / (float)len;
			u16 i = 0;
			u16 pos = 0;
			while (i * charSize < this->mSize.x)
				i++;

			mTitle = mTitle.substr(0, i - 4);
			mTitle += L"...";

			len = mTitle.length();
			mTitleC.resize(len + 1);
			mTitleC = utf16_to_utf8(mTitle);
			TTF_SizeText(mFont, mTitleC.c_str(), &mTextSize.x, &mTextSize.y);;
		}
		

		return mTitle;
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

				if (ImGui::Button("Print selected IDs") == true)
				{
					for (u16 i = 0; i < Graphics::MP::playlist.multipleSelect.size(); i++)
					{
						std::cout << *Graphics::MP::playlist.multipleSelect[i] << std::endl;
					}
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
				glBindTexture(GL_TEXTURE_2D, 0);
				Shader::shaderDefault->setVec3("color", borderColor);
				Shader::shaderDefault->setBool("border", true);
				Shader::shaderDefault->setFloat("border_width", 0.08);

				for (u16 i = 0; i < mdButtonsContainer.size(); i++)
				{
					model = glm::mat4();
					model = glm::translate(model, glm::vec3(mdButtonsContainer[i].second->mPos, 1.f));
					model = glm::scale(model, glm::vec3(mdButtonsContainer[i].second->mSize, 1.f));;
					Shader::shaderDefault->setFloat("aspectXY", mdButtonsContainer[i].second->mSize.x / mdButtonsContainer[i].second->mSize.y	);
					Shader::shaderDefault->setMat4("model", model);
					Shader::Draw();
				}
				for (u16 i = 0; i < mdMovableContainer.size(); i++)
				{
					model = glm::mat4();
					model = glm::translate(model, glm::vec3(mdMovableContainer[i]->mPos, 1.f));
					model = glm::scale(model, glm::vec3(mdMovableContainer[i]->mSize, 1.f));;
					Shader::shaderDefault->setFloat("aspectXY", mdMovableContainer[i]->mSize.x / mdMovableContainer[i]->mSize.y);
					Shader::shaderDefault->setMat4("model", model);
					Shader::Draw();
				}

				for (u16 i = 0; i < mdResizableContainer.size(); i++)
				{
					model = glm::mat4();
					model = glm::translate(model, glm::vec3(mdResizableContainer[i]->pos, 1.f));
					model = glm::scale(model, glm::vec3(mdResizableContainer[i]->size, 1.f));;
					Shader::shaderDefault->setFloat("aspectXY", mdResizableContainer[i]->size.x / mdResizableContainer[i]->size.y);
					Shader::shaderDefault->setMat4("model", model);
					Shader::Draw();
				}

				for (u16 i = 0; i < mdPlaylistButtonsContainer.size(); i++)
				{
					if (mdPlaylistButtonsContainer[i].second->mPos != glm::vec2(INVALID))
					{
						model = glm::mat4();
						model = glm::translate(model, glm::vec3(mdPlaylistButtonsContainer[i].second->mPos, 1.f));
						model = glm::scale(model, glm::vec3(mdPlaylistButtonsContainer[i].second->mSize, 1.f));;
						Shader::shaderDefault->setFloat("aspectXY", mdPlaylistButtonsContainer[i].second->mSize.x / mdPlaylistButtonsContainer[i].second->mSize.y);
						Shader::shaderDefault->setMat4("model", model);
						Shader::Draw();;
					}
				}
				glm::vec3 white(1.f);
				Shader::shaderDefault->setVec3("color", white);
				Shader::shaderDefault->setBool("border", false);
			}



#endif
		}

		void Start()
		{
			//std::string locale = setlocale(LC_ALL, "");
			//std::cout << "default locale:" << locale;

			mdCurrentWidth = mdEngine::Window::windowProperties.mWindowWidth;
			mdCurrentHeight = mdEngine::Window::windowProperties.mApplicationHeight;

			mdDefaultWidth = 500.f;
			mdDefaultHeight = 350.f;

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

			new Button(Input::ButtonType::SliderPlaylist, Data::_PLAYLIST_SCROLL_BAR_SIZE, Data::_PLAYLIST_SCROLL_BAR_POS);

			clickDelayTimer = Time::Timer(Data::_PLAYLIST_CHOOSE_ITEM_DELAY);


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

			for (u16 i = 0; i < mdPlaylistButtonsContainer.size(); i++)
				App::ProcessButton(mdPlaylistButtonsContainer[i].second);


			HandleInput();
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
			else if (Input::isButtonPressed(Input::ButtonType::Play))
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
				std::vector<s32*>::iterator it;
				if (App::Input::IsKeyDown(App::KeyCode::LShift) && App::Input::IsKeyDown(App::KeyCode::A))
				{
					it = std::find(Graphics::MP::playlist.multipleSelect.begin(),
								   Graphics::MP::playlist.multipleSelect.end(),
								   &mdItemContainer[i]->mID);

					if(it == Graphics::MP::playlist.multipleSelect.end())
						Graphics::MP::playlist.multipleSelect.push_back(&mdItemContainer[i]->mID);
				}
				else if (mdItemContainer[i]->isPressed == true)
				{
					std::wstring x = mdItemContainer[i]->GetTitle();

					clickDelayTimer.start();
					mdItemContainer[i]->clickCount++;

					// Check if current's item position is in vector with selected item's positions
					it = std::find(Graphics::MP::playlist.multipleSelect.begin(),
										Graphics::MP::playlist.multipleSelect.end(),
										&mdItemContainer[i]->mID);
					// WHY LCTRL IS NOT WORKING??
					if (App::Input::IsKeyDown(App::KeyCode::LShift))
					{
						if(it == Graphics::MP::playlist.multipleSelect.end())
							Graphics::MP::playlist.multipleSelect.push_back(&mdItemContainer[i]->mID);
						else
						{
							Graphics::MP::playlist.multipleSelect.erase(it);
						}
					}
					else
					{
						if (Graphics::MP::playlist.multipleSelect.size() < 1)
						{
							Graphics::MP::playlist.multipleSelect.push_back(&mdItemContainer[i]->mID);
						}
						else
						{
							Graphics::MP::playlist.multipleSelect.clear();
							Graphics::MP::playlist.multipleSelect.push_back(&mdItemContainer[i]->mID);
						}
					}

					if (mdItemContainer[i]->clickCount > 1)
					{
						MP::musicPlayerState = MP::MusicPlayerState::kMusicChosen;
						Playlist::RamLoadedMusic.load(Playlist::mdPathContainer[mdItemContainer[i]->mID], mdItemContainer[i]->mID);
						Playlist::PlayMusic();
					}

				}

				//If time for second click expires, reset click count
				if (clickDelayTimer.finished == true)
					mdItemContainer[i]->clickCount = 0;

			}

			// If delete pressed, delete every item on position from selected positions vector
			if (App::Input::IsKeyPressed(App::KeyCode::Delete))
			{
				for (u16 j = 0; j < Graphics::MP::playlist.multipleSelect.size(); j++)
				{
					Playlist::DeleteMusic(*Graphics::MP::playlist.multipleSelect[j]);

				}
				Graphics::MP::playlist.multipleSelect.clear();
			}


			// Update second click timer
			clickDelayTimer.update();
		}
	}

}
}