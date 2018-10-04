#include "graphics.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "music_player_graphics.h"
#include "../app/realtime_system_application.h"
#include "../utility/md_shape.h"
#include "../utility/md_text.h"
#include "../settings/music_player_settings.h"

using namespace mdEngine::MP::UI;

namespace mdEngine
{
	namespace Graphics
	{
		namespace Shader
		{
			mdShader* shaderDefault	= NULL;
			mdShader* shaderBorder	= NULL;
			mdShader* shaderWindow  = NULL;	
			mdShape* quad			= NULL;
			mdShape* dot			= NULL;
		}

		void Shader::InitShader()
		{
			shaderDefault =  new mdShader("shaders/default.vert", "shaders/default.frag", nullptr);

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

			/*shaderWindow = new mdShader("shaders/window.vert", "shaders/window.frag", nullptr);

			if (shaderWindow == NULL)
			{
				std::cout << "ERRROR: Could not initialize shader\n";
				return;
			}*/

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

			/*projection = glm::mat4();
			projection = glm::ortho(0.f, mdEngine::MP::Data::_OPTIONS_WINDOW_SIZE.x, 
										 mdEngine::MP::Data::_OPTIONS_WINDOW_SIZE.y, 
									0.f);

			shaderWindow->use();
			shaderWindow->setInt("image", 0);
			shaderWindow->setMat4("projection", projection);*/

		}

		void Shader::Draw(mdShader* shader)
		{
			quad->Draw(shader);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		void Shader::DrawDot()
		{
			dot->Draw(shaderBorder);
		}


		void Shader::ReloadOnNewContext()
		{
			shaderDefault = new mdShader("shaders/default.vert", "shaders/default.frag", nullptr);

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

			shaderDefault->use();
			shaderDefault->setInt("image", 0);
		}

		void Shader::Free()
		{
			delete shaderDefault;
			delete shaderBorder;
			delete shaderWindow;
			//delete quad;
			//delete dot;
		}
	}

	void Graphics::StartGraphics()
	{
		Text::InitializeText();
		Shader::InitShader();
		StartMainWindow();
	}

	void Graphics::UpdateGraphics()
	{
		UpdateMainWindow();
	}

	void Graphics::RenderGraphics()
	{
		RenderMainWindow();
	}

	void Graphics::CloseGraphics()
	{
		Shader::Free();
		CloseMainWindow();
	}

}