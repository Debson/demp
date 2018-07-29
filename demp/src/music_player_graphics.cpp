#include "music_player_graphics.h"

#include <gtc/matrix_transform.hpp>

#include "realtime_system_application.h"
#include "md_shader.h"
#include "md_shape.h"

namespace mdEngine
{
namespace Graphics
{
	namespace MP
	{
		Shader mdWindowShader;
		Shape * mdQuad = NULL;

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
		glm::mat4 projection;
		projection = glm::ortho(0.f, 500.f, 800.f, 0.f, -1.f, 1.f);
		mdWindowShader.use();
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
		model = glm::translate(model, glm::vec3(-1.0f, 0.90f, 0.f));
		model = glm::scale(model, glm::vec3(2.0f, 0.1f, 0.f));
		glm::vec3 color(1.f, 0.f, 0.f);
		mdWindowShader.setMat4("model", model);
		mdWindowShader.setVec3("color", color);
		mdQuad->Draw(mdWindowShader);

		/* exit*/
		model = glm::mat4();
		mdWindowShader.use();
		model = glm::translate(model, glm::vec3(0.92f, 0.90f, 0.f));
		model = glm::scale(model, glm::vec3(0.08f, 0.1f, 0.f));
		color = glm::vec3(0.f, 1.f, 0.f);
		mdWindowShader.setMat4("model", model);
		mdWindowShader.setVec3("color", color);
		mdQuad->Draw(mdWindowShader);

	}

	void MP::CloseMainWindow()
	{

	}
}
}