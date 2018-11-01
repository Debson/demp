#include "music_player_ui.h"

#include <vector>
#include <locale.h>
#include <thread>
#include <future>
#include <chrono>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <boost/filesystem.hpp>

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
#include "../interface/md_helper_windows.h"
#include "../utility/md_windows.h"


using namespace mdEngine::Graphics;
using namespace std::chrono_literals;
namespace fs = boost::filesystem;

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
		SDL_Cursor*				mdCursor;
		Interface::Movable*		mdPlaylistMovableLeft;
		Interface::Movable*		mdPlaylistMovableRight;
		Interface::Movable*		mdPlaylistMovableBottom;
		Interface::Button*		mdAddFilesButton;


		std::thread* fileLoadThread;
		std::atomic<bool> fileBrowserFinished(false);
		
#ifdef _DEBUG_
		b8 renderDebug = false;
		glm::vec3 borderColor(0.f, 1.f, 0.f);
#endif
		
		b8 music_repeat = false;
		b8 music_shuffle = false;
		s32 music_position = 0;

		Time::Timer clickDelayTimer;
		Time::Timer arrowsDelayTimer;
		Time::Timer arrowsDelayIntervalTimer;


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

		void UpdateInterfaceObjects();

		void DebugStart();

		void DebugRender();

		void OpenFileBrowserWrap();

		void OpenFolderBrowserWrap();

		void PlaylistItemsArrowsScrollUp();

		void PlaylistItemArrowsScrollDown();

		void FileDragDetector();
	}

	void UI::Start()
	{
		//std::string locale = setlocale(LC_ALL, "");
		//std::cout << "default locale:" << locale;

		mdCurrentWidth = mdEngine::Window::WindowProperties.m_WindowWidth;
		mdCurrentHeight = mdEngine::Window::WindowProperties.m_ApplicationHeight;

		mdDefaultWidth = 500.f;
		mdDefaultHeight = 350.f;
		//TOP bar
		new Interface::Movable(glm::vec2(mdDefaultWidth, 40), glm::vec2(0.f, 0.f));
		//new Interface::Movable(glm::vec2(20, Data::_DEFAULT_PLAYER_SIZE.y), glm::vec2(0.f, 0.f));
		//new Interface::Movable(glm::vec2(20, Data::_DEFAULT_PLAYER_SIZE.y), glm::vec2(Data::_DEFAULT_PLAYER_SIZE.x - 20, 0.f));

		// Middle
		s32 sizeY = 130;
		s32 posY = 110;
		new Interface::Movable(glm::vec2(Data::_DEFAULT_PLAYER_SIZE.x, sizeY), glm::vec2(0.f, Data::_DEFAULT_PLAYER_SIZE.y - posY));

		// Sides
		s32 sizeX = 20;
		mdPlaylistMovableLeft	= new Interface::Movable(glm::vec2(sizeX, Window::WindowProperties.m_ApplicationHeight), glm::vec2(0.f));
		mdPlaylistMovableRight	= new Interface::Movable(glm::vec2(sizeX, Window::WindowProperties.m_ApplicationHeight), glm::vec2(Data::_DEFAULT_PLAYER_SIZE.x - sizeX, 0.f));

		// bottom
		sizeX = 25;
		mdPlaylistMovableBottom = new Interface::Movable(glm::vec2(Data::_DEFAULT_PLAYER_SIZE.x, sizeX), glm::vec2(0.f, Window::WindowProperties.m_ApplicationHeight - sizeX));


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

		new Interface::Button(Input::ButtonType::StayOnTop, Data::_STAY_ON_TOP_BUTTON_SIZE, Data::_STAY_ON_TOP_BUTTON_POS);

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

		mdAddFilesButton = new Interface::Button(Input::ButtonType::PlaylistAddFile, Data::_PLAYLIST_ADD_BUTTON_SIZE, Data::_PLAYLIST_ADD_BUTTON_POS);
		// temporary
		//new Interface::Button(Input::ButtonType::PlaylistAddFolder, Data::_PLAYLIST_ADD_BUTTON_SIZE, Data::_PLAYLIST_ADD_BUTTON_POS);

		clickDelayTimer				= Time::Timer(Data::_PLAYLIST_CHOOSE_ITEM_DELAY);
		arrowsDelayTimer			= Time::Timer(Data::_PLAYLIST_ARROWS_SCROLL_DELAY);
		arrowsDelayIntervalTimer	= Time::Timer(Data::_PLAYLIST_ARROWS_SCROLL_INTERVAL);

		DebugStart();
	}

	void UI::Update()
	{
		FileDragDetector();

		if (State::CheckState(State::OtherWindowHasFocus) == true || 
			State::CheckState(State::PlaylistMovement) == true ||
			State::CheckState(State::Window::HasFocus) == false)
		{
			if (mdCursor != NULL)
			{
				SDL_FreeCursor(mdCursor);
				mdCursor = NULL;
			}

			return;
		}
		// Files are loading, don't take any input from user at that time
		if (State::CheckState(State::FilesLoaded) == false)
			//State::CheckState(State::Window::HasFocus) == false)
			return;


		/* Collect user input for buttons and movable */
		for(u16 i = 0; i < mdMovableContainer.size(); i++)
			App::ProcessMovable(mdMovableContainer[i]);

		App::ProcessResizableTop(&mdResizableTop, &mdResizableBottom);

		App::ProcessResizableBottom(&mdResizableBottom, &mdResizableTop);
		//md_log("bottom processed");

		if (State::CheckState(State::Window::Resized) == true ||
			State::CheckState(State::Window::PositionChanged) == true)
		{
			mdResizableBottom.m_Pos = glm::vec2(mdResizableBottom.m_Pos.x,
												Window::WindowProperties.m_ApplicationHeight - mdResizableBottom.m_Size.y);
		}

		UpdateMouseCursor();

		for (u16 i = 0; i < mdButtonsContainer.size(); i++)
			App::ProcessButton(mdButtonsContainer[i].second);

		if (Graphics::MP::GetPlaylistObject()->IsEnabled() == true)
		{
			// Make sure that hitboxes that are not visible cannot be clicked
			App::SetButtonCheckBounds(Data::_PLAYLIST_ITEMS_SURFACE_POS.y, Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y);
			for (auto i : *Graphics::MP::GetPlaylistObject()->GetIndexesToRender())
			{
				if (Audio::Object::GetAudioObject(i) == nullptr ||
					State::CheckState(State::Window::Resized) == true)
				{
					break;
				}

				if (Audio::Object::GetAudioObject(i)->IsPlaylistItemHidden() == false &&
					Graphics::MP::GetPlaylistObject()->PlaylistTextBoxActive == false)
				{
					App::ProcesPlaylistButton(Audio::Object::GetAudioObject(i));
				}
				else if (Graphics::MP::GetPlaylistObject()->PlaylistTextBoxActive == true)
				{
					Audio::Object::GetAudioObject(i)->bottomHasFocus = false;
					Audio::Object::GetAudioObject(i)->topHasFocus = false;
				}

			}

			// Process playlist separator buttons
			auto sepCon = Interface::Separator::GetContainer();
			for (u16 i = 0; i < Interface::Separator::GetSize(); i++)
			{
				App::ProcesPlaylistButton(sepCon->at(i).second);
			}
		}

		// Proces interface buttons
		for (u16 i = 0; i < Interface::m_InterfaceButtonContainer.size(); i++)
		{
			assert(Interface::m_InterfaceButtonContainer[i].second != nullptr);

			App::ProcessButton(Interface::m_InterfaceButtonContainer[i].second);
		}

		HandleInput();
		if (State::CheckState(State::PlaylistEmpty) == false &&
			Graphics::MP::GetPlaylistObject()->IsEnabled() == true)
		{
			HandlePlaylistInput();
			HandleSeparatorInput();
			clickDelayTimer.Update();
		}

		PlaylistFileExplorer();
		UpdateInterfaceObjects();
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
			//delete Interface::m_InterfaceButtonContainer[i].second;
			//Interface::m_InterfaceButtonContainer[i].second = nullptr;
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
		std::vector<s32> vec;
		for (s32 i = 1; i < len; i++)
			vec.push_back(i);

		std::reverse(vec.begin(), vec.end());

		Playlist::DeleteMusic(&vec);
		
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

			ImGui::Text("Separators loaded: %d", Interface::Separator::GetSize());

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
				Graphics::PrintVisibleItemsInfo();
			}

			if (ImGui::Button("Print audio object info") == true)
			{
				Graphics::PrintAudioObjectInfo();
			}

			if (ImGui::Button("Print indexes to render") == true)
			{
				Graphics::PrintIndexesToRender();
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

			for(auto & i : *Audio::Object::GetAudioObjectContainer())
			{
				if (i != nullptr)
				{
					if (i->GetPlaylistItemPos() != glm::vec2(POS_INVALID))
					{
						model = glm::mat4();
						model = glm::translate(model, glm::vec3(i->GetPlaylistItemPos(), 1.f));
						model = glm::scale(model, glm::vec3(i->GetButtonSize(), 1.f));;
						Shader::shaderDefault->setFloat("aspectXY", i->GetButtonSize().x / i->GetButtonSize().y);
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

			for (auto & i : *Interface::Separator::GetContainer())
			{
				model = glm::mat4();
				model = glm::translate(model, glm::vec3(i.second->GetPlaylistItemPos(), 1.f));
				model = glm::scale(model, glm::vec3(i.second->GetButtonSize(), 1.f));;
				Shader::shaderDefault->setFloat("aspectXY", i.second->GetButtonSize().x / i.second->GetButtonSize().y);
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

		if (Graphics::m_SettingsTextBox != NULL && 
			Graphics::m_SettingsTextBox->isItemPressed(0) && 
			Window::mdOptionsWindow == NULL)
		{
			Window::mdOptionsWindow = new Window::OptionsWindow();
			Window::WindowsContainer.insert(std::pair< std::string, Window::WindowObject*>("OptionsWindow", Window::mdOptionsWindow));
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

		if (App::Input::IsScrollForwardActive() && (Graphics::MP::GetMainPlayerObject()->hasFocus() || State::CheckState(State::Window::MouseOnTrayIcon)))
		{
			Playlist::IncreaseVolume(App::InputEvent::kScrollEvent);
		}

		if (App::Input::IsScrollBackwardActive() && (Graphics::MP::GetMainPlayerObject()->hasFocus() || State::CheckState(State::Window::MouseOnTrayIcon)))
		{
			Playlist::LowerVolume(App::InputEvent::kScrollEvent);
		}

		if (App::Input::IsKeyPressed(App::KeyCode::Space) == true)
		{
			Playlist::PauseMusic();
		}


		if(App::Input::IsKeyDown(App::KeyCode::LCtrl) == true && 
		   App::Input::IsKeyDown(App::KeyCode::A) == true)
		{
			auto audioCon = Audio::Object::GetAudioObjectContainer();
			Graphics::MP::GetPlaylistObject()->multipleSelect.clear();
			for (auto & i : *audioCon)
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

		if (State::CheckState(State::Window::PositionChanged) == true && mdCursor == NULL &&
			Input::hasFocus(Input::ButtonType::Options) == false)
		{
			mdCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
		}

		b8 buttonsFocus = Input::hasFocus(Input::ButtonType::SliderMusic);

		for (auto & i : mdButtonsContainer)
		{
			buttonsFocus = buttonsFocus || i.second->hasFocus;
		}

		for (auto & i : *Graphics::MP::GetPlaylistObject()->GetIndexesToRender())
		{
			assert(Audio::Object::GetAudioObject(i) != nullptr);
			buttonsFocus = buttonsFocus || Audio::Object::GetAudioObject(i)->hasFocus;
		}

		if (buttonsFocus == true && mdCursor == NULL)
		{
			mdCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
		}

		b8 deleteCursor = hasFocusResizable == false &&
						  buttonsFocus == false &&
						  State::CheckState(State::Window::PositionChanged) == false;


		if(deleteCursor == true && mdCursor != NULL)
		{
			SDL_FreeCursor(mdCursor);
			mdCursor = NULL;
		}


		SDL_SetCursor(mdCursor);
	}

	void UI::PlaylistItemsArrowsScrollUp()
	{
		if (Graphics::MP::GetPlaylistObject()->multipleSelect.empty() == true)
		{
			for (auto & i : *Audio::Object::GetAudioObjectContainer())
			{
				Graphics::MP::GetPlaylistObject()->multipleSelect.push_back(&i->GetID());
			}
		}
		else
		{
			if (*Graphics::MP::GetPlaylistObject()->multipleSelect.front() > 0)
			{
				u32 temp = *Graphics::MP::GetPlaylistObject()->multipleSelect.front();
				Graphics::MP::GetPlaylistObject()->multipleSelect.clear();
				Graphics::MP::GetPlaylistObject()->multipleSelect.push_back(
					&Audio::Object::GetAudioObject(temp - 1)->GetID()
				);
			}
			else if (Graphics::MP::GetPlaylistObject()->multipleSelect.size() == 1)
			{
				Graphics::MP::GetPlaylistObject()->multipleSelect.clear();
				for (auto & i : *Audio::Object::GetAudioObjectContainer())
				{
					Graphics::MP::GetPlaylistObject()->multipleSelect.push_back(&i->GetID());
				}
			}
		}

		Graphics::UpdatePlaylistCursorOffset();
	}

	void UI::PlaylistItemArrowsScrollDown()
	{
		if (Graphics::MP::GetPlaylistObject()->multipleSelect.empty() == true)
		{
			for (auto & i : *Audio::Object::GetAudioObjectContainer())
			{
				Graphics::MP::GetPlaylistObject()->multipleSelect.push_back(&i->GetID());
			}
		}
		else
		{
			if (Graphics::MP::GetPlaylistObject()->multipleSelect.size() > 1)
			{
				Graphics::MP::GetPlaylistObject()->multipleSelect.clear();
				Graphics::MP::GetPlaylistObject()->multipleSelect.push_back(&Audio::Object::GetAudioObject(0)->GetID());
			}
			else if (*Graphics::MP::GetPlaylistObject()->multipleSelect.front() < Audio::Object::GetSize() - 1)
			{
				u32 temp = *Graphics::MP::GetPlaylistObject()->multipleSelect.front();
				Graphics::MP::GetPlaylistObject()->multipleSelect.clear();
				Graphics::MP::GetPlaylistObject()->multipleSelect.push_back(
					&Audio::Object::GetAudioObject(temp + 1)->GetID()
				);
			}
		}

		Graphics::UpdatePlaylistCursorOffset();
	}

	void UI::HandlePlaylistInput()
	{
		if (App::Input::IsKeyDown(App::KeyCode::Up) == true)
		{
			if (App::Input::IsKeyPressed(App::KeyCode::Up) == true)
			{
				arrowsDelayTimer.Start();
				arrowsDelayIntervalTimer.Start();
				PlaylistItemsArrowsScrollUp();
			}
			else if (arrowsDelayTimer.IsFinished() == true &&
					 arrowsDelayIntervalTimer.IsFinished() == true)
			{
				arrowsDelayIntervalTimer.Start();
				PlaylistItemsArrowsScrollUp();
			};
		}

		if (App::Input::IsKeyDown(App::KeyCode::Down) == true)
		{
			if (App::Input::IsKeyPressed(App::KeyCode::Down) == true)
			{
				arrowsDelayTimer.Start();
				arrowsDelayIntervalTimer.Start();
				PlaylistItemArrowsScrollDown();
			}
			else if (arrowsDelayTimer.IsFinished() == true &&
					 arrowsDelayIntervalTimer.IsFinished() == true)
			{
				arrowsDelayIntervalTimer.Start();
				PlaylistItemArrowsScrollDown();
			}
		}

		for (auto & i : *Graphics::MP::GetPlaylistObject()->GetIndexesToRender())
		{
			if (Audio::Object::GetAudioObject(i) == NULL ||
				State::CheckState(State::FilesDroppedNotLoaded) == true)
			{
				return;
			}

			std::vector<s32*>::iterator it;
			s32 *currentPlaylistItemID = &Audio::Object::GetAudioObject(i)->GetID();

			if (Audio::Object::GetAudioObject(i)->isPressedRight == true)
			{
				//Graphics::MP::GetPlaylistObject()->multipleSelect.clear();
				Graphics::MP::GetPlaylistObject()->multipleSelect.push_back(currentPlaylistItemID);
				std::sort(Graphics::MP::GetPlaylistObject()->multipleSelect.begin(), Graphics::MP::GetPlaylistObject()->multipleSelect.end());

				Graphics::MP::GetPlaylistObject()->multipleSelect.erase(std::unique(Graphics::MP::GetPlaylistObject()->multipleSelect.begin(), Graphics::MP::GetPlaylistObject()->multipleSelect.end()), Graphics::MP::GetPlaylistObject()->multipleSelect.end());
			}

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
					if (it == Graphics::MP::GetPlaylistObject()->multipleSelect.end())
					{
						Graphics::MP::GetPlaylistObject()->multipleSelect.push_back(currentPlaylistItemID);
					}
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
					// Make sure audio file is available
					if (Playlist::RamLoadedMusic.load(Audio::Object::GetAudioObject(i)) == true)
					{
						if(Playlist::RamLoadedMusic.m_ID < Audio::Object::GetSize())
							Audio::Object::GetAudioObject(Playlist::RamLoadedMusic.m_ID)->DeleteAlbumImageTexture();
						Playlist::PlayMusic();
						State::SetState(State::AudioChosen);
					}
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
				[&](const s32* first, const s32* second) -> bool { return *first > *second; });

			// WHY WHEN IT IS SORTED IN DESCENDING ORDER IT DELETES FASTER????????????????
			s32 temp = *Graphics::MP::GetPlaylistObject()->multipleSelect.back();
			std::vector<s32> indexes;
			for (auto i : Graphics::MP::GetPlaylistObject()->multipleSelect)
			{
				indexes.push_back(*i);
			}

			Playlist::DeleteMusic(&indexes);
			
			Graphics::MP::GetPlaylistObject()->multipleSelect.clear();
			
			if (Audio::Object::GetSize() <= temp && Audio::Object::GetAudioObjectContainer()->empty() == false)
			{
				Graphics::MP::GetPlaylistObject()->multipleSelect.push_back(
								&Audio::Object::GetAudioObject(Audio::Object::GetSize() - 1)->GetID());
			}
			else if (Audio::Object::GetAudioObject(temp) != NULL)
			{
				Graphics::MP::GetPlaylistObject()->multipleSelect.push_back(&Audio::Object::GetAudioObject(temp)->GetID());
			}
		}



		if (App::Input::IsKeyPressed(App::KeyCode::Return))
		{
			if (Graphics::MP::GetPlaylistObject()->multipleSelect.empty() == false)
			{
				s32 index = *Graphics::MP::GetPlaylistObject()->multipleSelect.front();
				State::SetState(State::AudioChosen);
				if (Playlist::RamLoadedMusic.m_ID < Audio::Object::GetSize())
					Audio::Object::GetAudioObject(Playlist::RamLoadedMusic.m_ID)->DeleteAlbumImageTexture();
				Playlist::RamLoadedMusic.load(Audio::Object::GetAudioObject(index));

				Graphics::MP::GetPlaylistObject()->multipleSelect.clear();
				Graphics::MP::GetPlaylistObject()->multipleSelect.push_back(Audio::Object::GetAudioObject(index)->GetIDP());
				Playlist::PlayMusic();
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
		if (Graphics::m_AddFileTextBox != NULL)
		{
			if (Graphics::m_AddFileTextBox->isItemPressed(0))
			{
#ifdef _WIN32_
				fileBrowserActive = true;
				std::thread t(OpenFileBrowserWrap);
				t.detach();
#else
#endif
			}

			if (Graphics::m_AddFileTextBox->isItemPressed(1))
			{
#ifdef _WIN32_
				fileBrowserActive = true;
				std::thread t(OpenFolderBrowserWrap);
				t.detach();
#else
#endif
			}
		}

		if (Graphics::m_SettingsTextBox != NULL)
		{
			if (Graphics::m_SettingsTextBox->isItemPressed(1))
			{
#ifdef _WIN32_
				fileBrowserActive = true;
				std::thread t(OpenFileBrowserWrap);
				t.detach();
#else
#endif
			}

			if (Graphics::m_SettingsTextBox->isItemPressed(2))
			{
#ifdef _WIN32_
				fileBrowserActive = true;
				std::thread t(OpenFolderBrowserWrap);
				t.detach();
#else
#endif
			}
		}


		if (fileBrowserFinished)
		{
			// After file browser captuer mouse is switching off
			SDL_CaptureMouse(SDL_TRUE);

			fileBrowserActive = false;
			std::string fileNames = utf16_to_utf8(mdWindowsFile::GetFileNames());

			if (fileNames == "")
			{
				fileBrowserFinished = false;
				return;
			}

			std::string title;
			std::string dir;
			size_t dirPos = fileNames.find_first_of('\n');
			dir = fileNames.substr(0, dirPos);
			fileNames = fileNames.substr(dirPos + 1, fileNames.length());

			if (State::CheckState(State::FilesLoaded) == false)
				return;

			State::OnFileAddition();

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
				Audio::DroppedItemsCount++;
				Audio::SaveDroppedPath(dir);
			}
			else
			{
				while(fileNames.find_first_of('\n') != std::string::npos)
				{
					s32 titlePos = fileNames.find_first_of('\n');
					title = std::string();
					title += dir + "\\";
					title += fileNames.substr(0, titlePos);
					fileNames = fileNames.substr(titlePos + 1, fileNames.length());

					Audio::DroppedItemsCount++;
					{
						Audio::SaveDroppedPath(title);
					}
				}


			}

			State::SetState(State::AddedByFileBrowser);
			fileLoadThread = new std::thread(Audio::OnDropComplete);
			fileLoadThread->detach();
			delete fileLoadThread;

			fileBrowserFinished = false;
		}

	}

	void UI::UpdateInterfaceObjects()
	{
		mdPlaylistMovableLeft->m_Size = glm::vec2(mdPlaylistMovableLeft->m_Size.x, Window::WindowProperties.m_ApplicationHeight);
		mdPlaylistMovableRight->m_Size = glm::vec2(mdPlaylistMovableRight->m_Size.x, Window::WindowProperties.m_ApplicationHeight);
		mdPlaylistMovableBottom->m_Pos = glm::vec2(mdPlaylistMovableBottom->m_Pos.x, Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y);
		mdPlaylistMovableBottom->m_Size = glm::vec2(mdPlaylistMovableBottom->m_Size.x, Window::WindowProperties.m_ApplicationHeight - Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y - mdResizableBottom.m_Size.y);

		mdAddFilesButton->SetButtonPos(Data::_PLAYLIST_ADD_BUTTON_POS);
		mdAddFilesButton->SetButtonSize(Data::_PLAYLIST_ADD_BUTTON_SIZE);
	}

	void UI::FileDragDetector()
	{

	}
}
}