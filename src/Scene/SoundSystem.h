#ifndef SOUND_SYSTEM_H
#define SOUND_SYSTEM_H
#include "AL/al.h"
#include "AL/alc.h"
#include <vector>
#include <map>

class SoundSystem
{
private:
	ALCdevice *device;
	std::map<std::string, ALuint> soundsMap;
	SoundSystem();
	~SoundSystem();
public:
	static SoundSystem* getInstance();
	static irrklang::ISoundEngine* getEngine();
	static irrklang::ISoundSource* getSound(std::string name);
	static void loadSounds();
};

#endif