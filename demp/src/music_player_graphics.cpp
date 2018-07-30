#include "music_player_graphics.h"

#include <gtc/matrix_transform.hpp>

#include "realtime_system_application.h"
#include "md_shader.h"
#include "md_shape.h"
#define STB_IMAGE_IMPLEMENTATION
#include "md_load_texture.h"
#include "music_player_ui_input.h"

using namespace mdEngine::MP::UI;

namespace mdEngine
{
namespace Graphics
{
	namespace MP
	{
		Shader mdWindowShader;
		Shape * mdQuad = NULL;
		GLuint main_background, main_foreground;
		GLuint exit_background, exit_icon;
		GLuint volume_bar_bounds, volume_bar_middle, volume_bar_lines;
		GLuint play_button;

		s32 mdCurrentWidth = 500;
		s32 mdCurrentHeight = 800;

		void GetWindowScale(f32* scaleX, f32* scaleY);
	}

	void MP::GetWindowScale(f32* scaleX, f32* scaleY)
	{
		int w, h;
		GetWindowSize(&w, &h);

		*scaleX = (float)w / (float)mdCurrentWidth;
		*scaleY = (float)h / (float)mdCurrentHeight;
	}

	void MP::StartMainWindow()
	{
		mdQuad = Shape::QUAD();
		mdWindowShader = Shader("shaders/window.vert", "shaders/window.frag", nullptr);

		main_background = mdLoadTexture("assets/main.png");
		main_foreground = mdLoadTexture("assets/main_foreground.png");

		exit_background = mdLoadTexture("assets/exit_background.png");
		exit_icon = mdLoadTexture("assets/exit_icon.png");

		volume_bar_bounds = mdLoadTexture("assets/volume_bar_bounds.png");
		volume_bar_middle = mdLoadTexture("assets/volume_bar_middle.png");
		volume_bar_lines = mdLoadTexture("assets/volume_bar_lines.png");

		play_button = mdLoadTexture("assets/play_button.png");


		glm::mat4 projection;
		projection = glm::ortho(0.f, 360.f, 200.f, 0.f, 0.1f, 100.f);

		mdWindowShader.use();
		mdWindowShader.setInt("image", 0);
		mdWindowShader.setMat4("projection", projection);
	}

	void MP::UpdateMainWindow()
	{
		

	}

	void MP::RenderMainWindow()
	{
		/* Main Bar */
		glm::mat4 model;

		mdWindowShader.use();

		/* Main background*/
		{
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 0.99f));
			model = glm::scale(model, glm::vec3(2.0f, 2.0f, 1.f));;
			mdWindowShader.setMat4("model", model);
			mdWindowShader.setBool("cut", true);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, main_background);
			mdQuad->Draw(mdWindowShader);
			mdWindowShader.setBool("cut", false);

			model = glm::mat4();
			model = glm::translate(model, glm::vec3(-0.9f, -0.74f, 0.98f));
			model = glm::scale(model, glm::vec3(1.8f, 1.5f, 1.f));;
			mdWindowShader.setMat4("model", model);
			mdWindowShader.setBool("cut", true);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, main_foreground);
			mdQuad->Draw(mdWindowShader);
			mdWindowShader.setBool("cut", false);
		}

		/* Volume bar */
		{
			glm::vec3 scale = glm::vec3(0.60f, 0.08f, 1.f);

			/*model = glm::mat4();
			model = glm::translate(model, glm::vec3(0.23f, -0.695f, 0.95f));
			model = glm::scale(model, scale);
			mdWindowShader.setMat4("model", model);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, volume_bar_lines);
			mdQuad->Draw(mdWindowShader);*/

			model = glm::mat4();
			model = glm::translate(model, glm::vec3(0.23f, -0.4f, 0.95f));
			model = glm::scale(model, scale);
			mdWindowShader.setMat4("model", model);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, volume_bar_bounds);
			mdQuad->Draw(mdWindowShader);

			model = glm::mat4();
			model = glm::translate(model, glm::vec3(0.23f, -0.4f, 0.94f));
			model = glm::scale(model, scale);
			mdWindowShader.setMat4("model", model);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, volume_bar_middle);
			mdQuad->Draw(mdWindowShader);
		}

		/*  Music UI*/
		{
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(-0.05f, -0.7f, 0.94f));
			model = glm::scale(model, glm::vec3(0.2f, 0.25f, 1.f));;
			mdWindowShader.setMat4("model", model);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, play_button);
			mdQuad->Draw(mdWindowShader);
		}




		/* UI Window buttons*/
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(0.68f, 0.825f, 0.98f));
		model = glm::scale(model, glm::vec3(0.23f, 0.15f, 1.f));;
		mdWindowShader.setMat4("model", model);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, exit_background);
		mdQuad->Draw(mdWindowShader);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(0.83f, 0.85f, 0.02f));
		model = glm::scale(model, glm::vec3(0.07f, 0.1f, 0.1f));;
		mdWindowShader.setMat4("model", model);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, exit_icon);
		mdQuad->Draw(mdWindowShader);






	}

	void MP::CloseMainWindow()
	{

	}
}
}