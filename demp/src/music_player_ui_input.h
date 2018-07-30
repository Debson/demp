#ifndef MUSIC_PLAYER_UI_INPUT_H
#define MUSIC_PLAYER_UI_INPUT_H

#include "md_types.h"

namespace mdEngine
{
namespace MP
{
namespace UI
{
	namespace Input
	{

		enum ButtonType
		{
			Exit,
			Minimize,
			StayOnTop,
			Play,
			Next,
			Previous,
			Shuffle,
			Repeat,

			count
		};

		b8 isButtonPressed(ButtonType code);
		b8 isButtonReleased(ButtonType code);
		b8 isButtonDown(ButtonType code);
		b8 hasFocus(ButtonType code);
		
	}
}
}
}


#endif // !MUSIC_PLAYER_UI_INPUT_H
