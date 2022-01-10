#include "MusicPlayer.hpp"


MusicPlayer::MusicPlayer() : m_volume(100.0f)
{
	m_track_filenames[MusicTracks::kMenuTrack] = "Media/Music/DynamicFight_2.mp3";
	m_track_filenames[MusicTracks::kLevel1Track] = "Media/Music/Music_BG.mp3";
}

void MusicPlayer::Play(MusicTracks track)
{
	std::string filename = m_track_filenames[track];

	if (!m_music.openFromFile(filename))
	{
		throw std::runtime_error("Music track " + filename + " could not be loaded.");
	}

	m_music.setVolume(m_volume);
	m_music.setLoop(true);
	m_music.play();
}

void MusicPlayer::Stop()
{
	m_music.stop();
}

void MusicPlayer::SetVolume(float volume)
{
	m_volume = volume;
}

void MusicPlayer::SetPaused(bool paused)
{
	if (paused)
	{
		m_music.pause();
	}
	else
	{
		m_music.play();
	}
}