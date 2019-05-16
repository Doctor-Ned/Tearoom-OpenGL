#include "UiCanvas.h"
#include "Scene/AssetManager.h"

UiCanvas::UiCanvas(glm::vec2 position, glm::vec2 size, UiAnchor anchor) : UiElement(position, size, anchor) { }

void UiCanvas::updateWorld() {
	if(dirty && isRoot) {
		localTransform.setScale(referenceRescaler);
	}
	UiElement::updateWorld();
}

ShaderType UiCanvas::getShaderType() {
	return STNone;
}

void UiCanvas::setRoot() {
	isRoot = true;
}
