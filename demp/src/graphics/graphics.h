#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "../utility/md_shader.h"
#include "../utility/md_types.h"

namespace mdEngine
{
	namespace Graphics
	{
		namespace Shader
		{
			extern mdShader* shaderDefault;;
			extern mdShader* shaderBorder;
			extern mdShader* shaderWindow;

			void InitShader();
			void UpdateProjectionMatrix();
			void Draw(mdShader* shader);
			void DrawMainWindow(mdShader* shader);
			void DrawDot();
			void DrawOutline(glm::vec4 dim, f32 scale, glm::vec3 color = glm::vec3(0.f));
			void ReloadOnNewContext();
			void Free();
		}

		void StartGraphics();

		void UpdateGraphics();

		void RenderGraphics();

		void CloseGraphics();

		glm::mat4* GetProjectionMatrix();

	}
}
#endif // !GRAPHICS_H

