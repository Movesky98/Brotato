#pragma once

#include "EngineInfo.h"
#include "GameTypes.h"

// 웨이브 하나에 등장할 적 구성.
// 액터에게 넘겨지는 자산이 아니라 월드가 조회하는 표라서 CDataAsset을 상속하지 않는다.
class CWaveData
{
public:
	CWaveData();
	virtual ~CWaveData() = default;

	int Wave = 0;
	std::vector<FWaveSpawnDesc> Spawns;
	int Boss = 0;
};
