#ifndef SOUND_SYSTEM_H
#define SOUND_SYSTEM_H
#include "irrKlang.h"
#include <vector>

class SoundSystem
{
private:
	static SoundSystem* soundSystem;
	irrklang::ISoundEngine* irrKlangEngine;
	static std::vector<irrklang::ISoundSource*> sounds;
	SoundSystem();
	~SoundSystem();
public:
	static irrklang::ISoundEngine* getInstance();
	static void loadSounds();
};

#endif