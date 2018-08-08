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
		b8 buttonExtraState(false);
	}

	b8 UI::Input::isButtonPressed(ButtonType code)
	{
		auto item = std::find_if(mdButtonsContainer.begin(), mdButtonsContainer.end(),
			[&](std::pair<ButtonType, Button*> const & ref) { return ref.first == code; });

		return item == mdButtonsContainer.end() ? false : item->second->isPressed;
	}

	b8 UI::Input::isButtonReleased(ButtonType code)
	{
		auto item = std::find_if(mdButtonsContainer.begin(), mdButtonsContainer.end(),
			[&](std::pair<ButtonType, Button*> const & ref) { return ref.first == code; });

		return item == mdButtonsContainer.end() ? false : item->second->isReleased;
	}

	b8 UI::Input::isButtonDown(ButtonType code)
	{
		auto item = std::find_if(mdButtonsContainer.begin(), mdButtonsContainer.end(),
			[&](std::pair<ButtonType, Button*> const & ref) { return ref.first == code; });

		return item == mdButtonsContainer.end() ? false : item->second->isDown;
	}

	b8 UI::Input::hasFocus(ButtonType code)
	{

		auto item = std::find_if(mdButtonsContainer.begin(), mdButtonsContainer.end(),
			[&](std::pair<ButtonType, Button*> const & ref) { return ref.first == code; });

		return item == mdButtonsContainer.end() ? false : item->second->hasFocus;
	}

	b8 UI::Input::hasFocusTillRelease(ButtonType code)
	{
		auto item = std::find_if(mdButtonsContainer.begin(), mdButtonsContainer.end(),
			[&](std::pair<ButtonType, Button*> const & ref) { return ref.first == code; });

		return item == mdButtonsContainer.end() ? false : item->second->hasFocusTillRelease;
	}


	b8 UI::Input::GetButtonExtraState()
	{
		return buttonExtraState;
	}

	void UI::Input::SetButtonExtraState(b8 state)
	{
		buttonExtraState = state;
	}

	glm::vec2 UI::Input::GetButtonMousePos(ButtonType code)
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