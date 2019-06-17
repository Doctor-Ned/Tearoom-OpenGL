#ifndef SOUND_SYSTEM_H
#define SOUND_SYSTEM_H
#include "SFML/Audio.hpp"
#include <vector>
#include <map>

class SoundSystem {
private:
	SoundSystem();
	~SoundSystem();
	std::map<std::string, std::string> simpleNamesToFilesMap;
	sf::Music *music = nullptr;
public:
	static SoundSystem* getInstance();
	static void loadSounds();
	static sf::Sound *getSound(const std::string& file);
	static sf::Music *getMusic(const std::string& file);
	static void restartMusic();
};

#endif