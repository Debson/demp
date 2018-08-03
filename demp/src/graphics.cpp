#include "graphics.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

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
			mdShader* shaderText	= NULL;
			mdShape* quad			= NULL;

		}

		void Shader::InitShader()
		{
			shaderDefault =  new mdShader("shaders/window.vert", "shaders/window.frag", nullptr);
			shaderText = new mdShader("shaders/text.vert", "shaders/text.frag", nullptr);

			if (shaderDefault == NULL)
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

			glm::mat4 projection = glm::ortho(0.f, static_cast<float>(Window::windowProperties.mWindowWidth), 
												   static_cast<float>(Window::windowProperties.mWindowHeight), 0.f);
			shaderDefault->use();
			shaderDefault->setInt("image", 0);
			shaderDefault->setMat4("projection", projection);

			projection = glm::ortho(0.f, 800.f, 0.f, 600.f);
			shaderText->use();
			shaderText->setInt("text", 0);
			shaderText->setMat4("projection", projection);


		}

		void Shader::Draw()
		{
			quad->Draw(*shaderDefault);
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
		//MP::RenderTest();
	}
	void Graphics::CloseGraphics()
	{
		MP::CloseMainWindow();
	}

}