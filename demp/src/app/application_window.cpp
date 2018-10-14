#include "application_window.h"

#include "realtime_system_application.h"
#include "../player/music_player.h"
#include "../graphics/graphics.h"
#include "../settings/music_player_settings.h"
#include "../settings/music_player_string.h"
#include "../player/music_player_state.h"
#include "../ui/music_player_ui.h"
#include "../utility/md_parser.h"

namespace mdEngine
{
	namespace App
	{
		s32 globalMouseX, globalMouseY;
		s32 globalMouseXRes, globalMouseYRes;
		s32 currentMouseX, currentMouseY;
		s32 startX, startY;
		s32 startXRes, startYRes;

		s32 winSizeBeforeResizeBottom;
		s32 winSizeBeforeResizeTop;
		s32 winPosBeforeResize;

		s32 newWY;
		s32 newSizeY;

		s32 boundLow;
		s32 boundHigh;
		b8 changeBarSize(false);
		b8 checkBounds(false);

		b8 wasInsideMovable(false);
		b8 wasInsideResizableBottom(false);
		b8 wasInsideResizableTop(false);
		b8 resizeFinished(false);

	}

	App::WindowProperties::WindowProperties() :
						mWindowWidth(500), // 400
						mWindowHeight(700), // 300
						mApplicationHeight(700),
						mApplicationWidth(500),
						mStartApplicationHeight(700),
						mWindowPositionX(600),
						mWindowPositionY(100),
						mWindowMode(WindowMode::Windowed),
						mVerticalSync(true)
	{ }

	void App::InitializeConfig()
	{
		Window::windowProperties.mApplicationHeight = Parser::GetInt(Strings::_SETTINGS_FILE, Strings::_APP_HEIGHT);
		Window::windowProperties.mStartApplicationHeight = Window::windowProperties.mApplicationHeight;

		if (Window::windowProperties.mApplicationHeight < MP::Data::_MIN_PLAYER_SIZE.y)
			Window::windowProperties.mApplicationHeight = MP::Data::_MIN_PLAYER_SIZE.y + 20.f;
		if (Window::windowProperties.mApplicationHeight > Window::windowProperties.mWindowHeight)
			Window::windowProperties.mApplicationHeight = Window::windowProperties.mWindowHeight;
	}

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

				if (insideX > MP::Data::_UI_BUTTONS_BACKGROUND_RIGHT_POS.x &&
					insideX < MP::Data::_UI_BUTTONS_BACKGROUND_RIGHT_POS.x + MP::Data::_UI_BUTTONS_BACKGROUND_RIGHT_SIZE.x &&
					insideY > MP::Data::_UI_BUTTONS_BACKGROUND_RIGHT_POS.y &&
					insideY < MP::Data::_UI_BUTTONS_BACKGROUND_RIGHT_POS.y + MP::Data::_UI_BUTTONS_BACKGROUND_RIGHT_SIZE.y)
				{
					inside = false;
				}

				for (auto & i : MP::UI::mdButtonsContainer)
				{
					if (insideX > i.second->GetButtonPos().x &&
						insideX < i.second->GetButtonPos().x + i.second->GetButtonSize().x &&
						insideY > i.second->GetButtonPos().y &&
						insideY < i.second->GetButtonPos().y + i.second->GetButtonSize().y)
					{
						inside = false;
					}
				}


				if (inside &&
					Input::IsKeyPressed(KeyCode::MouseLeft) == true &&
					MP::UI::Input::isButtonDown(MP::UI::Input::ButtonType::Minimize) == false &&
					MP::UI::Input::isButtonDown(MP::UI::Input::ButtonType::Exit) == false &&
					MP::UI::Input::GetButtonExtraState() == false)
				{
					wasInsideMovable = true;
				}

