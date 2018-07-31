#include "application_window.h"

#include "realtime_system_application.h"

namespace mdEngine
{
	namespace App
	{

		s32 x, y;
		s32 startX, startY;

	}

	App::WindowProperties::WindowProperties() :
						mWindowWidth(400), // 400
						mWindowHeight(300), // 300
						mWindowPositionX(100),
						mWindowPositionY(100),
						mWindowMode(WindowMode::Windowed),
						mVerticalSync(true)
	{ }

	void App::ProcessMovable(MP::UI::Movable* bar)
	{
		if (Input::IsKeyDown(KeyCode::MouseLeft))
		{
			int currentMouseX = 0, currentMouseY = 0;
			Input::GetGlobalMousePosition(&currentMouseX, &currentMouseY);

			int currentWinX, currentWinY;
			GetWindowPos(&currentWinX, &currentWinY);

			int insideX, insideY;
			Input::GetMousePosition(&insideX, &insideY);

			int deltaX = x - currentMouseX;
			int deltaY = y - currentMouseY;

			//std::cout << "DeltaX: " << deltaX << "DeltaT: " << deltaY << std::endl;
			int newWX = startX - deltaX;
			int newWY = startY - deltaY;

			//f32 scaleX, scaleY;
			//GetWindowScale(&scaleX, &scaleY);

			f32 xL = bar->pos.x;
			f32 xR = bar->pos.x + bar->size.x;
			f32 yU = bar->pos.y;
			f32 yD = bar->pos.y + bar->size.y;

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

	void App::ProcessButtons(MP::UI::Button* button)
	{
		int mouseX, mouseY;
		Input::GetMousePosition(&mouseX, &mouseY);
		/*f32 scaleX, scaleY;
		GetWindowScale(&scaleX, &scaleY);

		f32 xL = (float)button->xL * scaleX;
		f32 xR = (float)button->xR * scaleX;
		f32 yU = (float)button->yU * scaleY;
		f32 yD = (float)button->yD * scaleY;

		bool inside = mouseX > xL && mouseX < xR &&
			mouseY > yU && mouseY < yD;*/

		bool inside = mouseX > button->pos.x && mouseX < (button->pos.x + button->size.x) &&
					  mouseY > button->pos.y && mouseY < (button->pos.y + button->size.y); 

		if (inside && Input::IsKeyPressed(KeyCode::MouseLeft))
			button->isPressed = true;
		else
			button->isPressed = false;

		if (inside && Input::IsKeyDown(KeyCode::MouseLeft))
			button->isDown = true;
		else
			button->isDown = false;

		if (inside)
			button->hasFocus = true;
		else
			button->hasFocus = false;

		button->isReleased = !(button->isPressed || button->isDown);
	}
	
}
