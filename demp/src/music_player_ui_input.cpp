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
			auto item = std::find_if(mdButtonsContainer.begin(), mdButtonsContainer.end(),
				[&](std::pair<ButtonType, Button*> const & ref) { return ref.first == code; });

			return item == mdButtonsContainer.end() ? false : item->second->isPressed;
		}

		b8 Input::isButtonReleased(ButtonType code)
		{
			auto item = std::find_if(mdButtonsContainer.begin(), mdButtonsContainer.end(),
				[&](std::pair<ButtonType, Button*> const & ref) { return ref.first == code; });

			return item == mdButtonsContainer.end() ? false : item->second->isReleased;
		}

		b8 Input::isButtonDown(ButtonType code)
		{
			auto item = std::find_if(mdButtonsContainer.begin(), mdButtonsContainer.end(),
				[&](std::pair<ButtonType, Button*> const & ref) { return ref.first == code; });

			return item == mdButtonsContainer.end() ? false : item->second->isDown;
		}

		b8 Input::hasFocus(ButtonType code)
		{

			auto item = std::find_if(mdButtonsContainer.begin(), mdButtonsContainer.end(),
				[&](std::pair<ButtonType, Button*> const & ref) { return ref.first == code; });

			return item == mdButtonsContainer.end() ? false : item->second->hasFocus;
		}

		b8 Input::hasFocusTillRelease(ButtonType code)
		{
			auto item = std::find_if(mdButtonsContainer.begin(), mdButtonsContainer.end(),
				[&](std::pair<ButtonType, Button*> const & ref) { return ref.first == code; });

			return item == mdButtonsContainer.end() ? false : item->second->hasFocusTillRelease;
		}

		glm::vec2 Input::GetButtonMousePos(ButtonType code)
		{
			auto item = std::find_if(mdButtonsContainer.begin(), mdButtonsContainer.end(),
				[&](std::pair<ButtonType, Button*> const & ref) { return ref.first == code; });

			glm::vec2 mousePos(-1.f);
			if (item != mdButtonsContainer.end())
			{
				mousePos = item->second->mMousePos;
				if (mousePos.x < item->second->mPos.x)
					mousePos.x = item->second->mPos.x;
			}

			return mousePos;
		}
	}

}
}