#pragma once

enum ShaderType;
class Shader;

class Renderable {
public:
	virtual void updateDrawData() = 0;
	virtual void drawSelf(Shader *shader) = 0;
	virtual ShaderType getShaderType() = 0;
	virtual bool isOpaque() = 0;
};
