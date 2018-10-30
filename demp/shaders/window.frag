#version 150 core
in vec2 TexCoords;

uniform sampler2D image;
uniform vec3 color;

void main()
{
	vec4 texColor = texture(image, TexCoords);

	gl_FragColor = texColor * vec4(color, 1.0);
}