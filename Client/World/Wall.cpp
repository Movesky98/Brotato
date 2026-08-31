#include "Wall.h"

#include "World/ColliderBox2D.h"

CWall::CWall()
{
}

CWall::CWall(const CWall& src)
    : CActor(src)
{
}

CWall::CWall(CWall&& src) noexcept
    : CActor(std::move(src))
{
}

CWall::~CWall()
{
}

bool CWall::Init()
{
    if (!CActor::Init()) return false;

    mBoxCollision = CreateComponent<CColliderBox2D>("BoxCollision");

    auto BoxCollision = mBoxCollision.lock();
    if (BoxCollision)
    {
        BoxCollision->SetInheritScale(false);
        BoxCollision->SetDebugDraw(true);
        BoxCollision->SetCollisionProfile("BlockAll");
    }

    return true;
}

void CWall::SetSize(FVector2 Size)
{
    auto BoxCollision = mBoxCollision.lock();
    if (BoxCollision)
    {
        BoxCollision->SetBoxSize(Size);
    }

}
