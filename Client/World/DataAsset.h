#pragma once

#include "EngineInfo.h"
#include "GameTypes.h"

struct FTextureDesc
{
	std::wstring Path;
	float SizeX;
	float SizeY;
	std::wstring IconPath;
	float IconSizeX;
	float IconSizeY;
};

enum class EDataAssetType
{
	None = -1,
	Character,
	Enemy,
	Item,
	Weapon,
	MAX
};

class CDataAsset
{
public:
	CDataAsset();
	virtual ~CDataAsset() = default;

	int ID;
	std::string Name;
	EDataAssetType AssetType;
	FTextureDesc TextureInfo;
	std::string Description;
};
