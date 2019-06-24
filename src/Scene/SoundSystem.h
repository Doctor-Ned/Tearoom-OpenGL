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
	sf::Sound *pickingSound = nullptr;
	static sf::Music *getMenuMusic();
public:
	static std::vector<std::string> getSoundFiles();
	static SoundSystem* getInstance();
	static void loadSounds();
	static sf::Sound *getSound(const std::string& file);
	static sf::Music *getMusic(const std::string& file);
	static float getMusicVolume();
	static void setMusicVolume(float volume);
	static void restartMusic();
	static void playPickingSound();
};

#endif