#pragma once
#include "SkillData.h"
class CSpawnEnemySkillData : public CSkillData
{
public:
	int Count = 0;
	float Interval = 0.f;
	int EnemyID = 0;
};
