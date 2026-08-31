#pragma once
#include "EnemyTypes.h"

class CSkillData
{
public:
    CSkillData() { }
    virtual ~CSkillData() = default;

    ESkillComponent Component = ESkillComponent::None;
    ESkillTarget Target = ESkillTarget::None;
    float Cooldown = 0.f;
};

