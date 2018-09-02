#include "md_types.h"


namespace mdEngine
{
	glm::vec3 Color::White(1.f);
	glm::vec3 Color::Black(0.f);
	glm::vec3 Color::Grey(0.85f);
	glm::vec3 Color::DarkGrey(0.5f);
	glm::vec3 Color::Red(1.f, 0.f, 0.f);
	glm::vec3 Color::Green(0.f, 1.f, 0.f);
	glm::vec3 Color::Blue(0.f, 0.f, 1.f);
	glm::vec3 Color::Pink(1.f, 0.0784f, 0.576f);


	SDL_Color SDLColor::White		= { 255, 255, 255 };
	SDL_Color SDLColor::Black		= { 0, 0, 0 };
	SDL_Color SDLColor::Grey		= { 216, 216, 216 };
	SDL_Color SDLColor::DarkGrey	= { 127, 127, 127 };
	SDL_Color SDLColor::Red			= {	255, 0, 0 };
	SDL_Color SDLColor::Green		= { 0, 255, 0 };
	SDL_Color SDLColor::Blue		= { 0, 0, 255};

}