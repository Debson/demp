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
		glm::mat4 mdProjection;

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

			mdProjection = glm::ortho(0.f, static_cast<float>(Window::WindowProperties.m_WindowWidth), 
										   static_cast<float>(Window::WindowProperties.m_WindowHeight), 0.f);
			shaderDefault->use();
			shaderDefault->setInt("image", 0);
			shaderDefault->setMat4("projection", mdProjection);

			shaderBorder->use();
			shaderBorder->setMat4("projection", mdProjection);
		}

		void Shader::UpdateProjectionMatrix()
		{
			if (Window::WindowProperties.m_ApplicationHeight > Window::WindowProperties.m_WindowHeight)
			{
				mdProjection = glm::ortho(0.f, static_cast<float>(Window::WindowProperties.m_WindowWidth),
											   static_cast<float>(Window::WindowProperties.m_ApplicationHeight), 0.f);
				shaderDefault->use();
				shaderDefault->setMat4("projection", mdProjection);
				md_log(Window::WindowProperties.m_ApplicationHeight);
				Window::SetWindowSize(Window::WindowProperties.m_WindowWidth, Window::WindowProperties.m_ApplicationHeight);
			}
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

		void Shader::DrawOutline(glm::vec4 dim, f32 scale, glm::vec3 color)
		{
			shaderDefault->use();
			glm::mat4 model;
			f32 scaleX, scaleY;
			if (dim.z > dim.w)
			{
				scaleX = 1.f + (scale - 1.f) * dim.w / dim.z;
				scaleY = scale;
			}
			else
			{
				scaleX = scale;
				scaleY = 1.f + (scale - 1.f) * dim.w / dim.z;
			}

			model = glm::translate(model, glm::vec3(dim.x - (dim.z * scaleX - dim.z) / 2.f, dim.y - (dim.w * scaleY - dim.w) / 2.f, 0.01f));
			model = glm::scale(model, glm::vec3(dim.z * scaleX, dim.w * scaleY, 1.0));
			shaderDefault->setBool("plain", true);
			shaderDefault->setMat4("model", model);
			shaderDefault->setVec3("color", color);
			Draw(shaderDefault);
			shaderDefault->setBool("plain", false);
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

	glm::mat4* Graphics::GetProjectionMatrix()
	{
		return &mdProjection;
	}

}