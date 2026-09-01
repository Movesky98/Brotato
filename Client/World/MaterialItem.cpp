#include "MaterialItem.h"

#include "World/MeshComponent.h"
#include "World/ColliderBox2D.h"
#include "World/MovementComponent.h"

#include "World/Actor.h"

#include <random>

#define MaterialTexture_NUM 11

CMaterialItem::CMaterialItem()
{
}

CMaterialItem::CMaterialItem(const CMaterialItem& src)
    : CActor(src)
{
}

CMaterialItem::CMaterialItem(CMaterialItem&& src) noexcept
    : CActor(std::move(src))
{
}

CMaterialItem::~CMaterialItem()
{
}

bool CMaterialItem::Init()
{
    if (!CActor::Init()) return false;

    mMeshComp = CreateComponent<CMeshComponent>("MeshComp");
    mMoveComp = CreateComponent<CMovementComponent>("MoveComp");
    mBoxCollision = CreateComponent<CColliderBox2D>("BoxCollision");

    auto MeshComp = mMeshComp.lock();
    if (MeshComp)
    {
        int Number = rand() % MaterialTexture_NUM;
        std::string Name = "Material_" + std::to_string(Number);
        std::wstring TexturePath = L"Custom/Material/material_" + std::to_wstring(Number) + L".png";

        MeshComp->SetMesh("TexRect");
        MeshComp->SetShader("Material");
        MeshComp->AddTexture(0, Name, TexturePath.c_str());
        MeshComp->SetBlendState(0, "AlphaBlend");
        MeshComp->SetRelativeScale(40.f, 40.f);
        MeshComp->SetBaseColor(0, FVector4(0.462745f, 1.0f, 0.462745f, 1.f));
    }

    auto MoveComp = mMoveComp.lock();
    if (MoveComp)
    {
        MoveComp->SetUpdateComp(mRoot);
        MoveComp->SetSpeed(400.f);
        MoveComp->SetCurrentSpeed(400.f);
    }

    auto BoxCollision = mBoxCollision.lock();
    if (BoxCollision)
    {
        BoxCollision->SetInheritScale(true);
        BoxCollision->SetDebugDraw(false);
        BoxCollision->SetCollisionProfile("Collectable");
        BoxCollision->SetBoxSize(40.f, 40.f);
        BoxCollision->SetBeginOverlapFunc(this, &CMaterialItem::OnCollsiionBeginOverlap);
    }

    return true;
}

void CMaterialItem::Update(float DeltaTime)
{
    CActor::Update(DeltaTime);

    if (mTarget.expired()) return;

    auto Target = mTarget.lock();
    auto MoveComp = mMoveComp.lock();
    if (MoveComp)
    {
        FVector3 Direction = Target->GetWorldPos() - GetWorldPos();

        MoveComp->SetMoveDir(Direction);
    }
}

void CMaterialItem::Destroy()
{
    auto Target = mTarget.lock();
    if (Target && mOnAcquiredFunc)
    {
        mOnAcquiredFunc(1);
    }

    CActor::Destroy();
}

void CMaterialItem::Collect(const std::weak_ptr<CActor>& W_Collector, std::function<void(int)> BindFunc)
{
    auto Collector = W_Collector.lock();
    if (!Collector) return;

    if (BindFunc)
    {
        mOnAcquiredFunc = BindFunc;
    }

    mTarget = Collector;


    // 1. Overlap Enable
    // 2. 타겟 지정
    // 3. 타겟에게 이동
    // 4. Begin Overlap 때, 플레이어에게 닿으면 Destroy(), 재화++
}

void CMaterialItem::OnCollsiionBeginOverlap(const FVector3& HitPoint, const FVector3& Normal, std::weak_ptr<class CCollider> Collider)
{
    auto ColliderComp = Collider.lock();
    if (!ColliderComp) return;

    auto Profile = ColliderComp->GetCollisionProfile();
    if (Profile->Name == "PickupRange") return;

    auto Player = ColliderComp->GetOwner().lock();
    if (Player && Player->GetActorTag() == "Player")
    {
        Destroy();
    }
}