				if (wasInsideMovable && State::CheckState(State::Window::Resized) == false)
				{
					State::SetState(State::Window::PositionChanged);
					Window::SetWindowPos(newWX, newWY);
				}
			}
			else
			{
				s32 mouseX, mouseY;
				Input::GetMousePosition(&mouseX, &mouseY);
				b8 inside = mouseX > bar->m_Pos.x && mouseX < (bar->m_Pos.x + bar->m_Size.x) &&
					mouseY > bar->m_Pos.y && mouseY < (bar->m_Pos.y + bar->m_Size.y);

				if (mouseX > MP::Data::_UI_BUTTONS_BACKGROUND_RIGHT_POS.x &&
					mouseX < MP::Data::_UI_BUTTONS_BACKGROUND_RIGHT_POS.x + MP::Data::_UI_BUTTONS_BACKGROUND_RIGHT_SIZE.x &&
					mouseY > MP::Data::_UI_BUTTONS_BACKGROUND_RIGHT_POS.y &&
					mouseY < MP::Data::_UI_BUTTONS_BACKGROUND_RIGHT_POS.y + MP::Data::_UI_BUTTONS_BACKGROUND_RIGHT_SIZE.y)
				{
					inside = false;
				}

				bar->hasFocus = inside;

				// TODO: dont call it every frame?
				winSizeBeforeResizeTop = Window::windowProperties.mApplicationHeight;
				wasInsideMovable = false;
				Input::GetGlobalMousePosition(&globalMouseX, &globalMouseY);
				Window::GetWindowPos(&startX, &startY);
			}
		
	}

	void App::ProcessButton(std::shared_ptr<Interface::Button> button)
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

		bool inside = mouseX > button->GetButtonPos().x && mouseX < (button->GetButtonPos().x + button->GetButtonSize().x) &&
			mouseY > button->GetButtonPos().y && mouseY < (button->GetButtonPos().y + button->GetButtonSize().y);

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

		bool inside = mouseX > button->GetButtonPos().x && mouseX < (button->GetButtonPos().x + button->GetButtonSize().x) &&
					  mouseY > button->GetButtonPos().y && mouseY < (button->GetButtonPos().y + button->GetButtonSize().y);

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


		if (inside && Input::IsKeyPressed(KeyCode::MouseRight))
			button->isPressedRight = true;
		else
			button->isPressedRight = false;

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

	void App::ProcesPlaylistButton(std::shared_ptr<Interface::PlaylistItem> button)
	{
		if (button->GetPlaylistItemPos() == glm::vec2(POS_INVALID))
		{
			return;
		}


		s32 mouseX, mouseY;

		s32 x, y;
		s32 winX, winY;
		Input::GetGlobalMousePosition(&x, &y);
		Window::GetWindowPos(&winX, &winY);

		
		mouseX = x - winX;
		mouseY = y - winY;
		
		bool check = mouseY > boundLow && mouseY < boundHigh;

		button->topHasFocus = mouseX > button->GetPlaylistItemPos().x && mouseX < (button->GetPlaylistItemPos().x + button->GetButtonSize().x) &&
			mouseY > button->GetPlaylistItemPos().y && mouseY < (button->GetPlaylistItemPos().y + button->GetButtonSize().y / 2.f);

		button->bottomHasFocus = mouseX > button->GetPlaylistItemPos().x && mouseX < (button->GetPlaylistItemPos().x + button->GetButtonSize().x) &&
			mouseY >(button->GetPlaylistItemPos().y + button->GetButtonSize().y / 2.f) &&
			mouseY < (button->GetPlaylistItemPos().y + button->GetButtonSize().y);

		button->topHasFocus = button->topHasFocus && check;
		button->bottomHasFocus = button->bottomHasFocus && check;
		
		bool inside = mouseX > button->GetPlaylistItemPos().x && mouseX < (button->GetPlaylistItemPos().x + button->GetButtonSize().x) &&
					  mouseY > button->GetPlaylistItemPos().y && mouseY < (button->GetPlaylistItemPos().y + button->GetButtonSize().y);


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

		if (inside && Input::IsKeyPressed(KeyCode::MouseRight))
			button->isPressedRight = true;
		else
			button->isPressedRight = false;


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

	void App::ProcessResizableTop(Interface::Resizable* barTop, Interface::Resizable* barBottom)
	{
		if (Input::IsKeyDown(KeyCode::MouseLeft))
		{
			int currentMouseX = 0, currentMouseY = 0;
			Input::GetGlobalMousePosition(&currentMouseX, &currentMouseY);

			int insideX, insideY;
			Input::GetMousePosition(&insideX, &insideY);

			s32 deltaY = globalMouseYRes - currentMouseY;

			s32 diff = startYRes + winSizeBeforeResizeTop;

			newSizeY = winSizeBeforeResizeTop + deltaY;
			newWY = startYRes - deltaY;


			f32 xL = barTop->m_Pos.x;
			f32 xR = barTop->m_Pos.x + barTop->m_Size.x;
			f32 yU = barTop->m_Pos.y;
			f32 yD = barTop->m_Pos.y + barTop->m_Size.y;

			bool inside = insideX > xL && insideX < xR && insideY > yU && insideY < yD;

			if (insideX > MP::Data::_UI_BUTTONS_BACKGROUND_RIGHT_POS.x &&
				insideX < MP::Data::_UI_BUTTONS_BACKGROUND_RIGHT_POS.x + MP::Data::_UI_BUTTONS_BACKGROUND_RIGHT_SIZE.x &&
				insideY > MP::Data::_UI_BUTTONS_BACKGROUND_RIGHT_POS.y &&
				insideY < MP::Data::_UI_BUTTONS_BACKGROUND_RIGHT_POS.y + MP::Data::_UI_BUTTONS_BACKGROUND_RIGHT_SIZE.y)
			{
				inside = false;
			}


			if (inside &&
				Input::IsKeyPressed(KeyCode::MouseLeft) == true &&
				MP::UI::Input::isButtonDown(MP::UI::Input::ButtonType::Minimize) == false &&
				MP::UI::Input::isButtonDown(MP::UI::Input::ButtonType::Exit) == false &&
				MP::UI::Input::GetButtonExtraState() == false)
			{
				wasInsideResizableTop = true;
			}

			if (wasInsideResizableTop == true)
			{
				State::SetState(State::Window::Resized);
				barTop->m_Pos = glm::vec2(barTop->m_Pos.x, -deltaY);

				resizeFinished = true;
			}
		}
		else if(resizeFinished == true)
		{
			// TODO: dont call it every frame?
			Window::windowProperties.mApplicationHeight = newSizeY;
			u32 diff = 0;
			

			if (Window::windowProperties.mApplicationHeight < MP::Data::_MIN_PLAYER_SIZE.y)
			{
				Window::windowProperties.mApplicationHeight = MP::Data::_MIN_PLAYER_SIZE.y;
				if (newWY > startYRes)
					newWY = startYRes;
				Window::SetWindowPos(startXRes, newWY + (winSizeBeforeResizeTop - Window::windowProperties.mApplicationHeight));
			}
			else if(Window::CheckWindowSize() == false)
			{
				Window::SetWindowPos(startXRes, newWY);
			}
			//State::SetState(State::Window::Resized);
			State::SetState(State::Window::ResizedFromTop);
			barTop->m_Pos = glm::vec2(0, 0);
			barBottom->m_Pos = glm::vec2(0, Window::windowProperties.mApplicationHeight - 5.f);
			resizeFinished = false;

		}
		else
		{
			s32 mouseX, mouseY;
			Input::GetMousePosition(&mouseX, &mouseY);
			b8 inside = mouseX > barTop->m_Pos.x && mouseX < (barTop->m_Pos.x + barTop->m_Size.x) &&
				mouseY > barTop->m_Pos.y && mouseY < (barTop->m_Pos.y + barTop->m_Size.y);

			barTop->hasFocus = inside;

			winSizeBeforeResizeTop = Window::windowProperties.mApplicationHeight;
			wasInsideResizableTop = false;
			Input::GetGlobalMousePosition(&globalMouseXRes, &globalMouseYRes);

			Window::GetWindowPos(&startXRes, &startYRes);
		}
	}

	void App::ProcessResizableBottom(Interface::Resizable* barBottom, Interface::Resizable* barTop)
	{
		if (Input::IsKeyDown(KeyCode::MouseLeft))
		{
			s32 mouseX, mouseY;
			Input::GetMousePosition(&mouseX, &mouseY);

			int relX = 0, relY = 0;
			Input::GetRelavtiveMousePosition(&relX, &relY);


			b8 inside = mouseX > barBottom->m_Pos.x && mouseX < (barBottom->m_Pos.x + barBottom->m_Size.x) &&
				mouseY > barBottom->m_Pos.y && mouseY < (barBottom->m_Pos.y + barBottom->m_Size.y);

			if (inside == true &&
				Input::IsKeyPressed(KeyCode::MouseLeft) == true)
			{
				wasInsideResizableBottom = true;
			}

			s32 globalMouseX, globalMouseY;
			Input::GetMousePosition(&globalMouseX, &globalMouseY);
			s32 winPosX, winPosY;
			Window::GetWindowPos(&winPosX, &winPosY);

			globalMouseY += winPosY;

			/*	If resizing is active and mouse is lower than bottom bound of appliaction and mouse has been
				going up, stop resizing till it reaches that bottom bound
			*/
			b8 stopResizing = (Window::windowProperties.mApplicationHeight + winPosY < globalMouseY) && relY < 0;
			
			if (wasInsideResizableBottom == true && 
				MP::UI::Input::GetButtonExtraState() == false &&
				stopResizing == false)
			{
				if(relY != 0)
					State::SetState(State::Window::Resized);
				Window::windowProperties.mDeltaHeightResize = relY;

				Window::windowProperties.mApplicationHeight = winSizeBeforeResizeBottom + relY;
				if (Window::CheckWindowSize() == false)
				{
					winSizeBeforeResizeBottom += relY;

					barBottom->m_Pos = glm::vec2(0, winSizeBeforeResizeBottom + relY - barBottom->m_Size.y);

					if (Window::windowProperties.mApplicationHeight - barBottom->m_Size.y < MP::Data::_MIN_PLAYER_SIZE.y)
						Window::windowProperties.mApplicationHeight = MP::Data::_MIN_PLAYER_SIZE.y + barBottom->m_Size.y;
					if (barBottom->m_Pos.y < MP::Data::_MIN_PLAYER_SIZE.y)
						barBottom->m_Pos.y = MP::Data::_MIN_PLAYER_SIZE.y;
				}
				else
				{
					// Make sure that window is resized to it's max possible size (1080 for 1920x1080 res)
					f32 diff = winSizeBeforeResizeBottom + relY - Window::windowProperties.mApplicationHeight;

					winSizeBeforeResizeBottom += relY - diff;

					barBottom->m_Pos = glm::vec2(0, winSizeBeforeResizeBottom + relY - diff - barBottom->m_Size.y);

					if (Window::windowProperties.mApplicationHeight - barBottom->m_Size.y < MP::Data::_MIN_PLAYER_SIZE.y)
						Window::windowProperties.mApplicationHeight = MP::Data::_MIN_PLAYER_SIZE.y + barBottom->m_Size.y;
					if (barBottom->m_Pos.y < MP::Data::_MIN_PLAYER_SIZE.y)
						barBottom->m_Pos.y = MP::Data::_MIN_PLAYER_SIZE.y;
				}
			}
		}
		else
		{
			s32 mouseX, mouseY;
			Input::GetMousePosition(&mouseX, &mouseY);
			b8 inside = mouseX > barBottom->m_Pos.x && mouseX < (barBottom->m_Pos.x + barBottom->m_Size.x) &&
				mouseY > barBottom->m_Pos.y && mouseY < (barBottom->m_Pos.y + barBottom->m_Size.y);

			barBottom->hasFocus = inside;
			State::ResetState(State::Window::Resized);
			winSizeBeforeResizeBottom = Window::windowProperties.mApplicationHeight;
			wasInsideResizableBottom = false;
		}
	}

	void App::SetButtonCheckBounds(s32 low, s32 high)
	{
		boundLow = low;
		boundHigh = high;
	}
}
