#pragma once
#include "DataAsset.h"
#include "EnemyTypes.h"

class CBossData :
    public CDataAsset
{
public:
    CBossData();

    float HP, Speed, Damage;
    std::vector<FMovementType> MovementTypes;
    std::vector<std::unordered_multimap<ESkillTrigger, std::shared_ptr<class CSkillData>>> Skills;
};

