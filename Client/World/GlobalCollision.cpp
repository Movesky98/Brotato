#include "GlobalCollision.h"

#include "World/CollisionInfoManager.h"

void CGlobalCollision::SetGlobalCollision()
{
	CCollisionInfoManager* InfoManager = CCollisionInfoManager::GetInst();

	if (InfoManager)
	{
		InfoManager->CreateChannel("PlayerProjectile");
		InfoManager->CreateProfile("PlayerProjectile", "PlayerProjectile", true, ECollisionInteraction::Ignore);
		
		InfoManager->CreateChannel("EnemyProjectile");
		InfoManager->CreateProfile("EnemyProjectile", "EnemyProjectile", true, ECollisionInteraction::Ignore);

		InfoManager->CreateChannel("BlockAll");
		InfoManager->CreateProfile("BlockAll", "BlockAll", true, ECollisionInteraction::Block);

		InfoManager->CreateChannel("OverlapAll");
		InfoManager->CreateProfile("OverlapAll", "OverlapAll", true);

		InfoManager->CreateChannel("IgnoreAll");
		InfoManager->CreateProfile("IgnoreAll", "IgnoreAll", true, ECollisionInteraction::Ignore);

		InfoManager->CreateChannel("Enemy");
		InfoManager->CreateProfile("Enemy", "Enemy", true, ECollisionInteraction::Ignore);

		InfoManager->CreateChannel("DetectEnemy");
		InfoManager->CreateProfile("DetectEnemy", "DetectEnemy", true, ECollisionInteraction::Ignore);
		
		InfoManager->CreateChannel("DetectPlayer");
		InfoManager->CreateProfile("DetectPlayer", "DetectPlayer", true, ECollisionInteraction::Ignore);

		InfoManager->CreateChannel("Collectable");
		InfoManager->CreateProfile("Collectable", "Collectable", true, ECollisionInteraction::Ignore);

		InfoManager->CreateChannel("PickupRange");
		InfoManager->CreateProfile("PickupRange", "PickupRange", true, ECollisionInteraction::Ignore);

		InfoManager->SetProfileInteraction("Static", "BlockAll", ECollisionInteraction::Block);
		InfoManager->SetProfileInteraction("Player", "BlockAll", ECollisionInteraction::Block);
		InfoManager->SetProfileInteraction("Monster", "BlockAll", ECollisionInteraction::Block);

		InfoManager->SetProfileInteraction("BlockAll", "BlockAll", ECollisionInteraction::Overlap);
		InfoManager->SetProfileInteraction("Enemy", "Player", ECollisionInteraction::Overlap);
		InfoManager->SetProfileInteraction("BlockAll", "Enemy", ECollisionInteraction::Block);
		InfoManager->SetProfileInteraction("Enemy", "BlockAll", ECollisionInteraction::Block);

		// A <-> B 서로 Block 이어야 활성화
		InfoManager->SetProfileInteraction("PlayerProjectile", "Enemy", ECollisionInteraction::Overlap);
		InfoManager->SetProfileInteraction("Enemy", "PlayerProjectile", ECollisionInteraction::Overlap);

		InfoManager->SetProfileInteraction("EnemyProjectile", "Player", ECollisionInteraction::Overlap);
		InfoManager->SetProfileInteraction("Player", "EnemyProjectile", ECollisionInteraction::Overlap);

		InfoManager->SetProfileInteraction("DetectEnemy", "Enemy", ECollisionInteraction::Overlap);
		InfoManager->SetProfileInteraction("Enemy", "DetectEnemy", ECollisionInteraction::Overlap);
		
		InfoManager->SetProfileInteraction("DetectPlayer", "Player", ECollisionInteraction::Overlap);
		InfoManager->SetProfileInteraction("Player", "DetectPlayer", ECollisionInteraction::Overlap);

		InfoManager->SetProfileInteraction("PickupRange", "Collectable", ECollisionInteraction::Overlap);
		InfoManager->SetProfileInteraction("Collectable", "PickupRange", ECollisionInteraction::Overlap);

		InfoManager->SetProfileInteraction("Collectable", "Player", ECollisionInteraction::Overlap);
	}
}
