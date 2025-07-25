#include "Audio.hpp"
using namespace std;

Audio::Audio() {
	if (!Mix_OpenAudio(0, NULL))
		throw AudioException("Failed to open audio: " + string(SDL_GetError()));
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
		throw AudioException("Failed to initialize SDL: " + string(SDL_GetError()));
	int flags = MIX_INIT_MP3 | MIX_INIT_OGG;
	if ((Mix_Init(flags) & flags) != flags)
		throw AudioException("Failed to initialize SDL_mixer with MP3 and OGG support: " + string(SDL_GetError()));
}

Audio::~Audio() {
	if (sound.size() > 0) {
		for (auto& pair : sound) {
			if (pair.second)
				Mix_FreeChunk(pair.second);
		}
	}
	if (music.size() > 0) {
		for (auto& pair : music) {
			if (pair.second)
				Mix_FreeMusic(pair.second);
		}
	}
	Mix_Quit();
}

void Audio::LoadSound(const string& filename, unsigned int channel) {
	sound[channel] = Mix_LoadWAV(filename.c_str());
	if (!sound[channel])
		throw AudioException("Failed to load sound: " + filename + " - " + string(SDL_GetError()));
}

void Audio::PlaySound(unsigned int channel, int loops) {
	if (!sound[channel])
		throw AudioException("No sound loaded to play.");
	if (Mix_PlayChannel(channel, sound[channel], loops) == -1)
		throw AudioException("Failed to play sound: " + string(SDL_GetError()));
}

void Audio::SetPos(unsigned int channel, double position) {
	if (!music.count(channel))
		throw AudioException("No music loaded to set position.");
	if (Mix_SetMusicPosition(position) == -1)
		throw AudioException("Failed to set music position: " + string(SDL_GetError()));
}

void Audio::LoadMusic(const string& filename, int channel) {
	music[channel] = Mix_LoadMUS(filename.c_str());
	if (!music[channel])
		throw AudioException("Failed to load music: " + filename + " - " + string(SDL_GetError()));
}

void Audio::PlayMusic(unsigned int channel, int loops) {
	if (!music[channel])
		throw AudioException("No music loaded to play.");
	if (Mix_PlayMusic(music[channel], loops) == -1)
		throw AudioException("Failed to play music: " + string(SDL_GetError()));
}

void Audio::PauseMusic(unsigned int channel) {
	if (!music[channel])
		throw AudioException("No music loaded to pause.");
	Mix_PauseMusic();
}

void Audio::ResumeMusic(unsigned int channel) {
	if (!music[channel])
		throw AudioException("No music loaded to resume.");
	Mix_ResumeMusic();
}

bool Audio::IsMusicPlaying(unsigned int channel) const {
	if (!music.count(channel))
		throw AudioException("No music loaded to check.");
	return Mix_PlayingMusic();
}

void Audio::SetVolume(int channel, int volume) {
	if (channel < 0 || channel > 255)
		throw AudioException("Volume must be between 0 and 255.");
	if (Mix_Volume(channel, volume) == -1)
		throw AudioException("Failed to set volume: " + string(SDL_GetError()));
}

int Audio::GetVolume(int channel) const {
	return Mix_Volume(channel, -1);
}

double Audio::GetMusicPosition(unsigned int channel) const {
	if (!music.count(channel))
		throw AudioException("No music loaded to get position.");
	return Mix_GetMusicPosition(music.at(channel));
}
