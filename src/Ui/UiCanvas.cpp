#include "UiCanvas.h"
#include "Scene/AssetManager.h"

UiCanvas::UiCanvas(glm::vec2 position, glm::vec2 size, UiAnchor anchor) : UiElement(position, size, anchor) { }

ShaderType UiCanvas::getShaderType() {
	return STNone;
}

void UiCanvas::setup() { }
