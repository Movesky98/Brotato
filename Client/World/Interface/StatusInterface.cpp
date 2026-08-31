#include "StatusInterface.h"

void CStatusInterface::AddHP(float HP)
{
	mHP += HP;

	if (mHP <= 0.f)
	{
		CallDeathFuncs();
	}
	else if (mHP > mHPMax)
	{
		mHP = mHPMax;
	}

	if(mUpdateHPFunc)
		mUpdateHPFunc(mHP, mHPMax);
}

void CStatusInterface::CallDeathFuncs()
{
	for (auto it : mDeathFuncs)
	{
		if (it)
		{
			it();
		}
	}

}
