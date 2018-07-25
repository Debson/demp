#include "system_application_input.h"


namespace mdEngine
{
	static bool mdNeedsInitializaiotn(true);
	static bool mdCurrentKeyStatus[mdEngine::Application::KeyCode::count];
	static bool mdPreviousKeyStatus[mdEngine::Application::KeyCode::count];

	static s32 mdCurrentMouseX = 0;
	static s32 mdCurrentMouseY = 0;
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
	void OnPressKey(const Application::KeyCode& key)
	{
		mdCurrentKeyStatus[key] = true;
	}

	void OnReleaseKey(const Application::KeyCode& key)
	{
		mdCurrentKeyStatus[key] = false;
	}

	void StartNewFrame(void)
	{
		if (mdNeedsInitializaiotn == true)
		{
			mdNeedsInitializaiotn = false;
			for (u8 keyIndex = 0; keyIndex < Application::KeyCode::count; ++keyIndex)
			{
				mdCurrentKeyStatus[keyIndex] = false;
			}
		}

		for (u8 keyIndex = 0; keyIndex < Application::KeyCode::count; ++keyIndex)
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
		for (u8 keyIndex = 0; keyIndex < Application::KeyCode::count; ++keyIndex)
		{
			mdCurrentKeyStatus[keyIndex] = state[keyIndex];
		}
	}

	void UpdateMousePosition(s32 mouseX, s32 mouseY)
	{
		mdCurrentMouseX = mouseX;
		mdCurrentMouseY = mouseY;;
	}

	void UpdateScrollPosition(s32 scrollX, s32 scrollY)
	{
		mdCurrentScrollX = scrollX;
		mdCurrentScrollY = scrollY;
		mdIsMouseScrollActive = true;
	}

	/* need to handle mouse input as well */

	/******************** KEYBOARD ********************/
	b8 Application::Input::IsKeyPressed(const KeyCode& key)
	{
		return (mdCurrentKeyStatus[key] == true && mdPreviousKeyStatus[key] == false) ? true : false;
	}

	b8 Application::Input::IsKeyReleased(const KeyCode& key)
	{
		return (mdCurrentKeyStatus[key] == false && mdPreviousKeyStatus[key] == true) ? true : false;
	}

	b8 Application::Input::IsKeyDown(const KeyCode& key)
	{
		return mdCurrentKeyStatus[key];
	}

	/******************** MOUSE ********************/

	void Application::Input::GetMousePosition(s32* mouseX, s32* mouseY)
	{
		*mouseX = mdCurrentMouseX;
		*mouseY = mdCurrentMouseY;
	}


	b8 Application::Input::IsMouseActive()
	{
		return ((mdCurrentMouseX - mdPreviousMouseX) != 0 ||
				(mdCurrentMouseY - mdPreviousMouseY) != 0) ? true : false;
	}


	/******************** MOUSE SCROLL********************/
	void Application::Input::GetMouseScrollPosition(s32* scrollX, s32* scrollY)
	{
		*scrollX = mdCurrentScrollX;
		*scrollY = mdCurrentScrollY;
	}

	b8 Application::Input::GetMouseScrollMovement(s32* scrollX, s32* scrollY)
	{
		*scrollX = mdCurrentScrollX - mdPreviousScrollX;
		*scrollY = mdCurrentScrollY - mdPreviousScrollY;
		return (*scrollX != 0 || *scrollY != 0) ? true : false;
	}

	b8 Application::Input::IsScrollForwardActive()
	{
		return (mdIsMouseScrollActive == true && mdCurrentScrollY > 0) ? true : false;
	}

	b8 Application::Input::IsScrollBackwardActive()
	{
		return (mdIsMouseScrollActive == true && mdCurrentScrollY < 0) ? true : false;
	}

	b8 Application::Input::IsScrollActive()
	{
		return mdIsMouseScrollActive == true ? true : false;
	}


}
