#include "EditorWorld.h"

#include "World/TileMapActor.h"
#include "World/TileMapComponent.h"
#include "World/UIManager.h"

#include "EditorPlayer.h"
#include "EditorWidget.h"

CEditorWorld::CEditorWorld()
{}

CEditorWorld::~CEditorWorld()
{}

bool CEditorWorld::Init()
{
	CWorld::Init();

	auto Player = CreateActor<CEditorPlayer>("Player");

	auto UIMgr = GetUIManager().lock();

	if (UIMgr)
	{
		UIMgr->CreateWidgetContainer<CEditorWidget>("EditorWidget");
	}

	auto TileMap = CreateActor<CTileMapActor>("TileMap").lock();

	if (TileMap)
	{
		auto TileMapComp = TileMap->GetTileMapComp().lock();

		if (TileMapComp)
		{
			TileMapComp->CreateTile(ETileShape::Rect, 50, 50, FVector2(50.f, 50.f), -1, true);

			TileMapComp->SetTileTexture(ETileTextureType::Back, "BackTexture", TEXT("Custom/Tile/Background_Gradient.png"));
			TileMapComp->SetTileTexture(ETileTextureType::Tile, "TileTexture", TEXT("Custom/Tile/tiles_1.png"));


			//스프라이트시트인 타일텍스처의 프레임을 지정해준다.
			for (int i = 0; i < 3; ++i)
			{
				for(int j = 0; j < 4; ++j)
				{
					TileMapComp->AddTileFrame(
						i * 64.f, 
						j * 64.f, 
						64.f + i * 64.f,
						64.f + j * 64.f);
				}

			}
		}
	}

	return true;
}
