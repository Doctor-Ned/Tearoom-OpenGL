#include "Scene/SoundSystem.h"
#include <iostream>
#include <filesystem>
namespace fs = std::experimental::filesystem;

SoundSystem* SoundSystem::soundSystem = new SoundSystem();
std::vector<irrklang::ISoundSource*> SoundSystem::sounds = std::vector<irrklang::ISoundSource*>();

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

irrklang::ISoundEngine* SoundSystem::getInstance()
{
	return soundSystem->irrKlangEngine;
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
		std::cout << it << std::endl;
		getInstance()->addSoundSourceFromFile(it.path().u8string().c_str());
		sounds.push_back(getInstance()->getSoundSource(it.path().u8string().c_str()));
	}


	std::cout << getInstance()->getSoundSourceCount() << std::endl;
	auto sound = sounds[0];
	sound->setDefaultVolume(0.03f);
	getInstance()->play2D(sound);
}
