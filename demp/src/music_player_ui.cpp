#include "music_player_ui.h"

#include <iostream>

#include "../external/imgui/imgui.h"
#include "music_player_playlist.h"
#include "md_types.h"



namespace mdEngine
{
namespace MP
{
	namespace UI
	{
		b8 music_repeat = false;
		b8 music_shuffle = false;
		s32 music_position = 0;
		
		s32 volume_scroll_step = 2;
		s32 volume_fade_time = 500;

		std::string title = "none";

		ImVec4 ClearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	}

	namespace UI
	{
		void Start()
		{
			
		}

		void Update()
		{

		}

		void Render()
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
				if(ImGui::Checkbox("Shuffle", &music_shuffle))
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
#endif
		}

	}
}
}