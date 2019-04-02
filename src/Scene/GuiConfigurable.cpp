#include "GuiConfigurable.h"
#include "Global.h"

void GuiConfigurable::drawGui() {
	ImGui::PushID(reinterpret_cast<uintptr_t>(this));
	renderGui();
	ImGui::PopID();
}
