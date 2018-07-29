#include "md_shape.h"

#include <GL/gl3w.h>
#include "md_vertices.h"

namespace mdEngine
{

	Shape::Shape()
	{

	}

	Shape* Shape::QUAD()
	{
		Shape* s = new Shape();
		s->currentShape = SHAPE::QUAD;

		if (s->vao == 0)
		{
			glGenBuffers(1, &s->vbo);
			glGenVertexArrays(1, &s->vao);
			glBindVertexArray(s->vao);
			glBindBuffer(GL_ARRAY_BUFFER, s->vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glBindVertexArray(0);

			std::cout << s->vao << std::endl;
		}
		return s;
	}

	Shape::~Shape()
	{
		/*switch (currentShape)
		{
		case SHAPE::CUBE: case SHAPE::CUBEMAP: case SHAPE::QUAD: case SHAPE::PLANE:
			glDeleteBuffers(1, &vbo);
			glDeleteVertexArrays(1, &vao);
			break;
		}*/
	}

	void Shape::Draw(Shader shader)
	{
		shader.use();

		switch (currentShape)
		{
		case SHAPE::QUAD:
			glBindVertexArray(vao);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
			break;
		}
	}

}