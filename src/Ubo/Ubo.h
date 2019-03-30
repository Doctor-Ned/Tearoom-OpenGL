#ifndef UBO_H
#define UBO_H

#include "Headers.h"

class Ubo {
public:
	virtual GLuint getID();
	virtual GLuint getBinding();
	virtual char* getBlockName();
	~Ubo();
protected:
	Ubo(unsigned int size, char* blockName, GLuint binding);
	GLuint binding;
	GLuint id;
	char* blockName;
	unsigned int size;
};

#endif
