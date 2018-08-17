#include "md_shape.h"

#include <GL/gl3w.h>

#include "../graphics/md_vertices.h"


mdShape::mdShape()
{

}

mdShape* mdShape::QUAD()
{
	mdShape* s = new mdShape();
	s->currentShape = Shape::QUAD;

	if (s->vao == 0)
	{
		glGenBuffers(1, &s->vbo);
		glGenVertexArrays(1, &s->vao);
		glBindVertexArray(s->vao);
		glBindBuffer(GL_ARRAY_BUFFER, s->vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glBindVertexArray(0);
	}
	return s;
}

mdShape* mdShape::DOT()
{
	mdShape* s = new mdShape();
	s->currentShape = Shape::DOT;

	if (s->vao == 0)
	{
		glGenBuffers(1, &s->vbo);
		glGenVertexArrays(1, &s->vao);
		glBindVertexArray(s->vao);
		glBindBuffer(GL_ARRAY_BUFFER, s->vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glBindVertexArray(0);
	}
	return s;
}


mdShape::~mdShape()
{
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);

	delete this;
}

void mdShape::Draw(mdShader shader)
{
	shader.use();

	switch (currentShape)
	{
	case Shape::QUAD:
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		break;
	case Shape::DOT:
		glBindVertexArray(vao);
		glDrawArrays(GL_POINTS, 0, 1);
		glBindVertexArray(0);
		break;
	}
}