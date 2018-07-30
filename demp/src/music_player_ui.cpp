#include "music_player_ui.h"

#include <iostream>
#include <vector>
#include <map>
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


namespace mdEngine
{
namespace MP
{
	namespace UI
	{
		static std::vector<Movable*> mdMovableContainer;
		std::vector<std::pair<Input::ButtonType, Button*>> mdButtonContainer;

#ifdef _DEBUG_
		Shader mdDebugShader;
		Shape * mdQuad = NULL;
		GLuint mdDebugTex;
#endif

		Movable * mainBar;

		Button * exit;
		
		b8 music_repeat = false;
		b8 music_shuffle = false;
		s32 music_position = 0;
		
		s32 volume_scroll_step = 2;
		s32 volume_fade_time = 500;

		std::string title = "none";

		ImVec4 ClearColor = ImVec4(1.f, 254.f/255.f, 1.f, 1.f);

		void HandleInput();

		void DebugStart();

		void DebugRender();

	}

	UI::Movable::Movable(s32 xL, s32 yU, s32 xR, s32 yD) : 
						 xL(xL), yU(yU), xR(xR), yD(yD)  
	{ 
		mdMovableContainer.push_back(this);
	}

	UI::Movable::Movable() { }

	UI::Movable::~Movable() { delete this; }

	UI::Button::Button(Input::ButtonType type, s32 xL, s32 yU, s32 xR, s32 yD) :
					   xL(xL), yU(yU), xR(xR), yD(yD), isPressed(false), isDown(false), hasFocus(false), isReleased(false)
	{
		mdButtonContainer.push_back(std::make_pair(type, this));
	}

	UI::Button::Button() { }

	UI::Button::~Button() { delete this;  }


	namespace UI
	{
		void DebugStart()
		{
#ifdef _DEBUG_
			mdDebugShader = Shader("shaders/window.vert", "shaders/window.frag", nullptr);
			mdQuad = Shape::QUAD();

			mdDebugShader.use();
			mdDebugShader.setInt("image", 0);

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

				ImGui::Checkbox("Repeat", &music_repeat);
				Playlist::SetRepeatState(music_repeat);
				ImGui::SameLine();
				if (ImGui::Checkbox("Shuffle", &music_shuffle))
					Playlist::SetShuffleState(music_shuffle);

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
			mdDebugShader.use();
			model = glm::translate(model, glm::vec3(1.f - (130.f/360.f) * 2.f, 1.f - (40.f / 360.f) * 2.f, 0.5f));
			model = glm::scale(model, glm::vec3((15.f/360.f) * 2.f, (20.f/360.f) * 2.f, 1.f));;
			mdDebugShader.setMat4("model", model);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mdDebugTex);
			mdQuad->Draw(mdDebugShader);

#endif
		}

		void Start()
		{
			mainBar = new Movable(0, 0, mdDefaultWindowProperties.mWindowWidth - 20, 20);

			exit = new Button(Input::ButtonType::Exit, mdDefaultWindowProperties.mWindowWidth - 130,
						  40, mdDefaultWindowProperties.mWindowWidth - 150, 60);

			DebugStart();
		}

		void Update()
		{
			for(u16 i = 0; i < mdMovableContainer.size(); i++)
				App::WindowMovableBar(mdMovableContainer[i]);

			for (u16 i = 0; i < mdButtonContainer.size(); i++)
				App::ProcessButtons(mdButtonContainer[i].second);

			HandleInput();
		}

		void Render()
		{

			DebugRender();


		}

		void HandleInput()
		{
			if (exit->isPressed == true)
			{
				mdEngine::AppExit();
			}
		}
	}

}
}