#pragma once
#include "DataAsset.h"
#include "EnemyTypes.h"
#include "SkillData.h"

class CEnemyData :
    public CDataAsset
{
public:
    CEnemyData();

    float HP, Speed, Damage;
    std::vector<EEnemyMovement> Movement;
    std::unordered_multimap<ESkillTrigger, std::shared_ptr<CSkillData>> Skills;
    
};

