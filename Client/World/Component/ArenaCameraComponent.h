#pragma once
#include "World/CameraComponent.h"
class CArenaCameraComponent : public CCameraComponent
{
public:
	CArenaCameraComponent();
	CArenaCameraComponent(const CArenaCameraComponent& src);
	CArenaCameraComponent(CArenaCameraComponent&& src) noexcept;
	virtual ~CArenaCameraComponent();

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);

public:
	virtual CArenaCameraComponent* Clone() const;

protected:
	float mLimitXPos = 0.f;
	float mLimitYPos = 0.f;
};

