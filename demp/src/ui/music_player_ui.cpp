#include "music_player_ui.h"

#include <vector>
#include <locale.h>
#include <thread>
#include <future>
#include <chrono>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#ifdef _DEBUG_
#include "../utility/md_load_texture.h"
#include "../utility/md_shape.h"
#endif

#include "../../external/imgui/imgui.h"

#include "../app/realtime_system_application.h"
#include "../audio/mp_audio.h"
#include "../sqlite/md_sqlite.h"
#include "../settings/music_player_string.h"
#include "../settings/music_player_settings.h"
#include "../graphics/graphics.h"
#include "../graphics/music_player_graphics.h"
#include "../graphics/music_player_graphics_playlist.h"
#include "../playlist/music_player_playlist.h"
#include "../player/music_player.h"
#include "../player/music_player_system.h"
#include "../player/music_player_state.h"
#include "../utility/md_windows.h"


using namespace mdEngine::Graphics;
using namespace std::chrono_literals;

namespace mdEngine
{
namespace MP
{
	namespace UI
	{
		MovableContainer		mdMovableContainer;
		Interface::Resizable	mdResizableBottom;
		Interface::Resizable	mdResizableTop;
		ButtonContainer			mdButtonsContainer;
		Window::OptionsWindow	mdOptionsWindow;
		SDL_Cursor*				mdCursor;

		std::atomic<bool> fileBrowserFinished(false);
		
#ifdef _DEBUG_
		b8 renderDebug = false;
		glm::vec3 borderColor(0.f, 1.f, 0.f);
#endif
		
		b8 music_repeat = false;
		b8 music_shuffle = false;
		s32 music_position = 0;
		Time::Timer clickDelayTimer;

		f32 mdDefaultWidth;
		f32 mdDefaultHeight;
		f32 mdCurrentWidth;
		f32 mdCurrentHeight;
		f32 mdProjectionHeight;


		s32 hiddenSeparatorsCount;
		b8 folderItemsHidden(false);
		b8 itemsHidden(false);
		b8 selectAllItemsInFolder(false);
		b8 playlistItemSelected(false);


		b8 fileBrowserActive(false);

		std::string title = "none";

		ImVec4 ClearColor = ImVec4(1.f, 254.f/255.f, 1.f, 1.f);

		std::locale l("");

		void HandleInput();

		void UpdateMouseCursor();

		void HandlePlaylistInput();

		void HandleSeparatorInput();

		void PlaylistFileExplorer();

		void UpdateOptionsWindow();

		void DebugStart();

		void DebugRender();

		void OpenFileBrowserWrap();

		void OpenFolderBrowserWrap();

	}

