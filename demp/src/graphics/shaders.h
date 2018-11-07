#ifndef SHADERS_H
#define SHADERS_H

#include <string>

namespace mdEngine
{
	namespace ShadersCode
	{

		const std::string Default_Vert = "#version 330 core\n\
						layout(location = 0) in vec4 vertex;\n\
			out vec2 TexCoords;\n\
			out vec4 pos;\n\
			uniform mat4 model;\n\
			uniform mat4 projection;\n\
			void main()\n\
			{\n\
				TexCoords = vertex.zw;\n\
				pos = model * vec4(vertex.xy, 0.0, 1.0);\n\
				gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);\n\
			}\n\
			";

		const std::string Default_Frag = "#version 330 core\n\
\n\
		in vec2 TexCoords;\n\
		in vec4 pos;\n\
\n\
		vec4 FragColor;\n\
\n\
		uniform sampler2D image;\n\
		uniform vec3 color;\n\
		uniform vec4 colorRGBA;\n\
		uniform bool cut;\n\
		uniform bool border;\n\
		uniform bool playlistCutY;\n\
		uniform bool playlistCutX;\n\
		uniform bool plain;\n\
		uniform bool plainRGBA;\n\
		uniform bool roundEdges;\n\
		uniform bool roundEdgesBackground;\n\
		uniform bool outlineEnabled;\n\
\n\
		uniform float aspectXY;\n\
		uniform float border_width;\n\
		uniform float playlistMinY;\n\
		uniform float playlistMaxY;\n\
		uniform float transVal = 1.0;\n\
\n\
		uniform vec2 playlistBoundsY;\n\
		uniform vec2 playlistBoundsX;\n\
\n\
		uniform float playerHeightChange;\n\
		uniform float roundEdgeFactor;\n\
\n\
\n\
		float aspectYX = 1.0 / aspectXY;\n\
		float maxX = 1.0 - border_width / aspectXY;\n\
		float minX = border_width / aspectXY;\n\
		float maxY = (1.0 - border_width);\n\
		float minY = border_width;\n\
\n\
\n\
		void main()\n\
		{\n\
\n\
			vec4 texColor = texture(image, TexCoords);\n\
			if (cut)\n\
			{\n\
				if (texColor.a < 0.9)\n\
					discard;\n\
			}\n\
			else\n\
			{\n\
				if (texColor.a < 0.1)\n\
					discard;\n\
			}\n\
\n\
\n\
			if (border)\n\
			{\n\
				if (TexCoords.x < maxX && TexCoords.x > minX &&\n\
					TexCoords.y < maxY && TexCoords.y > minY)\n\
				{\n\
					FragColor = vec4(1.0, 1.0, 1.0, 0.0);\n\
				}\n\
				else\n\
				{\n\
					FragColor = vec4(color, 1.0);\n\
				}\n\
			}\n\
			else\n\
			{\n\
				FragColor = texColor * vec4(color, transVal);\n\
			}\n\
\n\
			if (playlistCutY)\n\
			{\n\
				if (pos.y > playlistBoundsY.y || pos.y < playlistBoundsY.x)\n\
				{\n\
					FragColor = vec4(1.0, 1.0, 1.0, 0.0);\n\
				}\n\
				else\n\
				{\n\
					FragColor = texColor * vec4(color, 1.0);\n\
				}\n\
			}\n\
\n\
			if (playlistCutX)\n\
			{\n\
				if (pos.x > playlistBoundsX.y || pos.x < playlistBoundsX.x)\n\
				{\n\
					FragColor = vec4(1.0, 1.0, 1.0, 0.0);\n\
				}\n\
				else\n\
				{\n\
					FragColor = texColor * vec4(color, 1.0);\n\
				}\n\
			}\n\
\n\
			if (plain)\n\
			{\n\
				FragColor = vec4(color, 1.0);\n\
			}\n\
\n\
			if (plainRGBA)\n\
			{\n\
				FragColor = colorRGBA;\n\
			}\n\
\n\
			if (roundEdgesBackground == true)\n\
			{\n\
				float tex = 0.02 * playerHeightChange;\n\
				// top right\n\
				if (TexCoords.x < tex && TexCoords.y < tex && distance(TexCoords, vec2(tex)) > tex)\n\
					FragColor = vec4(0.0);\n\
				// top left\n\
				if (TexCoords.x > 1 - tex && TexCoords.y < tex && distance(TexCoords, vec2(1 - tex, tex)) > tex)\n\
					FragColor = vec4(0.0);\n\
				// bottom right\n\
				if (TexCoords.x < tex && TexCoords.y > 1 - tex && distance(TexCoords, vec2(tex, 1 - tex)) > tex)\n\
					FragColor = vec4(0.0);\n\
				// bottom left\n\
				if (TexCoords.x > 1 - tex && TexCoords.y > 1 - tex && distance(TexCoords, vec2(1 - tex, 1 - tex)) > tex)\n\
					FragColor = vec4(0.0);\n\
			}\n\
\n\
\n\
\n\
			gl_FragColor = FragColor;\n\
		}\
			";

