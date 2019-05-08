#ifndef SOUND_SYSTEM_H
#define SOUND_SYSTEM_H
#include "irrKlang.h"
#include <vector>
#include <map>

class SoundSystem
{
private:
	irrklang::ISoundEngine* irrKlangEngine;
	std::map<std::string, irrklang::ISoundSource*> soundsMap;
	SoundSystem();
	~SoundSystem();
public:
	static SoundSystem* getInstance();
	static irrklang::ISoundEngine* getEngine();
	static void loadSounds();
};

#endif