#ifndef AUDIO_HPP
#define AUDIO_HPP

#include "exception.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_mixer.h>
#include <map>
#include <memory>

class Audio {
private:
	std::map<unsigned int, Mix_Chunk*> sound;	//効果音
	std::map<unsigned int, Mix_Music*> music;	//BGM
public:
	Audio();
	~Audio();

	void LoadSound(const std::string& filename, unsigned int channel);
	void PlaySound(unsigned int channel, int loops = 0);
	void StopSound(int channel = -1) { Mix_HaltChannel(channel); };

	void LoadMusic(const std::string& filename, int channel);
	void PlayMusic(unsigned int channel, int loops = 0);
	void SetPos(unsigned int channel, double position);
	void PauseMusic(unsigned int channel);
	void ResumeMusic(unsigned int channel);
	void StopMusic() { Mix_HaltMusic(); };
	bool IsMusicPlaying(unsigned int channel) const;
	double GetMusicPosition(unsigned int channel) const;

	void SetVolume(int channel, int volume);	//音量設定
	int GetVolume(int channel) const;	//音量取得
};


#endif