#pragma once
#include "World/Actor.h"

class CProjectile :
    public CActor
{
public:
	CProjectile();
	CProjectile(const CProjectile& src);
	CProjectile(CProjectile&& src) noexcept;
	virtual ~CProjectile();
public:
	virtual bool Init();
protected:
	virtual CProjectile* Clone()
	{
		return new CProjectile(*this);
	}
};