	void UI::Start()
	{
		//std::string locale = setlocale(LC_ALL, "");
		//std::cout << "default locale:" << locale;

		mdCurrentWidth = mdEngine::Window::windowProperties.mWindowWidth;
		mdCurrentHeight = mdEngine::Window::windowProperties.mApplicationHeight;

		mdDefaultWidth = 500.f;
		mdDefaultHeight = 350.f;

		new Interface::Movable(glm::vec2(mdDefaultWidth, 20), glm::vec2(0.f, 5.f));


		mdResizableTop		= Interface::Resizable(glm::vec2(mdCurrentWidth, 5.f), glm::vec2(0, 0.f));

		mdResizableBottom	= Interface::Resizable(glm::vec2(mdCurrentWidth, 5), glm::vec2(0, mdCurrentHeight - 5.f));


		// Make slightly bigger hitbox for sliders
		f32 offsetX = 1.01f;
		f32 offsetY = 4.f;
		new Interface::Button(Input::ButtonType::SliderVolume, glm::vec2(Data::_VOLUME_BAR_SIZE.x * offsetX, Data::_VOLUME_BAR_SIZE.y * offsetY),
													glm::vec2(Data::_VOLUME_BAR_POS.x - (Data::_VOLUME_BAR_SIZE.x * offsetX - Data::_VOLUME_BAR_SIZE.x) / 2.f,
															  Data::_VOLUME_BAR_POS.y - (Data::_VOLUME_BAR_SIZE.y * offsetY - Data::_VOLUME_BAR_SIZE.y) / 2.f));

		new Interface::Button(Input::ButtonType::SliderMusic, glm::vec2(Data::_MUSIC_PROGRESS_BAR_SIZE.x * offsetX, Data::_MUSIC_PROGRESS_BAR_SIZE.y * offsetY),
												   glm::vec2(Data::_MUSIC_PROGRESS_BAR_POS.x - (Data::_MUSIC_PROGRESS_BAR_SIZE.x * offsetX - Data::_MUSIC_PROGRESS_BAR_SIZE.x) / 2.f,
															 Data::_MUSIC_PROGRESS_BAR_POS.y - (Data::_MUSIC_PROGRESS_BAR_SIZE.y * offsetY - Data::_MUSIC_PROGRESS_BAR_SIZE.y) / 2.f));



		new Interface::Button(Input::ButtonType::Exit, Data::_EXIT_BUTTON_SIZE, Data::_EXIT_BUTTON_POS);

		new Interface::Button(Input::ButtonType::Minimize, Data::_MINIMIZE_BUTTON_SIZE, Data::_MINIMIZE_BUTTON_POS);

		new Interface::Button(Input::ButtonType::Options, Data::_SETTINGS_BUTTON_SIZE, Data::_SETTINGS_BUTTON_POS);

		new Interface::Button(Input::ButtonType::Volume, Data::_VOLUME_SPEAKER_SIZE, Data::_VOLUME_SPEAKER_POS);

		new Interface::Button(Input::ButtonType::Shuffle, Data::_SHUFFLE_BUTTON_SIZE, Data::_SHUFFLE_BUTTON_POS);

		new Interface::Button(Input::ButtonType::Previous, Data::_PREVIOUS_BUTTON_SIZE, Data::_PREVIOUS_BUTTON_POS);

		new Interface::Button(Input::ButtonType::Play, Data::_PLAY_BUTTON_SIZE, Data::_PLAY_BUTTON_POS);

		new Interface::Button(Input::ButtonType::Stop, Data::_PLAY_BUTTON_SIZE, Data::_PLAY_BUTTON_POS);

		new Interface::Button(Input::ButtonType::Next, Data::_NEXT_BUTTON_SIZE, Data::_NEXT_BUTTON_POS);

		new Interface::Button(Input::ButtonType::Repeat, Data::_REPEAT_BUTTON_SIZE, Data::_REPEAT_BUTTON_POS);

		new Interface::Button(Input::ButtonType::Playlist, Data::_PLAYLIST_BUTTON_SIZE, Data::_PLAYLIST_BUTTON_POS);

		new Interface::Button(Input::ButtonType::SliderPlaylist, Data::_PLAYLIST_SCROLL_BAR_SIZE, Data::_PLAYLIST_SCROLL_BAR_POS);

		new Interface::Button(Input::ButtonType::PlaylistAddFile, Data::_PLAYLIST_ADD_BUTTON_SIZE, Data::_PLAYLIST_ADD_BUTTON_POS);
		// temporary
		//new Interface::Button(Input::ButtonType::PlaylistAddFolder, Data::_PLAYLIST_ADD_BUTTON_SIZE, Data::_PLAYLIST_ADD_BUTTON_POS);

		clickDelayTimer = Time::Timer(Data::_PLAYLIST_CHOOSE_ITEM_DELAY);

		DebugStart();
	}

	void UI::Update()
	{
		/* Collect user input for buttons and movable */
		for(u16 i = 0; i < mdMovableContainer.size(); i++)
			App::ProcessMovable(mdMovableContainer[i]);

		App::ProcessResizableTop(&mdResizableTop, &mdResizableBottom);

		App::ProcessResizableBottom(&mdResizableBottom, &mdResizableTop);

		UpdateMouseCursor();

		for (u16 i = 0; i < mdButtonsContainer.size(); i++)
			App::ProcessButton(mdButtonsContainer[i].second);

		// Make sure that hitboxes that are not visible cannot be clicked
		App::SetButtonCheckBounds(Data::_PLAYLIST_ITEMS_SURFACE_POS.y, Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y, true);
		for (auto i : Graphics::MP::GetPlaylistObject()->GetIndexesToRender())
		{
			if (Interface::PlaylistButton::GetButton(i) == nullptr || State::CheckState(State::Window::Resized) == true)
				break;

			if(Audio::Object::GetAudioObject(i)->IsPlaylistItemHidden() == false)
				App::ProcessButton(Interface::PlaylistButton::GetButton(i));

		}
		App::SetButtonCheckBounds(Data::_PLAYLIST_ITEMS_SURFACE_POS.y, Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y, false);

		// Process playlist separator buttons
		App::SetButtonCheckBounds(Data::_PLAYLIST_ITEMS_SURFACE_POS.y, Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y, true);
		auto sepCon = Interface::Separator::GetContainer();
		for (u16 i = 0; i < Interface::Separator::GetSize(); i++)
		{
			App::ProcessButton(sepCon->at(i).second);
		}
		App::SetButtonCheckBounds(Data::_PLAYLIST_ITEMS_SURFACE_POS.y, Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y, false);

		// Proces interface buttons
		for (u16 i = 0; i < Interface::m_InterfaceButtonContainer.size(); i++)
		{
			assert(Interface::m_InterfaceButtonContainer[i].second != nullptr);

			App::ProcessButton(Interface::m_InterfaceButtonContainer[i].second);
		}

		HandleInput();
		if (State::CheckState(State::PlaylistEmpty) == false)
		{
			HandlePlaylistInput();
			HandleSeparatorInput();
			clickDelayTimer.Update();
		}

		PlaylistFileExplorer();
	}

