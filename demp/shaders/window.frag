#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D image;
uniform vec3 color;
uniform bool cut;

void main()
{
	vec4 texColor = texture(image, TexCoords);
	if(cut)
	{
	if(texColor.a < 0.9)
		discard;
	}
	else
	{
	if(texColor.a < 0.2)
		discard;
	}

	FragColor = texColor * vec4(color, 1.0);
}