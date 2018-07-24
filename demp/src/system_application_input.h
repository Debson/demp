#pragma once
#ifndef MD_SYSTEM_APPLICATION_INPUT_H
#define  MD_SYSTEM_APPLICATION_INPUT_H

#include "input.h"

namespace mdEngine
{
	void OnPressKey(const Application::KeyCode& key);

	void OnReleaseKey(const Application::KeyCode& key);

	void StartNewFrame(void);

	void UpdateMousePosition(int mouseX, int mouseY);

	void UpdateKeyState(const u8* state);
}

#endif // !MD_SYSTEM_APPLICATION_INPUT_H
