#include "md_load_texture.h"

#define STB_IMAGE_IMPLEMENTATION

GLuint mdLoadTexture(std::string path)
{
	//stbi_set_flip_vertically_on_load(true);
	GLuint textureID = 0;

	GLint width, height, nrComponents;
	unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
	
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data); // generate a texture
		glGenerateMipmap(GL_TEXTURE_2D); // Automatically generate all required mipmaps for currently bound texture

										 // Set texture wrapping and filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

GLuint mdLoadTexture(void* data, size_t size)
{
	GLuint textureID = 0;	

	int width, height, nrComponents;

	unsigned char* dataUC = stbi_load_from_memory((unsigned char*)data, size, &width, &height, &nrComponents, 4);

	if (dataUC)
	{
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, dataUC); // generate a texture
		glGenerateMipmap(GL_TEXTURE_2D); // Automatically generate all required mipmaps for currently bound texture

										 // Set texture wrapping and filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "Texture failed to load." << std::endl;
	}

	stbi_image_free(dataUC);

	return textureID;
}

SDL_Texture* mdLoadTextureSDL(SDL_Renderer* renderer, std::string path)
{
	SDL_Texture* newTex = NULL;

	SDL_Surface* loadedSurface = IMG_Load(path.c_str());

	newTex = SDL_CreateTextureFromSurface(renderer, loadedSurface);

	SDL_FreeSurface(loadedSurface);

	return newTex;
}
