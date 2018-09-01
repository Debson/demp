#ifndef MD_UTIL_H
#define MD_UTIL_H


#include "../settings/configuration.h"

#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define ZERO_MEM_VAR(var) memset(&var, 0, sizeof(var))
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

#define SAFE_DELETE(p) if (p) { delete p; p = NULL; }

#define TEMPLATE template <typename T>


#ifdef _DEBUG_OUTPUT_
#define MD_SDL_ERROR(x) (SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, x, SDL_GetError(), nullptr))

#define MD_BASS_ERROR(x) (SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR", x, nullptr))

#define MD_ERROR(x) (std::cout << "ERROR: " << x << std::endl)

#define md_log(x) (std::cout << "LOG: " << x << std::endl);

#define md_log_compare(x, y) (std::cout << "LOG: " << x << "   -   " << y << std::endl); 

#endif

#endif // !MD_UTIL_H
