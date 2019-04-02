#pragma once

class GuiConfigurable {
public:
	void drawGui();
protected:
	virtual void renderGui() = 0;
};