#ifndef MD_SHAPE_H
#define MD_SHAPE_H

#include <glm.hpp>

#include "md_shader.h"

enum SHAPE
{
	QUAD,
};

namespace mdEngine
{
	class Shape
	{
	public:
		Shape();
		~Shape();

		static Shape* QUAD();

		virtual void Draw(Shader shader);

	private:
		GLuint vbo = 0;
		GLuint vao = 0;
		SHAPE currentShape;
	};
}
#endif // !MD_SHAPE_H