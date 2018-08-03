#include "application_window.h"

#include "realtime_system_application.h"
#include "music_player.h"

namespace mdEngine
{
	namespace App
	{
		s32 globalMouseX, globalMouseY;
		s32 currentMouseX = 0, currentMouseY = 0;
		s32 prevMouseX = 0, prevMouseY = 0;
		s32 startX, startY;
		s32 winSizeBeforeResize = 0;
		s32 prevWinSize = 0;
		s32 currWinSize = 0;

		b8 wasInsideMovable(false);
		b8 wasInsideResizable(false);
		b8 firstMove(false);

	}

	App::WindowProperties::WindowProperties() :
						mWindowWidth(500), // 400
						mWindowHeight(700), // 300
						mApplicationHeight(700),
						mWindowPositionX(600),
						mWindowPositionY(100),
						mWindowMode(WindowMode::Windowed),
						mWindowEvent(WindowEvent::kFocus),
						mVerticalSync(true)
	{ }

	void App::ProcessMovable(MP::UI::Movable* bar)
	{
		if (Input::IsKeyDown(KeyCode::MouseLeft))
		{
			int currentMouseX = 0, currentMouseY = 0;
			Input::GetGlobalMousePosition(&currentMouseX, &currentMouseY);

			int currentWinX, currentWinY;
			Window::GetWindowPos(&currentWinX, &currentWinY);

			int insideX, insideY;
			Input::GetMousePosition(&insideX, &insideY);

			int deltaX = globalMouseX - currentMouseX;
			int deltaY = globalMouseY - currentMouseY;

			int newWX = startX - deltaX;
			int newWY = startY - deltaY;

			//f32 scaleX, scaleY;
			//GetWindowScale(&scaleX, &scaleY);

			f32 xL = bar->mPos.x;
			f32 xR = bar->mPos.x + bar->mSize.x;
			f32 yU = bar->mPos.y;
			f32 yD = bar->mPos.y + bar->mSize.y;

			bool inside = insideX > xL && insideX < xR && insideY > yU && insideY < yD;

			if (inside)
				wasInsideMovable = true;

			if(wasInsideMovable && Window::windowProperties.mWindowEvent != App::WindowEvent::kResize)
				Window::SetWindowPos(newWX, newWY);
		}
		else
		{
			// TODO: dont call it every frame?
			wasInsideMovable = false;
			Input::GetGlobalMousePosition(&globalMouseX, &globalMouseY);
			Window::GetWindowPos(&startX, &startY);
		}
	}


	void App::ProcessButton(MP::UI::Button* button)
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

		bool inside = mouseX > button->mPos.x && mouseX < (button->mPos.x + button->mSize.x) &&
					  mouseY > button->mPos.y && mouseY < (button->mPos.y + button->mSize.y); 

		if (inside)
			button->hasFocus = true;
		else
			button->hasFocus = false;

		if (inside)
			button->hasFocusTillRelease = true;
		else if(!Input::IsKeyDown(KeyCode::MouseLeft))
			button->hasFocusTillRelease = false;

		if (inside && Input::IsKeyPressed(KeyCode::MouseLeft))
			button->isPressed = true;
		else
			button->isPressed = false;

		if (inside && Input::IsKeyDown(KeyCode::MouseLeft))
		{
			button->isDown = true;
			button->mMousePos.x = mouseX;
		}
		else
		{
			button->isDown = false;
			button->mMousePos.y = mouseY;
		}


		button->isReleased = !(button->isPressed || button->isDown);
	}


	void App::ProcessResizable(MP::UI::Resizable* bar)
	{
		//std::cout << winSizeBeforeResize << std::endl;
		if (Input::IsKeyDown(KeyCode::MouseLeft))
		{
			prevWinSize = currWinSize;
			currWinSize = winSizeBeforeResize;


			prevMouseY = currentMouseY;
			Input::GetGlobalMousePosition(&currentMouseX, &currentMouseY);

			s32 mouseX, mouseY;

			Input::GetMousePosition(&mouseX, &mouseY);

			s32 deltaY = 0;
			if (firstMove == true)
				deltaY = currentMouseY - prevMouseY;


			b8 inside = mouseX > bar->pos.x && mouseX < (bar->pos.x + bar->size.x) &&
				mouseY > bar->pos.y && mouseY < (bar->pos.y + bar->size.y);

			if (inside)
			{
				wasInsideResizable = true;

			}
			//std::cout << bar->pos.y << std::endl;

			if (wasInsideResizable)
			{
				if (MP::UI::Data::_MIN_PLAYER_SIZE.y < winSizeBeforeResize + deltaY &&
					MP::UI::Data::_MIN_PLAYER_SIZE.y < mouseY)
				{
					Window::windowProperties.mWindowEvent = WindowEvent::kResize;
					MP::musicPlayerState = MP::MusicPlayerState::kChanged;

					Window::windowProperties.mApplicationHeight = winSizeBeforeResize + deltaY;
					winSizeBeforeResize += deltaY;

					firstMove = true;
					bar->pos = glm::vec2(0, winSizeBeforeResize + deltaY - bar->size.y);
					//Window::SetWindowSize(Window::windowProperties.mWindowWidth, winSizeBeforeResize + deltaY);
				}
			}
		}
		else
		{
			Window::windowProperties.mWindowEvent = WindowEvent::kFocus;

			winSizeBeforeResize = Window::windowProperties.mApplicationHeight; 
			//std::cout << winSizeBeforeResize << std::endl;
			wasInsideResizable = false;
			firstMove = false;
			Input::GetGlobalMousePosition(&globalMouseX, &globalMouseY);
		}

	}
	
}
