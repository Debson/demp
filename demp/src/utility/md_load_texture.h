#pragma once

#ifndef MDLOAD_TEXTURE_H
#define MDLOAD_TEXTURE_H

#include <GL/gl3w.h>
#include <glm.hpp>
#include <iostream>
#include <vector>
#include <sstream>

#include "../../external/stb_image.h"
#include <SDL_image.h>


GLuint mdLoadTexture(std::string path);
SDL_Texture* mdLoadTextureSDL(SDL_Renderer* renderer, std::string path);

#endif // !MDLOAD_TEXUTRE_H

