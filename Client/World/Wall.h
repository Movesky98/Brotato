#pragma once
#include "World/Actor.h"
class CWall :
    public CActor
{
public:
	CWall();
	CWall(const CWall& src);
	CWall(CWall&& src) noexcept;
	virtual ~CWall();

public:
	virtual bool Init();

protected:
	virtual CWall* Clone()
	{
		return new CWall(*this);
	}

protected:
	std::weak_ptr<class CColliderBox2D> mBoxCollision;

public:
	void SetSize(FVector2 Size);
};

