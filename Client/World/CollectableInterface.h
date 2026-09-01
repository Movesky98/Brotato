#pragma once

#include "EngineInfo.h"
#include "World/Actor.h"

class ICollectableInterface
{
public:
	virtual void Collect(const std::weak_ptr<CActor>& W_Collector, std::function<void(int)> BindFunc) = 0;
};

