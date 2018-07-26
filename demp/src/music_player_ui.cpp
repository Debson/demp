#include "music_player_ui.h"

#include <iostream>

#include "../external/imgui/imgui.h"
#include "md_types.h"

namespace mdEngine
{
namespace MP
{
	namespace UI
	{
		b8 show_demo_window = true;
		b8 show_another_window = false;

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
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
			ImGui::ColorEdit3("clear color", (float*)&ClearColor); // Edit 3 floats representing a color
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

	}
}
}