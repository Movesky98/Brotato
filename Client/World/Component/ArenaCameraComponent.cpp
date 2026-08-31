#include "ArenaCameraComponent.h"

#include "../MainWorld.h"

CArenaCameraComponent::CArenaCameraComponent()
{
}

CArenaCameraComponent::CArenaCameraComponent(const CArenaCameraComponent& src)
    : CCameraComponent(src)
{
}

CArenaCameraComponent::CArenaCameraComponent(CArenaCameraComponent&& src) noexcept
    : CCameraComponent(std::move(src))
{
}

CArenaCameraComponent::~CArenaCameraComponent()
{
}

bool CArenaCameraComponent::Init()
{
    if (!CCameraComponent::Init()) return false;

    // 플레이어한테 따라붙는 카메라의 이동 범위를 어떻게 제한할건가?
    // 1. 맵 영역 - 카메라의 해상도 크기 만큼 빼는 방법
    // 이건 World로부터 맵의 크기를 전달받을 수 있음 (ArenaDesc)
    // 맵의 크기를 알 수 있고, 해상도 크기를 구할 수 있는 방법이 있다면 그냥 뺴면 되니까 가능할 듯 함.
    // 
    // 2. 뭔가 방법이 있을 것 같은데 모르겠음.

    auto World = mWorld.lock();
    if (auto MainWorld = std::dynamic_pointer_cast<CMainWorld>(World))
    {
        const FArenaDesc ArenaDesc = MainWorld->GetArenaDesc();
        mLimitXPos = ArenaDesc.CountX * ArenaDesc.TileSize / 2 - mWidth / 2;
        mLimitYPos = ArenaDesc.CountY * ArenaDesc.TileSize / 2 - mHeight / 2;
    }

    return true;
}

void CArenaCameraComponent::Update(float DeltaTime)
{
    CCameraComponent::Update(DeltaTime);

    auto Owner = mOwner.lock();
    if (Owner)
    {
        FVector2 CameraPos = FVector2(Owner->GetWorldPos().x, Owner->GetWorldPos().y);

        if (std::abs(CameraPos.x) > mLimitXPos)
        {
            CameraPos.x = CameraPos.x > 0 ? mLimitXPos : -mLimitXPos;
        }
        
        if (std::abs(CameraPos.y) > mLimitXPos)
        {
            CameraPos.y = CameraPos.y > 0 ? mLimitXPos : -mLimitXPos;
        }

        SetWorldPos(CameraPos);
    }
}

CArenaCameraComponent* CArenaCameraComponent::Clone() const
{
    return new CArenaCameraComponent(*this);
}
