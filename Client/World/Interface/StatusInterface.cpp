#include "StatusInterface.h"

void CStatusInterface::AddHP(float HP)
{
	mHP += HP;

	if (mHP <= 0.f)
	{
		mDeathFuncs();
	}
	else if (mHP > mHPMax)
	{
		mHP = mHPMax;
	}

	if(mUpdateHPFunc)
		mUpdateHPFunc(mHP, mHPMax);
}
