#pragma once
#include "Weapon.h"
class CRangedWeapon :
    public CWeapon
{
public:
	CRangedWeapon();
	CRangedWeapon(const CRangedWeapon& src);
	CRangedWeapon(CRangedWeapon&& src) noexcept;
	virtual ~CRangedWeapon();

protected:

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void Destroy();

protected:
	virtual CRangedWeapon* Clone()
	{
		return new CRangedWeapon(*this);
	}

	void InitializeSettings(const std::weak_ptr<CWeaponData>& InData, const FStat& Stat) override;

	void Attack(std::weak_ptr<class CEnemy> TargetPtr) override;
};

