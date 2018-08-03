#ifndef MD_SHAPE_H
#define MD_SHAPE_H

#include <glm.hpp>

#include "md_shader.h"

enum SHAPE
{
	QUAD,
};

class mdShape
{
public:
	mdShape();
	~mdShape();

	static mdShape* QUAD();

	virtual void Draw(mdShader shader);

private:
	GLuint vbo = 0;
	GLuint vao = 0;
	SHAPE currentShape;
};

#endif // !MD_SHAPE_H