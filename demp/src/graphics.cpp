#include "graphics.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include <thread>

#include "realtime_system_application.h"
#include "music_player_graphics.h"
#include "md_shape.h"
#include "md_text.h"

using namespace mdEngine::MP::UI;

namespace mdEngine
{
	namespace Graphics
	{
		namespace Shader
		{
			mdShader* shaderDefault	= NULL;
			mdShader* shaderBorder	= NULL;
			mdShape* quad			= NULL;
			mdShape* dot			= NULL;
		}

		void Shader::InitShader()
		{
			shaderDefault =  new mdShader("shaders/window.vert", "shaders/window.frag", nullptr);

			if (shaderDefault == NULL)
			{
				std::cout << "ERRROR: Could not initialize shader\n";
				return;
			}

			shaderBorder = new mdShader("shaders/border.vert", "shaders/border.frag", "shaders/border.geom");

			if (shaderBorder == NULL)
			{
				std::cout << "ERRROR: Could not initialize shader\n";
				return;
			}

			quad = mdShape::QUAD();

			if (quad == NULL)
			{
				std::cout << "ERROR: Could not initialize shape \"QUAD\"\n";
				return;
			}

			dot = mdShape::DOT();

			if (dot == NULL)
			{
				std::cout << "ERROR: Could not initialize shape \"QUAD\"\n";
				return;
			}


			glm::mat4 projection = glm::ortho(0.f, static_cast<float>(Window::windowProperties.mWindowWidth), 
												   static_cast<float>(Window::windowProperties.mWindowHeight), 0.f);
			shaderDefault->use();
			shaderDefault->setInt("image", 0);
			shaderDefault->setMat4("projection", projection);

			shaderBorder->use();
			shaderBorder->setMat4("projection", projection);

		}

		void Shader::Draw(mdShader* shader)
		{
			quad->Draw(*shader);
		}

		void Shader::DrawDot()
		{
			dot->Draw(*shaderBorder);
		}
	}

	void Graphics::StartGraphics()
	{
		
		Text::InitializeText();
		Shader::InitShader();
		MP::StartMainWindow();
	}

	void Graphics::UpdateGraphics()
	{
		MP::UpdateMainWindow();
	}

	void Graphics::RenderGraphics()
	{

		MP::RenderMainWindow();
	}
	void Graphics::CloseGraphics()
	{
		MP::CloseMainWindow();
	}

}