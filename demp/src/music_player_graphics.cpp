#include "music_player_graphics.h"

#include <gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "md_load_texture.h"
#include "realtime_system_application.h"
#include "md_shader.h"
#include "md_shape.h"
#include "music_player_ui_input.h"
#include <glm.hpp>



using namespace mdEngine::MP::UI;
using namespace mdEngine::MP::UI::Data;

namespace mdEngine
{
namespace Graphics
{
	namespace MP
	{
		Shader mdWindowShader;
		Shape * mdQuad = NULL;
		GLuint main_background, main_foreground;
		GLuint exit_background, exit_icon, minimize_icon, stay_on_top_icon;
		GLuint volume_bar_bounds, volume_bar_middle, volume_bar_lines;
		GLuint play_button, stop_button, next_button, previous_button, shuffle_button, repeat_button, dot_button_state;

		b8 shuffleActive(false);
		b8 repeatActive(false);
		b8 playActive(false);

		f32 mdCurrentWidth;
		f32 mdCurrentHeight;

	

	}

	void MP::StartMainWindow()
	{
		mdQuad			= Shape::QUAD();
		mdWindowShader	= Shader("shaders/window.vert", "shaders/window.frag", nullptr);

		main_background		= mdLoadTexture("assets/main.png");
		main_foreground		= mdLoadTexture("assets/main_foreground.png");

		exit_background		= mdLoadTexture("assets/exit_background.png");
		exit_icon			= mdLoadTexture("assets/exit_icon.png");

		volume_bar_bounds	= mdLoadTexture("assets/volume_bar_bounds.png");
		volume_bar_middle	= mdLoadTexture("assets/volume_bar_middle.png");
		volume_bar_lines	= mdLoadTexture("assets/volume_bar_lines.png");

		play_button			= mdLoadTexture("assets/play_button.png");
		stop_button			= mdLoadTexture("assets/stop_button.png");
		next_button			= mdLoadTexture("assets/next_button.png");
		previous_button		= mdLoadTexture("assets/previous_button.png");
		shuffle_button		= mdLoadTexture("assets/shuffle_button.png");
		repeat_button		= mdLoadTexture("assets/repeat_button.png");
		dot_button_state	= mdLoadTexture("assets/dot_button_state.png");

		mdCurrentWidth	= (float)mdEngine::windowProperties.mWindowWidth;
		mdCurrentHeight = (float)mdEngine::windowProperties.mWindowHeight;

		glm::mat4 projection = glm::ortho(0.f, mdCurrentWidth, mdCurrentHeight, 0.f, -1.0f, 1.f);

		mdWindowShader.use();
		mdWindowShader.setInt("image", 0);
		mdWindowShader.setMat4("projection", projection);
	}

	void MP::UpdateMainWindow()
	{
		

	}

	void MP::RenderMainWindow()
	{
		/* Main Bar */
		glm::mat4 model;
		glm::mat4 dotModel;
		f32 dotOffsetXShuffle = 0.4f;
		f32 dotOffsetXRepeat = -0.2f;

		glm::vec3 white(1.f);
		glm::vec3 grey(0.85f);
		glm::vec3 green(0.f, 0.7f, 0.f);
		glm::vec3 color(white);

		mdWindowShader.use();
		glActiveTexture(GL_TEXTURE0);
		mdWindowShader.setVec3("color", color);

		/* Main background*/
		{
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(_MAIN_BACKGROUND_POS, 0.f));
			model = glm::scale(model, glm::vec3(_MAIN_BACKGROUND_SIZE, 1.f));;
			mdWindowShader.setMat4("model", model);
			mdWindowShader.setBool("cut", true);
			glBindTexture(GL_TEXTURE_2D, main_background);
			mdQuad->Draw(mdWindowShader);
			mdWindowShader.setBool("cut", false);

			model = glm::mat4();
			model = glm::translate(model, glm::vec3(20.0f, 20.0f, 0.1f));
			model = glm::scale(model, glm::vec3(mdCurrentWidth - 40, mdCurrentHeight - 40, 1.f));;
			mdWindowShader.setMat4("model", model);
			mdWindowShader.setBool("cut", true);
			glBindTexture(GL_TEXTURE_2D, main_foreground);
			mdQuad->Draw(mdWindowShader);
			mdWindowShader.setBool("cut", false);
		}

