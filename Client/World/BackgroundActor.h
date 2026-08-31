#pragma once
#include "World/Actor.h"
class CBackgroundActor :
    public CActor
{
public:
	CBackgroundActor();
	CBackgroundActor(const CBackgroundActor& src);
	CBackgroundActor(CBackgroundActor&& src) noexcept;
	virtual ~CBackgroundActor();

public:
	virtual bool Init();

protected:
	virtual CBackgroundActor* Clone()
	{
		return new CBackgroundActor(*this);
	}

protected:
	std::weak_ptr<class CMeshComponent> mMeshComp;
};

