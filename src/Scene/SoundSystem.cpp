#include "Scene/SoundSystem.h"
#include <filesystem>
#include "Global.h"

namespace fs = std::experimental::filesystem;

SoundSystem::SoundSystem() {

}

SoundSystem::~SoundSystem() {

}

SoundSystem* SoundSystem::getInstance() {
	static SoundSystem* soundSys;
	if (!soundSys) {
		soundSys = new SoundSystem();
	}
	return soundSys;
}

void SoundSystem::loadSounds() {
	SoundSystem *ss = getInstance();
	for (auto it : fs::recursive_directory_iterator("res\\sounds")) {
		std::string fn = it.path().filename().u8string();
		if (Global::getExtension(fn) == "wav" || Global::getExtension(fn) == "ogg") {
			std::string noext = fn.substr(0, fn.find_last_of('.'));
			ss->simpleNamesToFilesMap.emplace(noext, "res\\sounds\\" + fn);
		}
	}
	restartMusic();
}

sf::Music *SoundSystem::getMusic(const std::string& file) {
	sf::Music *music = new sf::Music();
	if (!music->openFromFile(getInstance()->simpleNamesToFilesMap[file])) {
		delete music;
		return nullptr;
	}
	return music;
}

void SoundSystem::restartMusic() {
	SoundSystem *ss = getInstance();
	if (ss->music == nullptr) {
		ss->music = getMusic("sao-meo-loop");
		ss->music->setLoop(true);
		ss->music->setVolume(0.2f);
	} else {
		ss->music->stop();
	}
	ss->music->play();
}

sf::Sound *SoundSystem::getSound(const std::string& file) {
	sf::Sound *sound = new sf::Sound();
	sf::SoundBuffer *buffer = new sf::SoundBuffer();
	if (!buffer->loadFromFile(getInstance()->simpleNamesToFilesMap[file])) {
		delete buffer;
		delete sound;
		return nullptr;
	}
	sound->setBuffer(*buffer);
	return sound;
}
