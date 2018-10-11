#version 330 core
layout (points) in;
layout (points, max_vertices = 256) out;

uniform mat4 projection;
uniform mat4 model;

uniform float xOffset = 0.1;
uniform float yOffset = 0.1;
uniform vec2 playlistBoundsY;

vec4 pos;
vec4 FragPos;
vec4 CurrentPos;


void main()
{
	gl_PointSize = 1.0;

	for(int i = 0; i < 100; i++)
	{
		pos = gl_in[0].gl_Position + vec4(xOffset * i, 0.0, 0.0, 0.0);
		FragPos = projection * model * pos;
		CurrentPos = model * vec4(pos.xy, 0.0, 1.0);
		if(CurrentPos.y < playlistBoundsY.x || CurrentPos.y > playlistBoundsY.y)
			FragPos = vec4(1.0, 1.0, 1.0, 0.0);
		gl_Position = FragPos;
		EmitVertex();
	}

	for(int i = 0; i < 101; i++)
	{
		pos = gl_in[0].gl_Position + vec4(xOffset * i, 1.0, 0.0, 0.0);
		FragPos = projection * model * pos;
		CurrentPos = model * vec4(pos.xy, 0.0, 1.0);
		if(CurrentPos.y < playlistBoundsY.x || CurrentPos.y > playlistBoundsY.y)
			FragPos = vec4(1.0, 1.0, 1.0, 0.0);
		gl_Position = FragPos;;
		EmitVertex();
	}

	for(int i = 0; i < 10; i++)
	{
		pos = gl_in[0].gl_Position + vec4(0.0, yOffset * i, 0.0, 0.0);
		FragPos = projection * model * pos;
		CurrentPos = model * vec4(pos.xy, 0.0, 1.0);;
		if(CurrentPos.y < playlistBoundsY.x || CurrentPos.y > playlistBoundsY.y)
			FragPos = vec4(1.0, 1.0, 1.0, 0.0);
		gl_Position = FragPos;
		EmitVertex();
	}

	for(int i = 0; i < 10; i++)
	{
		pos = gl_in[0].gl_Position + vec4(1.0, yOffset * i, 0.0, 0.0);
		FragPos = projection * model * pos;
		CurrentPos = model * vec4(pos.xy, 0.0, 1.0);
		if(CurrentPos.y < playlistBoundsY.x || CurrentPos.y > playlistBoundsY.y)
			FragPos = vec4(1.0, 1.0, 1.0, 0.0);
		gl_Position = FragPos;
		EmitVertex();
	}

	EndPrimitive();
}