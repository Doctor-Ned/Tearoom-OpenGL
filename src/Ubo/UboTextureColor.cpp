#include "UboTextureColor.h"

UboTextureColor::UboTextureColor(bool disableTexture, glm::vec4 color) : Ubo(
	sizeof(bool) + sizeof(glm::vec4), "TextureColor", 2) {
	inject(disableTexture, color);
}

void UboTextureColor::inject(bool disableTexture, glm::vec4 color) {
	glBindBuffer(GL_UNIFORM_BUFFER, id);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec4), value_ptr(color));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::vec4), sizeof(bool), &disableTexture);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
