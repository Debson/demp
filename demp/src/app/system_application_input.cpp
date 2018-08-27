#include "system_application_input.h"


namespace mdEngine
{
	static bool mdNeedsInitializaiotn(true);
	static bool mdCurrentKeyStatus[mdEngine::App::KeyCode::count];
	static bool mdPreviousKeyStatus[mdEngine::App::KeyCode::count];

	static s32 mdCurrentMouseX = 0;
	static s32 mdCurrentMouseY = 0;
	static s32 mdRelativeMouseX = 0;
	static s32 mdRelativeMouseY = 0;
	static s32 mdPreviousMouseX = 0;
	static s32 mdPreviousMouseY = 0;

	static bool mdIsMouseScrollActive(false);
	static s32 mdCurrentScrollX		= 0;
	static s32 mdCurrentScrollY		= 0;
	static s32 mdPreviousScrollX	= 0;
	static s32 mdPreviousScrollY	= 0;
	
}

namespace mdEngine
{ 
	void OnPressKey(const App::KeyCode& key)
	{
		mdCurrentKeyStatus[key] = true;
	}

	void OnReleaseKey(const App::KeyCode& key)
	{
		mdCurrentKeyStatus[key] = false;
	}

	void StartNewFrame(void)
	{
		if (mdNeedsInitializaiotn == true)
		{
			mdNeedsInitializaiotn = false;
			for (u8 keyIndex = 0; keyIndex < App::KeyCode::count; ++keyIndex)
			{
				mdCurrentKeyStatus[keyIndex] = false;
			}
		}

		for (u8 keyIndex = 0; keyIndex < App::KeyCode::count; ++keyIndex)
		{
			mdPreviousKeyStatus[keyIndex] = mdCurrentKeyStatus[keyIndex];
		}

		mdPreviousMouseX = mdCurrentMouseX;
		mdPreviousMouseY = mdCurrentMouseY;

		mdPreviousScrollX = mdCurrentScrollX;
		mdPreviousScrollY = mdCurrentScrollY;
		mdIsMouseScrollActive = false;

	}

	void UpdateKeyState(const u8* state)
	{
		for (u8 keyIndex = 0; keyIndex < App::KeyCode::count; ++keyIndex)
		{
			mdCurrentKeyStatus[keyIndex] = state[keyIndex];
		}
	}

	void UpdateMouseState(const u32 state)
	{
		mdCurrentKeyStatus[App::KeyCode::MouseLeft] = state & SDL_BUTTON(SDL_BUTTON_LEFT);
		mdCurrentKeyStatus[App::KeyCode::MouseRight] = state & SDL_BUTTON(SDL_BUTTON_RIGHT);
		mdCurrentKeyStatus[App::KeyCode::MouseMiddle] = state & SDL_BUTTON(SDL_BUTTON_MIDDLE);
	}

	void UpdateMousePosition(s32 mouseX, s32 mouseY)
	{
		mdCurrentMouseX = mouseX;
		mdCurrentMouseY = mouseY;;
	}

	void UpdateRelativeMousePosition()
	{
		SDL_GetRelativeMouseState(&mdRelativeMouseX, &mdRelativeMouseY);
	}

	void UpdateScrollPosition(s32 scrollX, s32 scrollY)
	{
		mdCurrentScrollX = scrollX;
		mdCurrentScrollY = scrollY;
		mdIsMouseScrollActive = true;
	}

	/* need to handle mouse input as well */

	/******************** KEYBOARD ********************/
	b8 App::Input::IsKeyPressed(const KeyCode& key)
	{
		return (mdCurrentKeyStatus[key] == true && mdPreviousKeyStatus[key] == false) ? true : false;
	}

	b8 App::Input::IsKeyReleased(const KeyCode& key)
	{
		return (mdCurrentKeyStatus[key] == false && mdPreviousKeyStatus[key] == true) ? true : false;
	}

	b8 App::Input::IsKeyDown(const KeyCode& key)
	{
		return mdCurrentKeyStatus[key];
	}

	/******************** MOUSE ********************/

	void App::Input::GetMousePosition(s32* mouseX, s32* mouseY)
	{
		*mouseX = mdCurrentMouseX;
		*mouseY = mdCurrentMouseY;
	}

	void App::Input::GetGlobalMousePosition(s32* mouseX, s32* mouseY)
	{
		SDL_GetGlobalMouseState(mouseX, mouseY);
	}

	void App::Input::GetRelavtiveMousePosition(s32* mouseX, s32* mouseY)
	{
		*mouseX = mdRelativeMouseX;
		*mouseY = mdRelativeMouseY;
	}


	b8 App::Input::IsMouseActive()
	{
		return ((mdCurrentMouseX - mdPreviousMouseX) != 0 ||
				(mdCurrentMouseY - mdPreviousMouseY) != 0) ? true : false;
	}


	/******************** MOUSE SCROLL********************/
	void App::Input::GetMouseScrollPosition(s32* scrollX, s32* scrollY)
	{
		*scrollX = mdCurrentScrollX;
		*scrollY = mdCurrentScrollY;
	}

	b8 App::Input::GetMouseScrollMovement(s32* scrollX, s32* scrollY)
	{
		*scrollX = mdCurrentScrollX - mdPreviousScrollX;
		*scrollY = mdCurrentScrollY - mdPreviousScrollY;
		return (*scrollX != 0 || *scrollY != 0) ? true : false;
	}

	b8 App::Input::IsScrollForwardActive()
	{
		return (mdIsMouseScrollActive == true && mdCurrentScrollY > 0) ? true : false;
	}

	b8 App::Input::IsScrollBackwardActive()
	{
		return (mdIsMouseScrollActive == true && mdCurrentScrollY < 0) ? true : false;
	}

	b8 App::Input::IsScrollActive()
	{
		return mdIsMouseScrollActive == true ? true : false;
	}


}