			const std::string Border_Vert = "#version 330 core\n\
				layout(location = 0) in vec2 Pos;\n\
\n\
			void main()\n\
			{\n\
				gl_Position = vec4(Pos, 0.0, 1.0);\n\
			}\n\
			";

			const std::string Border_Frag = "#version 330 core\n\
\n\
				uniform vec3 color;\n\
\n\
			void main()\n\
			{\n\
				gl_FragColor = vec4(color, 1.0);\n\
			}\n\
			";

			const std::string Border_Geom = "#version 330 core\n\
				layout(points) in;\n\
			layout(points, max_vertices = 256) out;\n\
\n\
			uniform mat4 projection;\n\
			uniform mat4 model;\n\
\n\
			uniform float xOffset = 0.1;\n\
			uniform float yOffset = 0.1;\n\
			uniform vec2 playlistBoundsY;\n\
\n\
			vec4 pos;\n\
			vec4 FragPos;\n\
			vec4 CurrentPos;\n\
\n\
\n\
			void main()\n\
			{\n\
				gl_PointSize = 1.0;\n\
\n\
				for (int i = 0; i < 100; i++)\n\
				{\n\
					pos = gl_in[0].gl_Position + vec4(xOffset * i, 0.0, 0.0, 0.0);\n\
					FragPos = projection * model * pos;\n\
					CurrentPos = model * vec4(pos.xy, 0.0, 1.0);\n\
					if (CurrentPos.y < playlistBoundsY.x || CurrentPos.y > playlistBoundsY.y)\n\
						FragPos = vec4(1.0, 1.0, 1.0, 0.0);\n\
					gl_Position = FragPos;\n\
					EmitVertex();\n\
				}\n\
\n\
				for (int i = 0; i < 101; i++)\n\
				{\n\
					pos = gl_in[0].gl_Position + vec4(xOffset * i, 1.0, 0.0, 0.0);\n\
					FragPos = projection * model * pos;\n\
					CurrentPos = model * vec4(pos.xy, 0.0, 1.0);\n\
					if (CurrentPos.y < playlistBoundsY.x || CurrentPos.y > playlistBoundsY.y)\n\
						FragPos = vec4(1.0, 1.0, 1.0, 0.0);\n\
					gl_Position = FragPos;\n\
					EmitVertex();\n\
				}\n\
\n\
				for (int i = 0; i < 10; i++)\n\
				{\n\
					pos = gl_in[0].gl_Position + vec4(0.0, yOffset * i, 0.0, 0.0);\n\
					FragPos = projection * model * pos;\n\
					CurrentPos = model * vec4(pos.xy, 0.0, 1.0);\n\
					if (CurrentPos.y < playlistBoundsY.x || CurrentPos.y > playlistBoundsY.y)\n\
						FragPos = vec4(1.0, 1.0, 1.0, 0.0);\n\
					gl_Position = FragPos;\n\
					EmitVertex();\n\
				}\n\
\n\
				for (int i = 0; i < 10; i++)\n\
				{\n\
					pos = gl_in[0].gl_Position + vec4(1.0, yOffset * i, 0.0, 0.0);\n\
					FragPos = projection * model * pos;\n\
					CurrentPos = model * vec4(pos.xy, 0.0, 1.0);\n\
					if (CurrentPos.y < playlistBoundsY.x || CurrentPos.y > playlistBoundsY.y)\n\
						FragPos = vec4(1.0, 1.0, 1.0, 0.0);\n\
					gl_Position = FragPos;\n\
					EmitVertex();\n\
				}\n\
\n\
				EndPrimitive();\n\
			}\n\
			";
			

	}
}


#endif // !SHADERS_H
