#include "Projectile.h"

CProjectile::CProjectile()
{
}

CProjectile::CProjectile(const CProjectile& src)
    : CActor(src)
{
}

CProjectile::CProjectile(CProjectile&& src) noexcept
    : CActor(std::move(src))
{
}

CProjectile::~CProjectile()
{
}

bool CProjectile::Init()
{
    if (!CActor::Init()) return false;

    return true;
}
