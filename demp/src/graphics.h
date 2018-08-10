#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "md_shader.h"

namespace mdEngine
{
	namespace Graphics
	{
		namespace Shader
		{
			extern mdShader* shaderDefault;;
			extern mdShader* shaderBorder;

			void InitShader();
			void Draw(mdShader* shader);
			void DrawDot();
		}

		void StartGraphics();

		void UpdateGraphics();

		void RenderGraphics();

		void CloseGraphics();

	}
}
#endif // !GRAPHICS_H

