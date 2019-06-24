#include "Scene/SoundSystem.h"
#include <filesystem>
#include "Global.h"

namespace fs = std::experimental::filesystem;

SoundSystem::SoundSystem() {

}

SoundSystem::~SoundSystem() {

}

sf::Music *SoundSystem::getMenuMusic() {
	SoundSystem *ss = getInstance();
	if(ss->music == nullptr) {
		ss->music = getMusic("sao-meo-loop");
		ss->music->setLoop(true);
		ss->music->setVolume(0.15f);
	}
	return ss->music;
}

std::vector<std::string> SoundSystem::getSoundFiles() {
	SoundSystem *ss = getInstance();
	std::vector<std::string> result;
	for (auto pair : ss->simpleNamesToFilesMap) {
		result.push_back(pair.first);
	}
	return result;
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
			if (ss->simpleNamesToFilesMap.find(noext) == ss->simpleNamesToFilesMap.end()) {
				ss->simpleNamesToFilesMap.emplace(noext, "res\\sounds\\" + fn);
			}
		}
	}
}

sf::Music *SoundSystem::getMusic(const std::string& file) {
	sf::Music *music = new sf::Music();
	if (!music->openFromFile(getInstance()->simpleNamesToFilesMap[file])) {
		delete music;
		return nullptr;
	}
	return music;
}

float SoundSystem::getMusicVolume() {
	return getMenuMusic()->getVolume();
}

void SoundSystem::setMusicVolume(float volume) {
	getMenuMusic()->setVolume(volume);
}

void SoundSystem::restartMusic() {
	getMenuMusic()->stop();
	getMenuMusic()->play();
}

void SoundSystem::playPickingSound() {
	SoundSystem *ss = getInstance();
	if(ss->pickingSound == nullptr) {
		ss->pickingSound = getSound("pickSound");
	}
	ss->pickingSound->play();
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
