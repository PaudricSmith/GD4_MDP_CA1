#include "SoundPlayer.hpp"
#include "SoundEffects.hpp"

#include <SFML/Audio/Listener.hpp>

#include <cmath>
#include <iostream>


namespace
{
	// Sound coordinate system, point of view of a player in front of the screen:
	// X = left; Y = up; Z = back (out of the screen)
	const float ListenerZ = 300.f;
	const float Attenuation = 8.f;
	const float MinDistance2D = 200.f;
	const float MinDistance3D = std::sqrt(MinDistance2D * MinDistance2D + ListenerZ * ListenerZ);
}

SoundPlayer::SoundPlayer() : m_volume(100)
{
	// SFX below came from ultimate-space-game-mega-asset-package below
	// https://gamesupply.itch.io/ultimate-space-game-mega-asset-package

	m_sound_buffers.Load(SoundEffects::kTankMoving, "Media/Audio/SFX/VehicleMove.wav");
	m_sound_buffers.Load(SoundEffects::kTankMovingFast, "Media/Audio/SFX/car_sound.wav"); // ******* TO BE DONE ******* 
	m_sound_buffers.Load(SoundEffects::kTankCannonMoving, "Media/Audio/SFX/Random56.wav");
	m_sound_buffers.Load(SoundEffects::kLaunchGuidedMissile, "Media/Audio/SFX/dive.wav");
	m_sound_buffers.Load(SoundEffects::kGuidedMissileHit, "Media/Audio/SFX/bomb.wav");
	m_sound_buffers.Load(SoundEffects::kNormalBulletFire, "Media/Audio/SFX/Shoot1.wav");
	m_sound_buffers.Load(SoundEffects::kNormalBulletHit, "Media/Audio/SFX/EnemyHit3.wav");
	m_sound_buffers.Load(SoundEffects::kTankHitTank, "Media/Audio/SFX/EnemyHit1.wav");
	m_sound_buffers.Load(SoundEffects::kCollectGoodPickup, "Media/Audio/SFX/invicible.wav");
	m_sound_buffers.Load(SoundEffects::kCollectBadPickup, "Media/Audio/SFX/blackhole.wav");
	m_sound_buffers.Load(SoundEffects::kButtonSelected, "Media/Audio/SFX/Slide_Sharp_01.wav");
	m_sound_buffers.Load(SoundEffects::kButtonPressed, "Media/Audio/SFX/Click_Heavy_00.wav");
	m_sound_buffers.Load(SoundEffects::kWinJingle, "Media/Audio/SFX/win_loud.wav");
	m_sound_buffers.Load(SoundEffects::kToastBeep1, "Media/Audio/SFX/unitready2.wav");
	m_sound_buffers.Load(SoundEffects::kToastBeep2, "Media/Audio/SFX/unitready3.wav");

	// Listener points towards the screen (default in SFML)
	sf::Listener::setDirection(0.f, 0.f, 0.f); // SET ALL TO ZERO SO NO 3D SOUND
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
	sound.setVolume(m_volume);

	if (effect == SoundEffects::kTankCannonMoving)
	{
		sound.setVolume(m_volume / 7); // Keep the Cannon movement 1/7th times lower than everything else because its too loud
	}
	else if (effect == SoundEffects::kTankMoving)
	{
		sound.setVolume(m_volume / 2); // Keep the Tank movement SFX half times lower than everything else because its too loud
	}
	
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

void SoundPlayer::SetVolume(int volume)
{
	m_volume = volume;

	for (auto s : m_sounds)
	{
		s.setVolume(m_volume);
	}
}

int SoundPlayer::GetVolume()
{
	return m_volume;
}

sf::Vector2f SoundPlayer::GetListenerPosition() const
{
	sf::Vector3f position = sf::Listener::getPosition();
	return sf::Vector2f(position.x, -position.y);
}