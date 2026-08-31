#pragma once

enum class EWeaponType
{
	None = -1,
	Melee,
	Ranged,
	MAX
};

enum class EPrimaryStats
{
	None = -1,
	MaxHP,				// 최대 HP
	HPRegeneration,		// HP 재생
	LifeSteal,			// 생명 훔침 (%)
	Damage,				// 대미지 (%)
	MeleeDamage,		// 근거리 대미지
	RangedDamage,		// 원거리 대미지
	ElementalDamage,	// 원소 대미지
	AttackSpeed,		// 공격 속도
	CritChance,			// 치명타율 
	Engineering,		// 엔지니어링
	Range,				// 범위
	Armor,				// 방어구
	Dodge,				// 회피
	Speed,				// 속도
	Luck,				// 행운
	Harvesting,			// 수확
	MAX
};

enum class ESecondaryStats
{
	None = -1,
	ConsumableHeal,			// 소모품 치료
	MaterialsHealing,		// 재료 치료
	XPGain,					// XP 획득
	PickupRange,			// 획득 범위
	ItemsPrice,				// 아이템 가격
	ExplosionDamage,		// 폭발 대미지
	ExplosionSize,			// 폭발 크기
	Bounces,				// 튕김
	Piercing,				// 관통
	PiercingDamage,			// 관통 대미지
	DamageAgainstBosses,	// 보스에 대한 대미지
	StructureAttackSpeed,	// 건축물 공격 속도
	StructureRange,			// 건축물 범위
	BurningSpeed,			// 화상 속도
	BurningSpread,			// 화상 범위(?)
	Knockback,				// 넉백
	DoubleMaterialChance,	// 재료 두배 확률
	FreeRerolls,			// 무료 초기화
	Trees,					// 나무
	Enemies,				// 적
	EnemySpeed,				// 적 속도
	MAX
};

struct FStat
{
	int Primary[(int)EPrimaryStats::MAX] = { 0, };
	int Secondary[(int)ESecondaryStats::MAX] = { 0, };

	void operator+=(const FStat& Other)
	{
		for (int i = 0; i < (int)EPrimaryStats::MAX; ++i)
		{
			Primary[i] += Other.Primary[i];
		}

		for (int i = 0; i < (int)ESecondaryStats::MAX; ++i)
		{
			Secondary[i] += Other.Secondary[i];
		}
	}
};