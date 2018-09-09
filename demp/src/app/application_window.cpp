#include "application_window.h"

#include "realtime_system_application.h"
#include "../player/music_player.h"
#include "../settings/music_player_settings.h"
#include "../settings/music_player_settings.h"
#include "../player/music_player_state.h"

namespace mdEngine
{
	namespace App
	{
		s32 globalMouseX, globalMouseY;
		s32 currentMouseX, currentMouseY;
		s32 startX, startY;
		s32 winSizeBeforeResize;

		s32 boundLow;
		s32 boundHigh;
		b8 changeBarSize(false);
		b8 checkBounds(false);

		b8 wasInsideMovable(false);
		b8 wasInsideResizable(false);
	}

	App::WindowProperties::WindowProperties() :
						mWindowWidth(500), // 400
						mWindowHeight(700), // 300
						mApplicationHeight(700),
						mStartApplicationHeight(700),
						mWindowPositionX(600),
						mWindowPositionY(100),
						mWindowMode(WindowMode::Windowed),
						mVerticalSync(true)
	{ }

	void App::ProcessMovable(Interface::Movable* bar)
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

				f32 xL = bar->m_Pos.x;
				f32 xR = bar->m_Pos.x + bar->m_Size.x;
				f32 yU = bar->m_Pos.y;
				f32 yD = bar->m_Pos.y + bar->m_Size.y;

				bool inside = insideX > xL && insideX < xR && insideY > yU && insideY < yD;

				if (inside &&
					Input::IsKeyPressed(KeyCode::MouseLeft) &&
					!MP::UI::Input::GetButtonExtraState())
					wasInsideMovable = true;

				if (wasInsideMovable && State::CheckState(State::Window::Resized) == false)
				{
					State::SetState(State::Window::PositionChanged);
					Window::SetWindowPos(newWX, newWY);
				}
			}
			else
			{
				// TODO: dont call it every frame?
				wasInsideMovable = false;
				Input::GetGlobalMousePosition(&globalMouseX, &globalMouseY);
				Window::GetWindowPos(&startX, &startY);
			}
		
	}

	void App::ProcessButton(Interface::Button* button)
	{
		// Check if button positions is valid
		if (button->GetButtonPos() == glm::vec2(POS_INVALID))
		{
			return;
		}


		s32 mouseX, mouseY;

		s32 x, y;
		s32 winX, winY;
		Input::GetGlobalMousePosition(&x, &y);
		Window::GetWindowPos(&winX, &winY);
		
		if (checkBounds == true)
		{
			mouseX = x - winX;
			mouseY = y - winY;
		}
		else
		{
			Input::GetMousePosition(&mouseX, &mouseY);
		}

		bool check(true);
		if (checkBounds == true)
		{
			check = mouseY > boundLow && mouseY < boundHigh;
		}
		if (checkBounds == true)
		{
			button->topHasFocus = mouseX > button->GetButtonPos().x && mouseX < (button->GetButtonPos().x + button->GetButtonSize().x) &&
								  mouseY > button->GetButtonPos().y && mouseY < (button->GetButtonPos().y + button->GetButtonSize().y / 2.f);

			button->bottomHasFocus = mouseX > button->GetButtonPos().x && mouseX < (button->GetButtonPos().x + button->GetButtonSize().x) &&
									 mouseY > (button->GetButtonPos().y + button->GetButtonSize().y / 2.f) && 
									 mouseY < (button->GetButtonPos().y + button->GetButtonSize().y);

			button->topHasFocus = button->topHasFocus && check;
			button->bottomHasFocus = button->bottomHasFocus && check;
		}


		bool inside = mouseX > button->GetButtonPos().x && mouseX < (button->GetButtonPos().x + button->GetButtonSize().x) &&
					  mouseY > button->GetButtonPos().y && mouseY < (button->GetButtonPos().y + button->GetButtonSize().y);


		inside = inside && check;


		if (inside && !button->wasDown)
			button->hasFocus = true;
		else
		{
			button->hasFocus = false;
			button->wasDown = App::Input::IsKeyDown(App::KeyCode::MouseLeft);
		}

		if (inside)
			button->hasFocusTillRelease = true;
		else if (!Input::IsKeyDown(KeyCode::MouseLeft))
			button->hasFocusTillRelease = false;

		if (inside && Input::IsKeyPressed(KeyCode::MouseLeft))
			button->isPressed = true;
		else
			button->isPressed = false;

		if (inside && Input::IsKeyDown(KeyCode::MouseLeft))
		{
			button->isDown = true;
			button->GetInButtonMousePos().x = mouseX;
		}
		else
		{
			button->isDown = false;
			button->GetInButtonMousePos().y = mouseY;
		}


		button->isReleased = !(button->isPressed || button->isDown);
	}

	void App::ProcessResizable(Interface::Resizable* bar)
	{
		if (Input::IsKeyDown(KeyCode::MouseLeft))
		{
			s32 mouseX, mouseY;
			Input::GetMousePosition(&mouseX, &mouseY);

			int relX = 0, relY = 0;
			Input::GetRelavtiveMousePosition(&relX, &relY);


			b8 inside = mouseX > bar->m_Pos.x && mouseX < (bar->m_Pos.x + bar->m_Size.x) &&
				mouseY > bar->m_Pos.y && mouseY < (bar->m_Pos.y + bar->m_Size.y);

			if (inside == true &&
				Input::IsKeyPressed(KeyCode::MouseLeft) == true)
			{
				wasInsideResizable = true;
			}

			
			if (wasInsideResizable == true && 
				MP::UI::Input::GetButtonExtraState() == false)
			{
				if(relY != 0)
					State::SetState(State::Window::Resized);
				Window::windowProperties.mDeltaHeightResize = relY;

				Window::windowProperties.mApplicationHeight = winSizeBeforeResize + relY;
				winSizeBeforeResize += relY;

				bar->m_Pos = glm::vec2(0, winSizeBeforeResize + relY - bar->m_Size.y);

				if (Window::windowProperties.mApplicationHeight - bar->m_Size.y < MP::Data::_MIN_PLAYER_SIZE.y)
					Window::windowProperties.mApplicationHeight = MP::Data::_MIN_PLAYER_SIZE.y + bar->m_Size.y;
				if (bar->m_Pos.y < MP::Data::_MIN_PLAYER_SIZE.y)
					bar->m_Pos.y = MP::Data::_MIN_PLAYER_SIZE.y;
			}
		}
		else
		{
			State::ResetState(State::Window::Resized);
			winSizeBeforeResize = Window::windowProperties.mApplicationHeight;
			wasInsideResizable = false;
		}
	}

	void App::SetButtonCheckBounds(s32 low, s32 high, b8 val)
	{
		checkBounds = val;
		boundLow = low;
		boundHigh = high;
	}
	
}
