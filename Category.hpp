#pragma once

//SceneNode category used to dispatch commands
namespace Category
{
	enum Type
	{
		kNone = 0,
		kScene = 1 << 0,
		kPlayerTank = 1 << 1,
		kPlayer2Tank = 1 << 2,
		kAlliedTank = 1 << 3,
		kEnemyTank = 1 << 4,
		kPickup = 1 << 5,
		kAlliedProjectile = 1 << 6,
		kEnemyProjectile = 1 << 7,
		kSoundEffect = 1 << 8,

		kTank = kPlayerTank | kPlayer2Tank| kAlliedTank | kEnemyTank,
		kProjectile = kAlliedProjectile | kEnemyProjectile,
	};
}