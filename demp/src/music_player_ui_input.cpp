#include "music_player_ui_input.h"

#include <iostream>
#include <algorithm>

#include "music_player_ui.h"


namespace mdEngine
{
namespace MP
{
	namespace UI
	{

		b8 Input::isButtonPressed(ButtonType code)
		{
			auto item = std::find_if(mdButtonContainer.begin(), mdButtonContainer.end(),
				[&](std::pair<ButtonType, Button*> const & ref) { return ref.first == code; });

			return item->second->isPressed;
		}

		b8 Input::isButtonReleased(ButtonType code)
		{
			auto item = std::find_if(mdButtonContainer.begin(), mdButtonContainer.end(),
				[&](std::pair<ButtonType, Button*> const & ref) { return ref.first == code; });

			return item->second->isReleased;
		}

		b8 Input::isButtonDown(ButtonType code)
		{
			auto item = std::find_if(mdButtonContainer.begin(), mdButtonContainer.end(),
				[&](std::pair<ButtonType, Button*> const & ref) { return ref.first == code; });

			return item->second->isDown;
		}

		b8 Input::hasFocus(ButtonType code)
		{
			auto item = std::find_if(mdButtonContainer.begin(), mdButtonContainer.end(),
				[&](std::pair<ButtonType, Button*> const & ref) { return ref.first == code; });

			return item->second->hasFocus;
		}
	}

}
}