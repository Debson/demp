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
#include "../interface/md_interface.h"
#include "../audio/mp_audio.h"
#include "../sqlite/md_sqlite.h"
#include "../settings/music_player_string.h"
#include "../settings/music_player_settings.h"
#include "../graphics/graphics.h"
#include "../graphics/music_player_graphics.h"
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
		std::vector<Interface::Movable*> mdMovableContainer;
		std::vector<Interface::Resizable*> mdResizableContainer;
		std::vector<std::pair<Input::ButtonType, Interface::Button*>> mdButtonsContainer;

		std::atomic<bool> fileBrowserFinished(false);
		

#ifdef _DEBUG_
		b8 renderDebug = false;
		glm::vec3 borderColor(0.f, 1.f, 0.f);

		void DeleteAll();
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

		b8 fileBrowserActive(false);

		std::string title = "none";

		ImVec4 ClearColor = ImVec4(1.f, 254.f/255.f, 1.f, 1.f);

		std::locale l("");

		void HandleInput();

		void HandlePlaylistInput();

		void PlaylistFileExplorer();

		void DebugStart();

		void DebugRender();

		void OpenFileBrowserWrap();

		void OpenFolderBrowserWrap();

	}

	
	void UI::DebugStart()
	{
#ifdef _DEBUG_
		
#endif
	}

	void UI::DeleteAll()
	{
		s32 len = Audio::Object::GetSize();
		for (s32 j = 0; j < len; j++)
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
			ImGui::InputInt("Scroll volume step", &Data::VolumeScrollStep, 1, 1);
			Playlist::SetScrollVolumeStep(Data::VolumeScrollStep);

			ImGui::InputInt("Volume fade time", &Data::VolumeFadeTime, 20, 100);
			Playlist::SetVolumeFadeTime(Data::VolumeFadeTime);


			if (ImGui::Button("Delete All") == true)
			{
				//std::thread t(UI::DeleteAll);
				//t.detach();

				UI::DeleteAll();
			}


			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Other") == true)
		{
			ImGui::Text("Songs loaded: %d", Audio::Object::GetSize());

			ImGui::Text("Processed items count: %u", Audio::GetProccessedFileCount());

			ImGui::Text("Current shuffle pos: %d", Playlist::GetCurrentShufflePos());

			ImGui::Text("Shuffle container size: %d", Playlist::GetShuffleContainerSize());


			if (ImGui::Button("Print test") == true)
			{
				Audio::PrintTest();
			}
			

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
				for (u16 i = 0; i < Graphics::MP::GetPlaylistObject()->multipleSelect.size(); i++)
				{
					std::cout << *Graphics::MP::GetPlaylistObject()->multipleSelect[i] << std::endl;
				}
			}

			f32 bitrate = 0.f;
			if (ImGui::Button("Print Bitrate") == true)
			{
				Playlist::GetBitrate(&bitrate);
				std::cout << bitrate << std::endl;
			}


			if (ImGui::Button("Print items container") == true)
			{
				std::thread t(Audio::GetItemsInfo);
				t.detach();
			}

			if (ImGui::Button("Print loaded folders") == true)
			{
				Audio::Folders::PrintContent();
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

			for (u16 i = 0; i < mdResizableContainer.size(); i++)
			{
				model = glm::mat4();
				model = glm::translate(model, glm::vec3(mdResizableContainer[i]->m_Pos, 1.f));
				model = glm::scale(model, glm::vec3(mdResizableContainer[i]->m_Size, 1.f));;
				Shader::shaderDefault->setFloat("aspectXY", mdResizableContainer[i]->m_Size.x / mdResizableContainer[i]->m_Size.y);
				Shader::shaderDefault->setMat4("model", model);
				Shader::Draw(Shader::shaderDefault);
			}

			for (u16 i = 0; i < Interface::PlaylistButton::GetSize(); i++)
			{
				if (Interface::PlaylistButton::GetButton(i) != nullptr)
				{

					if (Interface::PlaylistButton::GetButton(i)->GetButtonPos() != glm::vec2(INVALID))
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

			/*for (u16 i = 0; i < Interface::mdInterfaceButtonContainer.size(); i++)
			{
				if (Interface::mdInterfaceButtonContainer[i].second != nullptr)
				{
					if (Interface::mdInterfaceButtonContainer[i].second->GetButtonPos() != glm::vec2(INVALID))
					{
						model = glm::mat4();
						model = glm::translate(model, glm::vec3(Interface::mdInterfaceButtonContainer[i].second->GetButtonPos(), 1.f));
						model = glm::scale(model, glm::vec3(Interface::mdInterfaceButtonContainer[i].second->GetButtonSize(), 1.f));;
						Shader::shaderDefault->setFloat("aspectXY", Interface::mdInterfaceButtonContainer[i].second->GetButtonSize().x / Interface::mdInterfaceButtonContainer[i].second->GetButtonSize().y);
						Shader::shaderDefault->setMat4("model", model);
						Shader::Draw(Shader::shaderDefault);;
					}
				}
			}*/



			glm::vec3 white(1.f);
			Shader::shaderDefault->setVec3("color", white);
			Shader::shaderDefault->setBool("border", false);
		}
#endif
	}

	void UI::Start()
	{
		//std::string locale = setlocale(LC_ALL, "");
		//std::cout << "default locale:" << locale;

		mdCurrentWidth = mdEngine::Window::windowProperties.mWindowWidth;
		mdCurrentHeight = mdEngine::Window::windowProperties.mApplicationHeight;

		mdDefaultWidth = 500.f;
		mdDefaultHeight = 350.f;

		new Interface::Movable(glm::vec2(mdDefaultWidth, 20), glm::vec2(0.f, 0.f));

		new Interface::Resizable(glm::vec2(mdCurrentWidth, 20), glm::vec2(0, mdCurrentHeight - 20.f));

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

		for (u16 i = 0; i < mdResizableContainer.size(); i++)
			App::ProcessResizable(mdResizableContainer[i]);

		for (u16 i = 0; i < mdButtonsContainer.size(); i++)
			App::ProcessButton(mdButtonsContainer[i].second);

		// Make sure that hitboxes that are not visible cannot be clicked
		App::SetButtonCheckBounds(Data::_PLAYLIST_ITEMS_SURFACE_POS.y, Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y, true);
		for (u16 i = 0; i < Interface::PlaylistButton::GetSize(); i++)
		{
			if (Interface::PlaylistButton::GetButton(i) == nullptr)
				break;

			App::ProcessButton(Interface::PlaylistButton::GetButton(i));
		}
		App::SetButtonCheckBounds(Data::_PLAYLIST_ITEMS_SURFACE_POS.y, Data::_PLAYLIST_ITEMS_SURFACE_SIZE.y, false);

		/*for (u16 i = 0; i < Interface::mdInterfaceButtonContainer.size(); i++)
		{
			assert(Interface::mdInterfaceButtonContainer[i].second != nullptr);

			App::ProcessButton(Interface::mdInterfaceButtonContainer[i].second);
		}*/


		HandleInput();
		if(State::IsPlaylistEmpty == false)
			HandlePlaylistInput();

		//PlaylistFileExplorer();

	}

	void UI::Render()
	{

		DebugRender();

	}

	
	void UI::HandleInput()
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

		if (App::Input::IsScrollForwardActive() && Graphics::MP::GetMainPlayerObject()->hasFocus())
		{
			Playlist::IncreaseVolume(App::InputEvent::kScrollEvent);
		}

		if (App::Input::IsScrollBackwardActive() && Graphics::MP::GetMainPlayerObject()->hasFocus())
		{
			Playlist::LowerVolume(App::InputEvent::kScrollEvent);
		}

	}

	void UI::HandlePlaylistInput()
	{
		for (s32 i = 0; i < Audio::Object::GetSize(); i++)
		{
			if (Audio::Object::GetAudioObject(i) == NULL)
				return;

			std::vector<s32*>::iterator it;
			s32 *currentPlaylistItemID = &Audio::Object::GetAudioObject(i)->GetID();

			if (App::Input::IsKeyDown(App::KeyCode::LCtrl) && App::Input::IsKeyDown(App::KeyCode::A))
			{
				it = std::find(Graphics::MP::GetPlaylistObject()->multipleSelect.begin(),
							   Graphics::MP::GetPlaylistObject()->multipleSelect.end(),
							   currentPlaylistItemID);
				if(it == Graphics::MP::GetPlaylistObject()->multipleSelect.end())
					Graphics::MP::GetPlaylistObject()->multipleSelect.push_back(
						currentPlaylistItemID);
			}
			else if (Audio::Object::GetAudioObject(i)->isPressed == true)
			{

				clickDelayTimer.start();
				Audio::Object::GetAudioObject(i)->Click();


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
					MP::musicPlayerState = MP::MusicPlayerState::kMusicChosen;
					Playlist::RamLoadedMusic.load(Audio::Object::GetAudioObject(i)->GetPath(), i);
					Playlist::PlayMusic();
				}

			}

			//If time for second click expires, reset click count
			if (clickDelayTimer.finished == true)
				Audio::Object::GetAudioObject(i)->SetClickCount(0);

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

		// Update second click timer
		clickDelayTimer.update();
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
				std::wstring * path = new std::wstring(dir);
				MP::PushToPlaylist(path);
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
					std::wstring * path = new std::wstring(title);
					MP::PushToPlaylist(path);
				}
			}

			fileBrowserFinished = false;
		}

	}

	void UI::Close()
	{
		for (size_t i = 0; i < mdMovableContainer.size(); i++)
		{
			delete mdMovableContainer[i];
			mdMovableContainer[i] == nullptr;
		}
		mdMovableContainer.clear();

		for (size_t i = 0; i < mdResizableContainer.size(); i++)
		{
			delete mdResizableContainer[i];
			mdResizableContainer[i] = nullptr;
		}
		mdResizableContainer.clear();

		for (size_t i = 0; i < mdButtonsContainer.size(); i++)
		{
			delete mdButtonsContainer[i].second;
			mdButtonsContainer[i].second = nullptr;
		}
		mdButtonsContainer.clear();

		for (size_t i = 0; i < Interface::mdInterfaceButtonContainer.size(); i++)
		{
			delete Interface::mdInterfaceButtonContainer[i].second;
			Interface::mdInterfaceButtonContainer[i].second = nullptr;
		}
		Interface::mdInterfaceButtonContainer.clear();

	}
}
}