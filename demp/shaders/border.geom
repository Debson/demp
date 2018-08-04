#version 420 core
layout (points) in;
layout (points, max_vertices = 256) out;


uniform mat4 projection;
uniform mat4 model;

uniform float xOffset = 0.1;
uniform float yOffset = 0.1;
vec4 pos;

void main()
{
	gl_PointSize = 1.0;

	for(int i = 0; i < 100; i++)
	{
		pos = gl_in[0].gl_Position + vec4(xOffset * i, 0.0, 0.0, 0.0);
		gl_Position = projection * model * pos;
		EmitVertex();
	}

	for(int i = 0; i < 10; i++)
	{
		pos = gl_in[0].gl_Position + vec4(0.0, yOffset * i, 0.0, 0.0);
		gl_Position = projection * model * pos;
		EmitVertex();
	}


	for(int i = 0; i < 100; i++)
	{
		pos = gl_in[0].gl_Position + vec4(xOffset * i, 1.0, 0.0, 0.0);
		gl_Position = projection * model * pos;
		EmitVertex();
	}

	for(int i = 0; i < 10; i++)
	{
		pos = gl_in[0].gl_Position + vec4(1.0, yOffset * i, 0.0, 0.0);
		gl_Position = projection * model * pos;
		EmitVertex();
	}

	EndPrimitive();
}