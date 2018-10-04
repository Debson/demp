#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "../utility/md_shader.h"

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
			void Draw(mdShader* shader);
			void DrawDot();
			void ReloadOnNewContext();
			void Free();
		}

		void StartGraphics();

		void UpdateGraphics();

		void RenderGraphics();

		void CloseGraphics();

	}
}
#endif // !GRAPHICS_H

