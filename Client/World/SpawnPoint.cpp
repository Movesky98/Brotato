#include "SpawnPoint.h"

#include "MainWorld.h"
#include "DataAsset.h"

#include "World/MeshComponent.h"
#include "World/ColliderBox2D.h"

CSpawnPoint::CSpawnPoint()
{
}

CSpawnPoint::CSpawnPoint(const CSpawnPoint& src)
    : CActor(src)
{
}

CSpawnPoint::CSpawnPoint(CSpawnPoint&& src) noexcept
    : CActor(std::move(src))
{
}

CSpawnPoint::~CSpawnPoint()
{
}

bool CSpawnPoint::Init()
{
    if (!CActor::Init()) return false;

    mMeshComponent = CreateComponent<CMeshComponent>("MeshComponent");
    mBoxCollision = CreateComponent<CColliderBox2D>("BoxCollision");

    auto MeshComponent = mMeshComponent.lock();
    if (MeshComponent)
    {
        MeshComponent->SetMesh("TexRect");
        MeshComponent->SetShader("Material");
        MeshComponent->AddTexture(0, "SpawnPoint_X", TEXT("Custom/Spawn/entity_birth.png"));
        MeshComponent->SetBaseColor(0, FVector4::Red);
        MeshComponent->SetBlendState(0, "AlphaBlend");
        MeshComponent->SetRelativeScale(56.f, 56.f);
    }

    auto BoxCollision = mBoxCollision.lock();
    if (BoxCollision)
    {
        BoxCollision->SetInheritScale(false);
        BoxCollision->SetDebugDraw(false);
        BoxCollision->SetBoxSize(56.f, 56.f);
        BoxCollision->SetCollisionProfile("DetectPlayer");
        BoxCollision->SetBeginOverlapFunc(this, &CSpawnPoint::OnCollisionBeginOverlap);
    }

    mBlinkInterval = mSpawnDelayTime / 2.f;

    return true;
}

void CSpawnPoint::Update(float DeltaTime)
{
    CActor::Update(DeltaTime);

    mElapsedTime += DeltaTime;
    if (mElapsedTime > mSpawnDelayTime)
    {
        Destroy();
    }

    if (mElapsedTime > mBlinkInterval)
    {
        auto MeshComponent = mMeshComponent.lock();
        if (MeshComponent)
        {
            MeshComponent->SetRenderEnable(!MeshComponent->IsRenderEnable());
        }

        float Diff = mElapsedTime - mBlinkInterval;
        float RemainingTime = mSpawnDelayTime - mBlinkInterval;

        mBlinkInterval += RemainingTime / 5.f;
    }
}

void CSpawnPoint::Destroy()
{
    if(mOnSpawnEntityDelegate)
        mOnSpawnEntityDelegate(GetWorldPos(), mSpawnType, mDataAsset);

    CActor::Destroy();
}

void CSpawnPoint::OnCollisionBeginOverlap(const FVector3& HitPoint, const FVector3& Normal, std::weak_ptr<class CCollider> Collider)
{
    auto ColliderComp = Collider.lock();
    if (!ColliderComp) return;

    auto ColliderActor = ColliderComp->GetOwner().lock();
    if (ColliderActor && ColliderActor->GetActorTag() == "Player")
    {
        auto MainWorld = std::dynamic_pointer_cast<CMainWorld>(mWorld.lock());
        if (MainWorld)
        {
            SetWorldPos(MainWorld->GetRandomSpawnPosition());
            
            mElapsedTime = 0;
            mBlinkInterval = mSpawnDelayTime / 2.f;
        }
    }
}

void CSpawnPoint::InitializeSpawnSettings(ESpawnType SpawnType, std::weak_ptr<class CDataAsset> Data)
{
    mSpawnType = SpawnType;
    mDataAsset = Data;
}
