#include "system_application_input.h"


namespace mdEngine
{
	static bool mdiNeedsInitializaiotn(true);
	static bool mdiCurrentKeyStauts[mdEngine::Application::KeyCode::count];
	static bool tbiPreviousKeyStatus[mdEngine::Application::KeyCode::count];
}

namespace mdEngine
{ 
	void OnPressKey(const Application::KeyCode& key)
	{
		mdiCurrentKeyStauts[key] = true;
	}

	void OnReleaseKey(const Application::KeyCode& key)
	{
		mdiCurrentKeyStauts[key] = false;
	}

	void StartNewFrame(void)
	{
		if (mdiNeedsInitializaiotn == true)
		{
			mdiNeedsInitializaiotn = false;
			for (u32 keyIndex = 0; keyIndex < Application::KeyCode::count; ++keyIndex)
			{
				mdiCurrentKeyStauts[keyIndex] = false;
			}
		}

		for (u32 keyIndex = 0; keyIndex < Application::KeyCode::count; ++keyIndex)
		{
			tbiPreviousKeyStatus[keyIndex] = mdiCurrentKeyStauts[keyIndex];
		}

	}

	void UpdateKeyState(const u8* state)
	{
		for (u8 keyIndex = 0; keyIndex < Application::KeyCode::count; ++keyIndex)
		{
			mdiCurrentKeyStauts[keyIndex] = state[keyIndex];
		}
	}

	/* need to handle mouse input as well */

	b8 Application::Input::IsKeyPressed(const KeyCode& key)
	{
		return (mdiCurrentKeyStauts[key] == true && tbiPreviousKeyStatus[key] == false) ? true : false;
	}

	b8 Application::Input::IsKeyReleased(const KeyCode& key)
	{
		return (mdiCurrentKeyStauts[key] == false && tbiPreviousKeyStatus[key] == true) ? true : false;
	}

	b8 Application::Input::IsKeyDown(const KeyCode& key)
	{
		return mdiCurrentKeyStauts[key];
	}
}
