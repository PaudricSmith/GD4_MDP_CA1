#include "SoundPlayer.hpp"
#include "SoundEffects.hpp"

#include <SFML/Audio/Listener.hpp>

#include <cmath>


namespace
{
	// Sound coordinate system, point of view of a player in front of the screen:
	// X = left; Y = up; Z = back (out of the screen)
	const float ListenerZ = 300.f;
	const float Attenuation = 8.f;
	const float MinDistance2D = 200.f;
	const float MinDistance3D = std::sqrt(MinDistance2D * MinDistance2D + ListenerZ * ListenerZ);
}

SoundPlayer::SoundPlayer()
{
	m_sound_buffers.Load(SoundEffects::kLaunchGuidedMissile, "Media/Audio/SFX/dive.wav");
	m_sound_buffers.Load(SoundEffects::kGuidedMissileHit, "Media/Audio/SFX/bomb.wav");
	m_sound_buffers.Load(SoundEffects::kNormalBulletFire, "Media/Audio/SFX/Shoot1.wav");
	m_sound_buffers.Load(SoundEffects::kNormalBulletHit, "Media/Audio/SFX/EnemyHit3.wav");
	m_sound_buffers.Load(SoundEffects::kTankHitTank, "Media/Audio/SFX/EnemyHit1.wav");
	//m_sound_buffers.Load(SoundEffects::kCollectPickup, "Media/Audio/SFX/CollectPickup.wav");
	m_sound_buffers.Load(SoundEffects::kButtonSelected, "Media/Audio/SFX/Slide_Sharp_01.wav");
	m_sound_buffers.Load(SoundEffects::kButtonPressed, "Media/Audio/SFX/Click_Heavy_00.wav");

	// Listener points towards the screen (default in SFML)
	sf::Listener::setDirection(0.f, 0.f, -1.f);
}

void SoundPlayer::Play(SoundEffects effect)
{
	Play(effect, GetListenerPosition());
}

void SoundPlayer::Play(SoundEffects effect, sf::Vector2f position)
{
	m_sounds.emplace_back(sf::Sound());
	sf::Sound& sound = m_sounds.back();

	sound.setBuffer(m_sound_buffers.Get(effect));
	sound.setPosition(position.x, -position.y, 0.f);
	sound.setAttenuation(Attenuation);
	sound.setMinDistance(MinDistance3D);

	sound.play();
}

void SoundPlayer::RemoveStoppedSounds()
{
	m_sounds.remove_if([](const sf::Sound& s)
		{
			return s.getStatus() == sf::Sound::Stopped;
		});
}

void SoundPlayer::RemovePlayingSounds()
{
	m_sounds.remove_if([](const sf::Sound& s)
		{
			return s.getStatus() == sf::Sound::Playing;
		});
}

void SoundPlayer::SetListenerPosition(sf::Vector2f position)
{
	sf::Listener::setPosition(position.x, -position.y, ListenerZ);
}

sf::Vector2f SoundPlayer::GetListenerPosition() const
{
	sf::Vector3f position = sf::Listener::getPosition();
	return sf::Vector2f(position.x, -position.y);
}