	void UI::Render()
	{
		DebugRender();
	}

	void UI::Close()
	{
		for (size_t i = 0; i < mdMovableContainer.size(); i++)
		{
			delete mdMovableContainer[i];
			mdMovableContainer[i] = nullptr;
		}
		mdMovableContainer.clear();

		
		for (size_t i = 0; i < mdButtonsContainer.size(); i++)
		{
			delete mdButtonsContainer[i].second;
			mdButtonsContainer[i].second = nullptr;
		}
		mdButtonsContainer.clear();

		for (size_t i = 0; i < Interface::m_InterfaceButtonContainer.size(); i++)
		{
			delete Interface::m_InterfaceButtonContainer[i].second;
			Interface::m_InterfaceButtonContainer[i].second = nullptr;
		}
		Interface::m_InterfaceButtonContainer.clear();

	}


	void UI::DebugStart()
	{
#ifdef _DEBUG_
		
#endif
	}

	void UI::DeleteAllFiles()
	{
		s32 len = Audio::Object::GetSize();
		while(Audio::Object::GetSize() > 0)
		{

			Playlist::DeleteMusic(Audio::Object::GetSize() - 1);
		}
		Graphics::MP::GetPlaylistObject()->multipleSelect.clear();
	}

	void UI::DebugRender()
	{
#ifdef _DEBUG_
		ImGui::Begin("_DEBUG_");
		if (ImGui::TreeNode("Player") == true)
		{
			ImGui::Text(Playlist::GetPositionInString().c_str());
			ImGui::SameLine();
			ImGui::Text("     Song ID: %d", Playlist::RamLoadedMusic.m_ID);
			ImGui::SameLine();
			ImGui::Text("     Volume: %d", (int)(Playlist::GetVolume() * 100));;

			music_position = Playlist::GetPosition();
			if (ImGui::SliderInt("Pos", &music_position, 0.0f, Playlist::GetMusicLength()))
			{
				Playlist::SetPosition(music_position);
			}

			ImGui::TextColored(ImVec4(0.3, 0.3, 0.3, 1.0), "Prev: %s", Playlist::GetTitle(Playlist::GetPreviousID()).c_str());
			ImGui::TextColored(ImVec4(0.7, 0.0, 0.0, 1.0), "Curr: %s", Playlist::GetTitle(Playlist::RamLoadedMusic.m_ID).c_str());;
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
			ImGui::InputInt("Scroll volume step", &Data::VolumeScrollStep, 1, 1);
			Playlist::SetScrollVolumeStep(Data::VolumeScrollStep);

			ImGui::InputInt("Volume fade time", &Data::PauseFadeTime, 20, 100);
			Playlist::SetVolumeFadeTime(Data::PauseFadeTime);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Other") == true)
		{
			if (ImGui::Button("Delete All") == true)
			{
				//std::thread t(UI::DeleteAll);
				//t.detach();

				UI::DeleteAllFiles();
			}

			ImGui::Text("Songs loaded: %d", Audio::Object::GetSize());

			ImGui::Text("Processed items count: %u", Audio::GetProccessedFileCount());

			/*ImGui::Text("Current shuffle pos: %d", Playlist::GetCurrentShufflePos());

			ImGui::Text("Shuffle container size: %d", Playlist::GetShuffleContainerSize());*/


			/*if (ImGui::Button("Print test") == true)
			{
				Audio::PrintTest();
			}*/
			
			if (ImGui::Button("Print Shuffled Pos") == true)
			{
				Playlist::PrintShuffledPositions();
			}

			/*if (ImGui::Button("Print Loaded Paths") == true)
			{
				Playlist::PrintLoadedPaths();
			}*/

			/*if (ImGui::Button("Print selected IDs") == true)
			{
				for (u16 i = 0; i < Graphics::MP::GetPlaylistObject()->multipleSelect.size(); i++)
				{
					std::cout << *Graphics::MP::GetPlaylistObject()->multipleSelect[i] << std::endl;
				}
			}*/

			

			/*f32 bitrate = 0.f;
			if (ImGui::Button("Print Bitrate") == true)
			{
				Playlist::GetBitrate(&bitrate);
				std::cout << bitrate << std::endl;
			}*/

			/*
			if (ImGui::Button("Print items container") == true)
			{
				std::thread t(Audio::GetItemsInfo);
				t.detach();
			}*/
			if (ImGui::Button("Space") == true)
			{
				for (s32 i = 0; i < 10; i++)
					std::cout << "*\n";
			}


			if (ImGui::Button("Print loaded folders") == true)
			{
				Audio::Folders::PrintContent();
			}

			if (ImGui::Button("Print folder sep and items") == true)
			{
				Interface::PrintSeparatorAndItsSubFiles();
			}

			if (ImGui::Button("Print separators info") == true)
			{
				Interface::Separator::PrintSeparatorInfo();
			}

			if (ImGui::Button("Print visible items info") == true)
			{
				Graphics::MP::PrintVisibleItemsInfo();
			}

			if (ImGui::Button("Print audio object info") == true)
			{
				Graphics::MP::PrintAudioObjectInfo();
			}

			if (ImGui::Button("Print indexes to render") == true)
			{
				Graphics::MP::PrintIndexesToRender();
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
				model = glm::translate(model, glm::vec3(mdButtonsContainer[i].second->GetButtonPos(), 1.f));
				model = glm::scale(model, glm::vec3(mdButtonsContainer[i].second->GetButtonSize(), 1.f));;
				Shader::shaderDefault->setFloat("aspectXY", mdButtonsContainer[i].second->GetButtonSize().x / mdButtonsContainer[i].second->GetButtonSize().y	);
				Shader::shaderDefault->setMat4("model", model);
				Shader::Draw(Shader::shaderDefault);
			}
			for (u16 i = 0; i < mdMovableContainer.size(); i++)
			{
				model = glm::mat4();
				model = glm::translate(model, glm::vec3(mdMovableContainer[i]->m_Pos, 1.f));
				model = glm::scale(model, glm::vec3(mdMovableContainer[i]->m_Size, 1.f));;
				Shader::shaderDefault->setFloat("aspectXY", mdMovableContainer[i]->m_Size.x / mdMovableContainer[i]->m_Size.y);
				Shader::shaderDefault->setMat4("model", model);
				Shader::Draw(Shader::shaderDefault);
			}

			model = glm::mat4();
			model = glm::translate(model, glm::vec3(mdResizableTop.m_Pos, 1.f));
			model = glm::scale(model, glm::vec3(mdResizableTop.m_Size, 1.f));;
			Shader::shaderDefault->setFloat("aspectXY", 0.8f);
			Shader::shaderDefault->setMat4("model", model);
			Shader::Draw(Shader::shaderDefault);

			
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(mdResizableBottom.m_Pos, 1.f));
			model = glm::scale(model, glm::vec3(mdResizableBottom.m_Size, 1.f));;
			Shader::shaderDefault->setFloat("aspectXY", 0.8f);
			Shader::shaderDefault->setMat4("model", model);
			Shader::Draw(Shader::shaderDefault);
			

			for (u16 i = 0; i < Interface::PlaylistButton::GetSize(); i++)
			{
				if (Interface::PlaylistButton::GetButton(i) != nullptr)
				{

					if (Interface::PlaylistButton::GetButton(i)->GetButtonPos() != glm::vec2(POS_INVALID))
					{
						model = glm::mat4();
						model = glm::translate(model, glm::vec3(Interface::PlaylistButton::GetButton(i)->GetButtonPos(), 1.f));
						model = glm::scale(model, glm::vec3(Interface::PlaylistButton::GetButton(i)->GetButtonSize(), 1.f));;
						Shader::shaderDefault->setFloat("aspectXY", Interface::PlaylistButton::GetButton(i)->GetButtonSize().x / Interface::PlaylistButton::GetButton(i)->GetButtonSize().y);
						Shader::shaderDefault->setMat4("model", model);
						Shader::Draw(Shader::shaderDefault);;
					}
				}
			}

			for (u16 i = 0; i < Interface::m_InterfaceButtonContainer.size(); i++)
			{
				if (Interface::m_InterfaceButtonContainer[i].second != nullptr)
				{
					if (Interface::m_InterfaceButtonContainer[i].second->GetButtonPos() != glm::vec2(POS_INVALID))
					{
						model = glm::mat4();
						model = glm::translate(model, glm::vec3(Interface::m_InterfaceButtonContainer[i].second->GetButtonPos(), 1.f));
						model = glm::scale(model, glm::vec3(Interface::m_InterfaceButtonContainer[i].second->GetButtonSize(), 1.f));;
						Shader::shaderDefault->setFloat("aspectXY", Interface::m_InterfaceButtonContainer[i].second->GetButtonSize().x / Interface::m_InterfaceButtonContainer[i].second->GetButtonSize().y);
						Shader::shaderDefault->setMat4("model", model);
						Shader::Draw(Shader::shaderDefault);;
					}
				}
			}

			auto sepCon = Interface::Separator::GetContainer();
			for (u16 i = 0; i < Interface::Separator::GetSize(); i++)
			{
				model = glm::mat4();
				model = glm::translate(model, glm::vec3(sepCon->at(i).second->GetButtonPos(), 1.f));
				model = glm::scale(model, glm::vec3(sepCon->at(i).second->GetButtonSize(), 1.f));;
				Shader::shaderDefault->setFloat("aspectXY", sepCon->at(i).second->GetButtonSize().x / sepCon->at(i).second->GetButtonSize().y);
				Shader::shaderDefault->setMat4("model", model);
				Shader::Draw(Shader::shaderDefault);;
			}



			glm::vec3 white(1.f);
			Shader::shaderDefault->setVec3("color", white);
			Shader::shaderDefault->setBool("border", false);
		}
#endif
	}

