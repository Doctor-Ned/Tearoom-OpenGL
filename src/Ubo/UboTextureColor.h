#ifndef UBOTEXTURECOLOR_H
#define UBOTEXTURECOLOR_H

#include "Ubo.h"

class UboTextureColor : public Ubo {
public:
	UboTextureColor(bool disableTexture, glm::vec4 color);
	void inject(bool disableTexture, glm::vec4 color);
};

#endif
