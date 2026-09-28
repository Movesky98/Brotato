#pragma once

#include "EngineInfo.h"

class CStatusInterface
{

protected:
	float mHP = 10.f;
	float mHPMax = 10.f;

	bool mIsDead = false;

	std::function<void()> mDeathFuncs;
	std::function<void(float /* Current */, float /* Max */)> mUpdateHPFunc;

	void SetHPMax(float HPMax)
	{
		mHPMax = HPMax;
	}

	virtual void SetHP(float HP) = 0;

	void AddHP(float HP);

public:
	float GetHP() const
	{
		return mHP;
	}

	bool IsDead() const
	{
		return mIsDead;
	}
	
	float GetMaxHP() const
	{
		return mHPMax;
	}

public:
	template<typename T>
	void SetDeathFunction(T* Object, void (T::* Func)())
	{
		mDeathFuncs = std::bind(Func, Object);
		// mDeathFuncs.push_back(BindFunc);
	}

	template<typename T>
	void SetUpdateHPFunction(T* Object, void (T::* Func)(float, float))
	{
		mUpdateHPFunc = std::bind(Func, Object, std::placeholders::_1, std::placeholders::_2);
	}
};

