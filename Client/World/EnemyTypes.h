#pragma once

// 적 이동 방식
enum class EEnemyMovement
{
	None,
	ChaseTarget,
	FleeFromTarget,
	Random,
	MAX
};

// 스킬 발동 조건
enum class ESkillTrigger
{
	None,
	OnHit,
	InPerceptionRange,
	OnDeath,
	Always,
	MAX
};

// 스킬이 생성할 컴포넌트 종류
enum class ESkillComponent
{
	None,
	Dash,
	SpawnProjectile,
	MAX
};

// 스킬 대상
enum class ESkillTarget
{
	None,
	Target,
	Owner,
	MAX
};

enum class EExpansionMode
{
	None = -1,
	Inflate,
	Deflate,
	MAX
};