	void UI::HandleInput()
	{
		if (Input::isButtonPressed(Input::ButtonType::Exit))
		{
			if(State::CheckState(State::OnExitMinimizeToTray) == true)
				Window::HideToTray();
			else
				mdEngine::AppExit();
		}

		if (Input::isButtonPressed(Input::ButtonType::Minimize))
		{
			Window::MinimizeWindow();
		}


		if (App::Input::IsKeyPressed(App::KeyCode::F5))
		{
			//Window::ShowWindow();
			s32 x, y;
			Window::GetWindowPos(&x, &y);
			md_log_compare(x, y);
			Window::RestoreWindow();
			Window::ShowWindow();
			md_log("f5");
		}

		if (App::Input::IsKeyPressed(App::KeyCode::F6))
		{
			Window::MinimizeWindow();;
			md_log("f6");
		}

		if (Input::isButtonPressed(Input::ButtonType::Options))
		{
			md_log("settings");
			mdOptionsWindow.Init();
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

		if (App::Input::IsScrollForwardActive() && Graphics::MP::GetMainPlayerObject()->hasFocus())
		{
			Playlist::IncreaseVolume(App::InputEvent::kScrollEvent);
		}

		if (App::Input::IsScrollBackwardActive() && Graphics::MP::GetMainPlayerObject()->hasFocus())
		{
			Playlist::LowerVolume(App::InputEvent::kScrollEvent);
		}


		if(App::Input::IsKeyDown(App::KeyCode::LCtrl) == true && 
		   App::Input::IsKeyDown(App::KeyCode::A) == true)
		{
			auto audioCon = Audio::Object::GetAudioObjectContainer();
			Graphics::MP::GetPlaylistObject()->multipleSelect.clear();
			for (auto & i : audioCon)
			{
				Graphics::MP::GetPlaylistObject()->multipleSelect.push_back(&i->GetID());
			}
		}

	}

	void UI::UpdateMouseCursor()
	{
		b8 hasFocusResizable(false);
		if ((mdResizableBottom.hasFocus == true ||
			mdResizableTop.hasFocus == true) && 
			mdCursor == NULL)
		{
			mdCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
			hasFocusResizable = true;
		}
		else if (mdResizableBottom.hasFocus == true ||
				 mdResizableTop.hasFocus == true)
		{
			hasFocusResizable = true;
		}

		if (State::CheckState(State::Window::PositionChanged) == true && mdCursor == NULL)
		{
			mdCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
		}

		if(hasFocusResizable == false &&
		   State::CheckState(State::Window::PositionChanged) == false &&
		   mdCursor != NULL)
		{
			SDL_FreeCursor(mdCursor);
			mdCursor = NULL;
		}


		SDL_SetCursor(mdCursor);
	}

	void UI::HandlePlaylistInput()
	{

		for (auto & i : Graphics::MP::GetPlaylistObject()->GetIndexesToRender())
		{
			if (Audio::Object::GetAudioObject(i) == NULL)
				return;

			std::vector<s32*>::iterator it;
			s32 *currentPlaylistItemID = &Audio::Object::GetAudioObject(i)->GetID();

			if (Audio::Object::GetAudioObject(i)->isPressed == true)
			{
				clickDelayTimer.Start();
				Audio::Object::GetAudioObject(i)->Click();
				playlistItemSelected = true;

				// Check if current's item position is in vector with selected item's positions
				it = std::find(Graphics::MP::GetPlaylistObject()->multipleSelect.begin(),
									Graphics::MP::GetPlaylistObject()->multipleSelect.end(),
									&Audio::Object::GetAudioObject(i)->GetID());
				
				if (App::Input::IsKeyDown(App::KeyCode::LCtrl))
				{
					if(it == Graphics::MP::GetPlaylistObject()->multipleSelect.end())
						Graphics::MP::GetPlaylistObject()->multipleSelect.push_back(currentPlaylistItemID);
					else
					{
						Graphics::MP::GetPlaylistObject()->multipleSelect.erase(it);
					}
				}
				else
				{
					Graphics::MP::GetPlaylistObject()->multipleSelect.clear();
					Graphics::MP::GetPlaylistObject()->multipleSelect.push_back(currentPlaylistItemID);
				}

				if (Audio::Object::GetAudioObject(i)->GetClickCount() > 1)
				{
					State::SetState(State::AudioChosen);
					Playlist::RamLoadedMusic.load(Audio::Object::GetAudioObject(i));
					Playlist::PlayMusic();
				}

			}

			//If time for second click expires, reset click count
			if (clickDelayTimer.finished == true)
			{
				Audio::Object::GetAudioObject(i)->SetClickCount(0);
			}

		}

		// If delete pressed, delete every item on position from selected positions vector
		if (App::Input::IsKeyPressed(App::KeyCode::Delete) && 
			Graphics::MP::GetPlaylistObject()->multipleSelect.empty() == false)
		{
			/* If multiple items are selected, after deletion go back to the lowest position selected.
			   e.g. (6, 2, 9, 12) selected, sort it and select first item (2) 
			*/
			std::sort(Graphics::MP::GetPlaylistObject()->multipleSelect.begin(), 
					  Graphics::MP::GetPlaylistObject()->multipleSelect.end(),
				[&](const s32* first, const s32* second) -> bool { return *first < *second; });

			s32 temp = *Graphics::MP::GetPlaylistObject()->multipleSelect[0];
			for (s32 j = 0; j < Graphics::MP::GetPlaylistObject()->multipleSelect.size(); j++)
			{
				Playlist::DeleteMusic(*Graphics::MP::GetPlaylistObject()->multipleSelect[j]);
			}
			Graphics::MP::GetPlaylistObject()->multipleSelect.clear();
			
			if (Audio::Object::GetSize() <= temp && Audio::Object::GetAudioObjectContainer().empty() == false)
			{
				Graphics::MP::GetPlaylistObject()->multipleSelect.push_back(
								&Audio::Object::GetAudioObject(Audio::Object::GetSize() - 1)->GetID());
			}
			else if (Audio::Object::GetAudioObject(temp) != NULL)
			{
				Graphics::MP::GetPlaylistObject()->multipleSelect.push_back(&Audio::Object::GetAudioObject(temp)->GetID());
			}
		}
	}

	void UI::HandleSeparatorInput()
	{
		// On deletion reset flag, so it will be able to select
		if (State::CheckState(State::AudioDeleted) == true)
		{
			selectAllItemsInFolder = false;
		}

		auto sepCon = Interface::Separator::GetContainer();

		s32 idOfSelected = -1;
		for (s32 i = 0 ; i < sepCon->size(); i++)
		{
			if (sepCon->at(i).second->IsSelected() == true)
				idOfSelected = i;
		}

		for (auto & i : *sepCon)
		{
			if (i.second->isPressed == true)
			{
				clickDelayTimer.Start();
				i.second->Click();


				if(playlistItemSelected == true)
					i.second->Select(false);

				if (i.second->GetClickCount() > 0 &&
					i.second->IsSelected() == false &&
					i.second->IsSeparatorHidden() == false)
				{
					// Unselect separator that is already selected
					if (idOfSelected >= 0)
						sepCon->at(idOfSelected).second->Select(false);
					
					i.second->Select(true);
					playlistItemSelected = false;
				}
				else if(i.second->GetClickCount() > 0)
				{
					i.second->Select(false);
				}

				/*if (i.second->GetClickCount() > 1)
				{
					//md_log(std::to_string(i.second->GetClickCount()));
					folderItemsHidden = true;
				}

				auto audioCon = Audio::Object::GetAudioObjectContainer();
				if (folderItemsHidden == true)
				{
					i.second->SetClickCount(0);
					if (i.second->IsSeparatorHidden() == false)
					{
						State::SetState(State::AudioHidden);
						
						i.second->HideSeparator(true);
					}
					else
					{
						State::SetState(State::AudioHidden);
						
						i.second->HideSeparator(false);
					}
				}
				folderItemsHidden = false;

				if(i.second->IsSeparatorHidden() == true)
					Graphics::MP::GetPlaylistObject()->SetHiddenSeparatorCount(hiddenSeparatorsCount);*/


			}

			if (clickDelayTimer.finished == true)
				i.second->SetClickCount(0);
		}

		/*s32 hiddenSeparators = 0;
		for (auto & i : *sepCon)
		{
			if (i.second->IsSeparatorHidden() == true)
			{
				hiddenSeparators++;
			}
		}
		Graphics::MP::GetPlaylistObject()->SetHiddenSeparatorCount(hiddenSeparators);*/

	}

	void UI::OpenFileBrowserWrap()
	{
		mdWindowsFile::OpenFileBrowser();
		MP::UI::fileBrowserFinished = true;
	}

	void UI::OpenFolderBrowserWrap()
	{
		mdWindowsFile::OpenFolderBrowser(Strings::_SAVED_PATH);
		MP::UI::fileBrowserFinished = true;
	}

	void UI::PlaylistFileExplorer()
	{
		if (Graphics::MP::m_AddFileTextBox.isItemPressed(Strings::_PLAYLIST_ADD_FILE))
		{
			fileBrowserActive = true;
			std::thread t(OpenFileBrowserWrap);
			t.detach();
		}

		if (Graphics::MP::m_AddFileTextBox.isItemPressed(Strings::_PLAYLIST_ADD_FOLDER))
		{
			fileBrowserActive = true;
			std::thread t(OpenFolderBrowserWrap);
			t.detach();
		}


		if (fileBrowserFinished)
		{
			fileBrowserActive = false;
			std::wstring fileNames = mdWindowsFile::GetFileNames();

			std::wstring title;
			std::wstring dir;
			size_t dirPos = fileNames.find_first_of('\n');
			dir = fileNames.substr(0, dirPos);
			fileNames = fileNames.substr(dirPos + 1, fileNames.length());

			s32 nlCount = 0;
			for (s32 i = 0; i < fileNames.length(); i++)
			{
				if (fileNames[i] == '\n')
				{
					nlCount++;
					break;
				}
			}

			if (nlCount == 0)
			{
				Audio::PushToPlaylist(dir);;
			}
			else
			{
				while(fileNames.find_first_of('\n') != std::string::npos)
				{
					s32 titlePos = fileNames.find_first_of('\n');
					title = std::wstring();
					title += dir + L"\\";
					title += fileNames.substr(0, titlePos);
					fileNames = fileNames.substr(titlePos + 1, fileNames.length());;
					Audio::PushToPlaylist(title);
				}
			}

			fileBrowserFinished = false;
		}

	}

	Window::OptionsWindow* UI::GetOptionsWindow()
	{
		return &mdOptionsWindow;
	}
}
}