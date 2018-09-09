#version 420 core

in vec2 TexCoords;
in vec4 pos;

vec4 FragColor;

uniform sampler2D image;
uniform vec3 color;
uniform bool cut;
uniform bool border;
uniform bool playlistCut;
uniform bool plain;
uniform bool roundEdges;
uniform bool roundEdgesBackground;

uniform float aspectXY;
uniform float border_width;
uniform float playlistMinY;
uniform float playlistMaxY;
uniform float playerHeightChange;
uniform float roundEdgeFactor;

float aspectYX = 1.0 / aspectXY;
float maxX = 1.0 - border_width / aspectXY;
float minX = border_width / aspectXY;
float maxY = (1.0 - border_width);
float minY = border_width;


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


	if(border)
	{
		if(TexCoords.x < maxX && TexCoords.x > minX &&
		   TexCoords.y < maxY && TexCoords.y > minY)
		{
			FragColor = vec4(1.0, 1.0, 1.0, 0.0);
		}
		else
		{
			FragColor = vec4(color, 1.0);
		}
	}
	else
	{
		FragColor = texColor * vec4(color, 1.0);
	}

	if(playlistCut)
	{
		if(pos.y > playlistMaxY || pos.y < playlistMinY)
		{
			FragColor = vec4(1.0, 1.0, 1.0, 0.0); 
		}
		else
		{
			FragColor = texColor * vec4(color, 1.0);
		}
	}

	if(plain)
	{
		FragColor = vec4(color, 1.0);
	}

	if(roundEdgesBackground == true)
	{
		float tex = 0.02 * playerHeightChange;
		// top right
		if(TexCoords.x < tex && TexCoords.y < tex && distance(TexCoords, vec2(tex)) > tex)
			FragColor = vec4(0.0); 
		// top left
		if(TexCoords.x > 1 - tex && TexCoords.y < tex && distance(TexCoords, vec2(1 - tex, tex)) > tex)
			FragColor = vec4(0.0); 
		// bottom right
		if(TexCoords.x < tex && TexCoords.y > 1 - tex && distance(TexCoords, vec2(tex, 1 - tex)) > tex)
			FragColor = vec4(0.0);
		// bottom left
		if(TexCoords.x > 1 - tex && TexCoords.y > 1 -tex && distance(TexCoords, vec2(1 - tex, 1 - tex)) > tex)
			FragColor = vec4(0.0);
	}



	gl_FragColor = FragColor;
}