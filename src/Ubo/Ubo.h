#ifndef UBO_H
#define UBO_H

#include "Headers.h"

class Ubo {
public:
	virtual GLuint getID();
	virtual GLuint getBinding();
	virtual char* getBlockName();
	virtual ~Ubo() = 0;
protected:
	Ubo(unsigned int size, char* blockName, GLuint binding);
protected:
	GLuint binding;
	GLuint id;
	char* blockName;
	unsigned int size;
};

#endif
