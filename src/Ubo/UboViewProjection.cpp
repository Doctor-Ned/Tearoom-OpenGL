#include "UboViewProjection.h"

UboViewProjection::UboViewProjection(glm::mat4 view, glm::mat4 projection) : Ubo(
	2 * sizeof(glm::mat4), "ViewProjection", 0) {
	inject(view, projection);
}

void UboViewProjection::inject(glm::mat4 view, glm::mat4 projection) {
	glBindBuffer(GL_UNIFORM_BUFFER, id);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), value_ptr(view));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), value_ptr(projection));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
