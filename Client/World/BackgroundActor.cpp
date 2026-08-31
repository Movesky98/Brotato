#include "BackgroundActor.h"
#include "World/MeshComponent.h"

CBackgroundActor::CBackgroundActor()
{
}

CBackgroundActor::CBackgroundActor(const CBackgroundActor& src)
    : CActor(src)
{
}

CBackgroundActor::CBackgroundActor(CBackgroundActor&& src) noexcept
    : CActor(std::move(src))
{
}

CBackgroundActor::~CBackgroundActor()
{
}

bool CBackgroundActor::Init()
{
    if (!CActor::Init()) return false;

    mMeshComp = CreateComponent<CMeshComponent>("MeshComp");

    auto MeshComponent = mMeshComp.lock();
    if (MeshComponent)
    {
        MeshComponent->SetMesh("TexRect");
        MeshComponent->SetShader("Material");
        MeshComponent->AddTexture(0, "Arena_BackgorundImage", TEXT("Custom/Tile/Background_Gradient_01.png"));
        MeshComponent->SetRelativeScale(10000.f, 10000.f);

        float r = (float)(rand() % 100);
        float g = (float)(rand() % 100);
        float b = (float)(rand() % 100);

        r /= 100.f;
        g /= 100.f;
        b /= 100.f;


        MeshComponent->SetBaseColor(0, r, g, b, 1.f);
    }

    return true;
}
