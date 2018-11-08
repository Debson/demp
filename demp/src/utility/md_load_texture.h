#pragma once

#ifndef MDLOAD_TEXTURE_H
#define MDLOAD_TEXTURE_H


#include <GL/gl3w.h>
#include <glm.hpp>
#include <iostream>
#include <vector>
#include <sstream>

#include "../../external/stb_image.h"

#include "md_types.h"



GLuint mdLoadTexture(std::string path, mdEngine::b8 clamp = false);
//GLuint mdLoadTexture(void* data, mdEngine::u32 size);
GLuint mdLoadTexture(void* data, mdEngine::u32 size);
GLuint mdLoadTexture(unsigned char* data, mdEngine::s32 width, mdEngine::s32 height);


#endif // !MDLOAD_TEXUTRE_H

