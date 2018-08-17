#ifndef MUSIC_PLAYER_UI_INPUT_H
#define MUSIC_PLAYER_UI_INPUT_H

#include <glm.hpp>

#include "../utility/md_types.h"

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
					None,
					Exit,
					Minimize,
					StayOnTop,
					Volume,
					Play,
					Stop,
					Next,
					Previous,
					Shuffle,
					Repeat,
					Playlist,
					PlaylistAddFile,
					PlaylistAddFileTextBox,
					PlaylistAddFolder,
					SliderVolume,
					SliderMusic,
					SliderPlaylist,
					PlaylistItem,

					count
				};

				b8 isButtonPressed(ButtonType code);
				b8 isButtonReleased(ButtonType code);
				b8 isButtonDown(ButtonType code);
				b8 hasFocus(ButtonType code);
				b8 hasFocusTillRelease(ButtonType code);

				b8 GetButtonExtraState();
				void SetButtonExtraState(b8 state);
				glm::vec2 GetButtonMousePos(ButtonType code);

			}
		}
	}
}


#endif // !MUSIC_PLAYER_UI_INPUT_H