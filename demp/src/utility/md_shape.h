#ifndef MD_SHAPE_H
#define MD_SHAPE_H

#include <glm.hpp>

#include "md_shader.h"

enum Shape
{
	QUAD,
	DOT
};

class mdShape
{
public:
	mdShape();
	~mdShape();

	static mdShape* QUAD();
	static mdShape* MAIN_WINDOW_QUAD();
	static mdShape* DOT();

	virtual void Draw(mdShader* shader);

	GLuint* GetVBO();
	GLuint* GETVAO();

private:
	GLuint vbo = 0;
	GLuint vao = 0;
	Shape currentShape;
};

#endif // !MD_SHAPE_H