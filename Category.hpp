#pragma once
//SceneNode category used to dispatch commands
namespace Category
{
	enum Type
	{
		kNone = 0,
		kScene = 1 << 0,
		kPlayerTank = 1 << 1,
		kAlliedTank = 1 << 2,
		kEnemyTank = 1 << 3,
		kPickup = 1 << 4,
		kAlliedProjectile = 1 << 5,
		kEnemyProjectile = 1 << 6,

		kTank = kPlayerTank | kAlliedTank | kEnemyTank,
		kProjectile = kAlliedProjectile | kEnemyProjectile,
	};
}