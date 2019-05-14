#include "Scene/SoundSystem.h"
#include <iostream>
#include <filesystem>
namespace fs = std::experimental::filesystem;

SoundSystem::SoundSystem()
{
	irrKlangEngine = irrklang::createIrrKlangDevice();
	if (!irrKlangEngine)
	{
		std::cout << "Sound Engine setup error" << std::endl;
	}
}

SoundSystem::~SoundSystem()
{
	irrKlangEngine->drop();
}

SoundSystem* SoundSystem::getInstance()
{
	static SoundSystem* soundSys;
	if(!soundSys)
	{
		soundSys = new SoundSystem();
	}
	return soundSys;
}

irrklang::ISoundEngine* SoundSystem::getEngine()
{
	return getInstance()->irrKlangEngine;
}

irrklang::ISoundSource* SoundSystem::getSound(std::string name)
{
	return getInstance()->soundsMap.at(name);
}

void SoundSystem::loadSounds()
{
	fs::path currentPath = fs::current_path();
	fs::path srcPath;
	for (auto& it : currentPath)
	{
		srcPath /= it;
		if (it == "src")
			break;
	}
	srcPath /= "res\\sounds";

	for(auto it : fs::recursive_directory_iterator(srcPath))
	{
		std::cout << it.path().filename().stem() << std::endl;
		getEngine()->addSoundSourceFromFile(it.path().u8string().c_str());
		getInstance()->soundsMap.emplace(it.path().filename().stem().string(), getEngine()->getSoundSource(it.path().u8string().c_str()));
	}

	/*auto sound = getSound("Jigoku Shoujo Mitsuganae OST - Nigakute Amai Mizu");
	sound->setDefaultVolume(0.03f);
	getEngine()->play2D(sound);*/
}
