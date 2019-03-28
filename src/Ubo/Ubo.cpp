#include "Ubo.h"

Ubo::Ubo(unsigned int size, char* blockName, GLuint binding) {
	this->size = size;
	this->blockName = blockName;
	this->binding = binding;
	glGenBuffers(1, &id);
	glBindBuffer(GL_UNIFORM_BUFFER, id);
	glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, binding, id, 0, size);
}

Ubo::~Ubo() { }

GLuint Ubo::getID() {
	return id;
}

GLuint Ubo::getBinding() {
	return binding;
}

char* Ubo::getBlockName() {
	return blockName;
}
