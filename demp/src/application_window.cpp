#include "application_window.h"

#include "realtime_system_application.h"

namespace mdEngine
{
	namespace App
	{

		WindowProperties::WindowProperties() :
			mWindowWidth(500),
			mWindowHeight(400),
			mWindowPositionX(100),
			mWindowPositionY(100),
			mWindowMode(WindowMode::Windowed),
			mVerticalSync(true)
		{ }

		int x, y;
		int startX, startY;

		void WindowMovableBar(MP::UI::Movable& bar)
		{
			if (Input::IsKeyDown(KeyCode::MouseLeft))
			{
				int currentMouseX, currentMouseY;
				int currentWinX, currentWinY;
				int insideX, insideY;
				Input::GetGlobalMousePosition(&currentMouseX, &currentMouseY);
				Input::GetMousePosition(&insideX, &insideY);
				GetWindowPos(&currentWinX, &currentWinY);
				int deltaX = x - currentMouseX;
				int deltaY = y - currentMouseY;

				//std::cout << "DeltaX: " << deltaX << "DeltaT: " << deltaY << std::endl;
				int newWX = startX - deltaX;
				int newWY = startY - deltaY;

				f32 scaleX, scaleY;
				GetWindowScale(&scaleX, &scaleY);

				f32 xL = bar.xL * scaleX;
				f32 xR = bar.xR * scaleX;
				f32 yU = bar.yU * scaleY;
				f32 yD = bar.yD * scaleY;

				if(insideX > xL && insideX < xR && 
				   insideY > yU && insideY < yD)
					SetWindowPos(newWX, newWY);
			}
			else
			{
				Input::GetGlobalMousePosition(&x, &y);
				GetWindowPos(&startX, &startY);
			}
		}

		void ProcessButtons(MP::UI::Button* button)
		{
			int mouseX, mouseY;
			Input::GetMousePosition(&mouseX, &mouseY);
			f32 scaleX, scaleY;
			GetWindowScale(&scaleX, &scaleY);

			f32 xL = button->xL * scaleX;
			f32 xR = button->xR * scaleX;
			f32 yU = button->yU * scaleY;
			f32 yD = button->yD * scaleY;

			bool inside = mouseX > xL && mouseX < xR &&
						  mouseY > yU && mouseY < yD;

			if (inside && Input::IsKeyPressed(KeyCode::MouseLeft))
				button->isPressed = true;
			else
				button->isPressed = false;

			if (inside && Input::IsKeyDown(KeyCode::MouseLeft))
				button->isDown = true;
			else
				button->isDown = false;
		}
	}
}
