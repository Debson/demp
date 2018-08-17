#pragma once
#ifndef MD_SYSTEM_APPLICATION_INPUT_H
#define  MD_SYSTEM_APPLICATION_INPUT_H

#include "../app/input.h"

namespace mdEngine
{
	void OnPressKey(const App::KeyCode& key);

	void OnReleaseKey(const App::KeyCode& key);

	void StartNewFrame(void);

	void UpdateMousePosition(s32 mouseX, s32 mouseY);

	void UpdateScrollPosition(s32 scrollX, s32 scrollY);

	void UpdateKeyState(const u8* state);

	void UpdateMouseState(const u32 state);
}

#endif // !MD_SYSTEM_APPLICATION_INPUT_H
