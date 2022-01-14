#pragma once

#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "MusicTracks.hpp"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Audio/Music.hpp>
#include <map>
#include <string>


class MusicPlayer : private sf::NonCopyable
{
public:
	MusicPlayer();

	void Play(MusicTracks track);
	void Stop();
	void SetPaused(bool paused);
	void SetVolume(float volume);

	int GetVolume();

	int m_volume;

private:
	sf::Music m_music;
	std::map<MusicTracks, std::string> m_track_filenames;
};