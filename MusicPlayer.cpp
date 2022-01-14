#include "MusicPlayer.hpp"


MusicPlayer::MusicPlayer() : m_volume(100)
{
	m_track_filenames[MusicTracks::kMenuTrack] = "Media/Audio/Music/DynamicFight_2.ogg";
	m_track_filenames[MusicTracks::kLevel1Track] = "Media/Audio/Music/Music_BG.wav";
	
}

void MusicPlayer::Play(MusicTracks track)
{
	std::string filename = m_track_filenames[track];

	if (!m_music.openFromFile(filename))
	{
		throw std::runtime_error("Music track " + filename + " could not be loaded.");
	}

	//m_music.setVolume(m_volume);
	//m_music.setLoop(true);
	m_music.play();
}

void MusicPlayer::Stop()
{
	m_music.stop();
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

void MusicPlayer::SetVolume(float volume)
{
	m_volume = volume;

	m_music.setVolume(m_volume);
}

int MusicPlayer::GetVolume()
{
	return m_volume;
}