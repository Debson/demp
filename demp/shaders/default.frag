#version 330 core

in vec2 TexCoords;
in vec4 pos;

vec4 FragColor;

uniform sampler2D image;
uniform vec3 color;
uniform vec4 colorRGBA;
uniform bool cut;
uniform bool border;
uniform bool playlistCutY;
uniform bool playlistCutX;
uniform bool plain;
uniform bool plainRGBA;
uniform bool roundEdges;
uniform bool roundEdgesBackground;
uniform bool outlineEnabled;

uniform float aspectXY;
uniform float border_width;
uniform float playlistMinY;
uniform float playlistMaxY;
uniform float transVal = 1.0;

uniform vec2 playlistBoundsY;
uniform vec2 playlistBoundsX;

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
		if(texColor.a < 0.1)
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
		FragColor = texColor * vec4(color, transVal);
	}

	if(playlistCutY)
	{
		if(pos.y > playlistBoundsY.y || pos.y < playlistBoundsY.x)
		{
			FragColor = vec4(1.0, 1.0, 1.0, 0.0); 
		}
		else
		{
			FragColor = texColor * vec4(color, 1.0);
		}
	}
	if(playlistCutX)
	{
		if(pos.x > playlistBoundsX.y || pos.x < playlistBoundsX.x)
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
	if(plainRGBA)
	{
		FragColor = colorRGBA;
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