		/* Volume bar */
		{
			/*model = glm::mat4();
			model = glm::translate(model, glm::vec3(0.23f, -0.695f, 0.95f));
			model = glm::scale(model, scale);
			mdWindowShader.setMat4("model", model);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, volume_bar_lines);
			mdQuad->Draw(mdWindowShader);*/

			model = glm::mat4();
			model = glm::translate(model, glm::vec3(_VOLUME_BAR_BOUNDS_POS, 0.3f));
			model = glm::scale(model, glm::vec3(_VOLUME_BAR_BOUNDS_SIZE, 1.f));
			mdWindowShader.setMat4("model", model);
			glBindTexture(GL_TEXTURE_2D, volume_bar_bounds);
			mdQuad->Draw(mdWindowShader);

			model = glm::mat4();
			model = glm::translate(model, glm::vec3(_VOLUME_BAR_MIDDLE_POS, 0.2));
			model = glm::scale(model, glm::vec3(_VOLUME_BAR_MIDDLE_SIZE, 1.f));
			mdWindowShader.setMat4("model", model);
			glBindTexture(GL_TEXTURE_2D, volume_bar_middle);
			mdQuad->Draw(mdWindowShader);
		}

		/*  Music UI*/
		{
			f32 focusMultiplier;

			// Shuffle 
			glm::mat4 dotModel;
			model = glm::mat4();

			if(Input::isButtonPressed(Input::ButtonType::Shuffle))
				shuffleActive = !shuffleActive;

			if (Input::hasFocus(Input::ButtonType::Shuffle) && shuffleActive == false)
			{
				focusMultiplier = 1.02f;
				color = white;
				model = glm::translate(model, glm::vec3(_SHUFFLE_BUTTON_POS.x + (_SHUFFLE_BUTTON_SIZE.x / 2.f) * (1.f - focusMultiplier),
					_SHUFFLE_BUTTON_POS.y + (_SHUFFLE_BUTTON_SIZE.y / 2.f) * (1.f - focusMultiplier), 0.3f));
				model = glm::scale(model, glm::vec3(_SHUFFLE_BUTTON_SIZE * focusMultiplier, 1.f));

			}
			else if (shuffleActive == false)
			{
				color = grey;
				model = glm::translate(model, glm::vec3(_SHUFFLE_BUTTON_POS, 0.3f));
				model = glm::scale(model, glm::vec3(_SHUFFLE_BUTTON_SIZE, 1.f));;
			}
			else
			{
				color = green;
				model = glm::translate(model, glm::vec3(_SHUFFLE_BUTTON_POS, 0.3f));
				model = glm::scale(model, glm::vec3(_SHUFFLE_BUTTON_SIZE, 1.f));;

				dotModel = glm::translate(dotModel, glm::vec3(_SHUFFLE_BUTTON_POS.x + _SHUFFLE_BUTTON_SIZE.x / 2.f - _DOT_BUTTON_STATE_SIZE.x / 2.f - dotOffsetXShuffle, _SHUFFLE_BUTTON_POS.y + 18.f, 0.3f));
				dotModel = glm::scale(dotModel, glm::vec3(_DOT_BUTTON_STATE_SIZE, 1.f));
				mdWindowShader.setVec3("color", color);
				mdWindowShader.setMat4("model", dotModel);
				glBindTexture(GL_TEXTURE_2D, dot_button_state);
				mdQuad->Draw(mdWindowShader);

			}
			mdWindowShader.setVec3("color", color);
			mdWindowShader.setMat4("model", model);
			glBindTexture(GL_TEXTURE_2D, shuffle_button);
			mdQuad->Draw(mdWindowShader);





			// Previous
			model = glm::mat4();
			if (Input::hasFocus(Input::ButtonType::Previous))
			{
				focusMultiplier = 1.02f;
				color = white;
				model = glm::translate(model, glm::vec3(_PREVIOUS_BUTTON_POS.x + (_PREVIOUS_BUTTON_SIZE.x / 2.f) * (1.f - focusMultiplier),
					_PREVIOUS_BUTTON_POS.y + (_PREVIOUS_BUTTON_SIZE.y / 2.f) * (1.f - focusMultiplier), 0.3f));
				model = glm::scale(model, glm::vec3(_PREVIOUS_BUTTON_SIZE * focusMultiplier, 1.f));

			}
			else
			{
				color = grey;
				model = glm::translate(model, glm::vec3(_PREVIOUS_BUTTON_POS, 0.3f));
				model = glm::scale(model, glm::vec3(_PREVIOUS_BUTTON_SIZE, 1.f));;
			}
			mdWindowShader.setVec3("color", color);
			mdWindowShader.setMat4("model", model);
			glBindTexture(GL_TEXTURE_2D, previous_button);
			mdQuad->Draw(mdWindowShader);


			// Play

			if (Input::isButtonPressed(Input::ButtonType::Play))
				playActive = !playActive;

			model = glm::mat4();
			if (Input::hasFocus(Input::ButtonType::Play) && playActive == false)
			{
				focusMultiplier = 1.07f;
				color = white;
				model = glm::translate(model, glm::vec3(_PLAY_BUTTON_POS.x + (_PLAY_BUTTON_SIZE.x / 2.f) * (1.f - focusMultiplier), 
														_PLAY_BUTTON_POS.y + (_PLAY_BUTTON_SIZE.y / 2.f) * (1.f - focusMultiplier), 0.3f));
				model = glm::scale(model, glm::vec3(_PLAY_BUTTON_SIZE * focusMultiplier, 1.f));

			}
			else if(!Input::hasFocus(Input::ButtonType::Play) && playActive == false) 
			{
				color = grey;
				model = glm::translate(model, glm::vec3(_PLAY_BUTTON_POS, 0.3f));
				model = glm::scale(model, glm::vec3(_PLAY_BUTTON_SIZE, 1.f));;
			}

			if (playActive == false)
			{
				mdWindowShader.setVec3("color", color);

				mdWindowShader.setMat4("model", model);
				glBindTexture(GL_TEXTURE_2D, play_button);
				mdQuad->Draw(mdWindowShader);
			}

			// Stop

		

			model = glm::mat4();
			if (Input::hasFocus(Input::ButtonType::Stop) && playActive == true)
			{
				focusMultiplier = 1.07f;
				color = white;
				model = glm::translate(model, glm::vec3(_PLAY_BUTTON_POS.x + (_PLAY_BUTTON_SIZE.x / 2.f) * (1.f - focusMultiplier),
					_PLAY_BUTTON_POS.y + (_PLAY_BUTTON_SIZE.y / 2.f) * (1.f - focusMultiplier), 0.3f));
				model = glm::scale(model, glm::vec3(_PLAY_BUTTON_SIZE * focusMultiplier, 1.f));

			}
			else if(!Input::hasFocus(Input::ButtonType::Stop) && playActive == true)
			{
				color = grey;
				model = glm::translate(model, glm::vec3(_PLAY_BUTTON_POS, 0.3f));
				model = glm::scale(model, glm::vec3(_PLAY_BUTTON_SIZE, 1.f));;
			}

			if (playActive)
			{
				mdWindowShader.setVec3("color", color);

				mdWindowShader.setMat4("model", model);
				glBindTexture(GL_TEXTURE_2D, stop_button);
				mdQuad->Draw(mdWindowShader);
			}


			// Next
			model = glm::mat4();
			if (Input::hasFocus(Input::ButtonType::Next))
			{
				focusMultiplier = 1.02f;
				color = white;
				model = glm::translate(model, glm::vec3(_NEXT_BUTTON_POS.x + (_NEXT_BUTTON_SIZE.x / 2.f) * (1.f - focusMultiplier),
					_NEXT_BUTTON_POS.y + (_NEXT_BUTTON_SIZE.y / 2.f) * (1.f - focusMultiplier), 0.3f));
				model = glm::scale(model, glm::vec3(_NEXT_BUTTON_SIZE * focusMultiplier, 1.f));

			}
			else
			{
				color = grey;
				model = glm::translate(model, glm::vec3(_NEXT_BUTTON_POS, 0.3f));
				model = glm::scale(model, glm::vec3(_NEXT_BUTTON_SIZE, 1.f));;
			}
			mdWindowShader.setVec3("color", color);
			mdWindowShader.setMat4("model", model);
			glBindTexture(GL_TEXTURE_2D, next_button);
			mdQuad->Draw(mdWindowShader);


			// Repeat
			if (Input::isButtonPressed(Input::ButtonType::Repeat))
				repeatActive = !repeatActive;

			model = glm::mat4();
			if (Input::hasFocus(Input::ButtonType::Repeat) && repeatActive == false)
			{
				focusMultiplier = 1.02f;
				color = white;
				model = glm::translate(model, glm::vec3(_REPEAT_BUTTON_POS.x + (_REPEAT_BUTTON_SIZE.x / 2.f) * (1.f - focusMultiplier),
					_REPEAT_BUTTON_POS.y + (_REPEAT_BUTTON_SIZE.y / 2.f) * (1.f - focusMultiplier), 0.3f));
				model = glm::scale(model, glm::vec3(_REPEAT_BUTTON_SIZE * focusMultiplier, 1.f));

			}
			else if(repeatActive == false)
			{
				color = grey;
				model = glm::translate(model, glm::vec3(_REPEAT_BUTTON_POS, 0.3f));
				model = glm::scale(model, glm::vec3(_REPEAT_BUTTON_SIZE, 1.f));;
			}
			else
			{
				color = green;
				model = glm::translate(model, glm::vec3(_REPEAT_BUTTON_POS, 0.3f));
				model = glm::scale(model, glm::vec3(_REPEAT_BUTTON_SIZE, 1.f));

				dotModel = glm::mat4();
				dotModel = glm::translate(dotModel, glm::vec3(_REPEAT_BUTTON_POS.x + _REPEAT_BUTTON_SIZE.x / 2.f - _DOT_BUTTON_STATE_SIZE.x / 2.f - dotOffsetXRepeat, _REPEAT_BUTTON_POS.y + 18.f, 0.3f));
				dotModel = glm::scale(dotModel, glm::vec3(_DOT_BUTTON_STATE_SIZE, 1.f));
				mdWindowShader.setVec3("color", color);
				mdWindowShader.setMat4("model", dotModel);
				glBindTexture(GL_TEXTURE_2D, dot_button_state);
				mdQuad->Draw(mdWindowShader);
			}
			mdWindowShader.setVec3("color", color);
			mdWindowShader.setMat4("model", model);
			glBindTexture(GL_TEXTURE_2D, repeat_button);
			mdQuad->Draw(mdWindowShader);
		}

		color = white;
		mdWindowShader.setVec3("color", color);


		/* UI Window buttons*/
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(mdCurrentWidth - 70.f, 5.f, 0.2f));
		model = glm::scale(model, glm::vec3(40.f, 15.0f, 1.f));;
		mdWindowShader.setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, exit_background);
		mdQuad->Draw(mdWindowShader);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(Data::_EXIT_BUTTON_POS, 0.3f));
		model = glm::scale(model, glm::vec3(Data::_EXIT_BUTTON_SIZE, 1.f));;
		mdWindowShader.setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, exit_icon);
		mdQuad->Draw(mdWindowShader);






	}

	void MP::CloseMainWindow()
	{

	}
}
}