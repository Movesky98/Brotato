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

		InfoManager->CreateChannel("PlayerTrigger");
		InfoManager->CreateProfile("PlayerTrigger", "PlayerTrigger", true, ECollisionInteraction::Ignore);
		
		InfoManager->CreateChannel("EnemyTrigger");
		InfoManager->CreateProfile("EnemyTrigger", "EnemyTrigger", true, ECollisionInteraction::Ignore);

		InfoManager->CreateChannel("Wall");
		InfoManager->CreateProfile("Wall", "Wall", true, ECollisionInteraction::Block);

		InfoManager->SetProfileInteraction("Static", "BlockAll", ECollisionInteraction::Block);
		InfoManager->SetProfileInteraction("Player", "BlockAll", ECollisionInteraction::Block);
		InfoManager->SetProfileInteraction("Monster", "BlockAll", ECollisionInteraction::Block);

		InfoManager->SetProfileInteraction("Enemy", "Player", ECollisionInteraction::Overlap);
		InfoManager->SetProfileInteraction("BlockAll", "Enemy", ECollisionInteraction::Block);
		InfoManager->SetProfileInteraction("Enemy", "BlockAll", ECollisionInteraction::Block);

		// A <-> B 서로 Block 이어야 활성화
		InfoManager->SetProfileInteraction("PlayerProjectile", "Enemy", ECollisionInteraction::Overlap);
		InfoManager->SetProfileInteraction("Enemy", "PlayerProjectile", ECollisionInteraction::Overlap);

		InfoManager->SetProfileInteraction("EnemyProjectile", "Player", ECollisionInteraction::Overlap);
		InfoManager->SetProfileInteraction("Player", "EnemyProjectile", ECollisionInteraction::Overlap);

		InfoManager->SetProfileInteraction("PlayerTrigger", "Enemy", ECollisionInteraction::Overlap);
		InfoManager->SetProfileInteraction("Enemy", "PlayerTrigger", ECollisionInteraction::Overlap);
		
		InfoManager->SetProfileInteraction("EnemyTrigger", "Player", ECollisionInteraction::Overlap);
		InfoManager->SetProfileInteraction("Player", "EnemyTrigger", ECollisionInteraction::Overlap);

		InfoManager->SetProfileInteraction("Wall", "Wall", ECollisionInteraction::Ignore);
		InfoManager->SetProfileInteraction("Enemy", "Wall", ECollisionInteraction::Block);
		InfoManager->SetProfileInteraction("Player", "Wall", ECollisionInteraction::Block);
	}
}
