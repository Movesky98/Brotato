#include "MeleeWeapon.h"

#include "WeaponData.h"

#include "Enemy.h"
#include "EnemyBullet.h"

#include "World/World.h"
#include "World/MeshComponent.h"
#include "World/ColliderBox2D.h"
#include "World/MovementComponent.h"

CMeleeWeapon::CMeleeWeapon()
{
}

CMeleeWeapon::CMeleeWeapon(const CMeleeWeapon& src)
    : CWeapon(src)
{
}

CMeleeWeapon::CMeleeWeapon(CMeleeWeapon&& src) noexcept
    : CWeapon(std::move(src))
{
}

CMeleeWeapon::~CMeleeWeapon()
{
}

bool CMeleeWeapon::Init()
{
    if (!CWeapon::Init()) return false;

    mMoveComp = CreateComponent<CMovementComponent>("MoveComp");
    mBoxCollision = CreateComponent<CColliderBox2D>("BoxCollision");

    auto MoveComp = mMoveComp.lock();
    if (MoveComp)
    {
        MoveComp->SetUpdateComp(mRoot);
        MoveComp->SetSpeed(mDefaultSpeed);
        MoveComp->SetCurrentSpeed(mDefaultSpeed);
    }

    return true;
}

void CMeleeWeapon::Update(float DeltaTime)
{
    CWeapon::Update(DeltaTime);

    if (!mAttackEnded)
    {
        auto Owner = mOwner.lock();
        auto Target = mTarget.lock();
        if (Owner && Target)
        {
            // 거리 항상 체크
            FVector3 Direction = Target->GetWorldPos() - Owner->GetWorldPos();
            mDistance = Direction.Length();

            // SwingAngle 구해야 함.
            float CurrentRadian = std::atan2f(Direction.y, Direction.x);
            mSwingAngle = CurrentRadian * 180.f / 3.14159265f;
        }

        HandleAttackPhase(DeltaTime);
    }
}

void CMeleeWeapon::Destroy()
{
    CWeapon::Destroy();

}

void CMeleeWeapon::InitializeSettings(const std::weak_ptr<CWeaponData>& InData, const FStat& Stat)
{
    auto Data = InData.lock();
    if (!Data) return;

    mBaseDamage = Data->BaseDamage + Stat.Primary[(int)EPrimaryStats::MeleeDamage];
    mBaseDamage += mBaseDamage * (float)(Stat.Primary[(int)EPrimaryStats::Damage] / 100.f);
    if (mBaseDamage < 1) mBaseDamage = 1;

    mCooldown = Data->Cooldown;
    mKnockback = Data->Knockback;
    mCritical = Data->Critical;
    mRange = Data->Range;
    mLifeSteal = Data->LifeSteal;
    mPiercing = Data->Piercing;
    mRange = Data->Range + Stat.Primary[(int)EPrimaryStats::Range];

    CWeapon::InitializeSettings(InData, Stat);
}

void CMeleeWeapon::Attack(std::weak_ptr<class CEnemy> TargetPtr)
{
    // TODO : 적 있는 곳에 다가가서 개패야 함.
    // 1. 기본 위치 (RelativePos) 저장
    // 2. 플레이어(Owner)와 적 (Target)과의 거리 계산
    // 3. 휘두르는 각도(예: 타겟 기준 ± Angle) 가 존재해야 함.
    // 4. 플레이어 기준으로 플레이어 ↔ 적 거리만큼 각도 [타겟 기준 - Angle, 타겟 기준 + Angle] 까지 휘두름
    // 5. 타겟 기준 + Angle 까지 갔으면 -> 원래 위치로 돌아오도록
    // 6. 위의 1 ~ 5번이 끝나면 mAttackEnded = true;
  

    auto Owner = mOwner.lock();
    auto Target = TargetPtr.lock();
    if (Owner && Target)
    {
        mTarget = Target;
        mSpawnDirection = GetWorldPos() - Owner->GetWorldPos();
        mSpawnDirection.Normalize();
        FVector3 Direction = Target->GetWorldPos() - Owner->GetWorldPos();
        mDistance = Direction.Length();

        // SwingAngle 구해야 함.
        float CurrentRadian = std::atan2f(Direction.y, Direction.x);
        mSwingAngle = CurrentRadian * 180.f / 3.14159265f;

        // Owner(Player) 위치에서부터 접근 시작
        SetWorldPos(Owner->GetWorldPos());
        mAttackPhase = EMeleeAttackPhase::Swing;

        /* Swing 진입 시 */
        mDeltaAngle = mSwingAngle - mSpread;

        auto MoveComp = mMoveComp.lock();
        auto BoxCollision = mBoxCollision.lock();

        if(MoveComp) MoveComp->SetSpeed(mDefaultSpeed * 1.2f);
        if (BoxCollision) BoxCollision->SetEnable(true);

    }
}

void CMeleeWeapon::HandleAttackPhase(float DeltaTime)
{
    switch (mAttackPhase)
    {
    case EMeleeAttackPhase::None:
        mAttackEnded = true;
        mTarget.reset();
        mSpawnDirection = FVector3::Zero;
        mDeltaDistance = 0.f;
        mDistance = 0.f;
        mSwingAngle = 0.f;
        break;
    case EMeleeAttackPhase::Approach:
    {
        const float Angle = mSwingAngle - mSpread;
        const float Radian = DirectX::XMConvertToRadians(Angle);
        FVector3 Direction = FVector3(cosf(Radian), sinf(Radian), GetWorldPos().z);

        auto MoveComp = mMoveComp.lock();
        if(MoveComp)
        {
            mDeltaDistance += DeltaTime * MoveComp->GetMoveSpeed();

            if (mDeltaDistance <= mDistance)
            {
                MoveComp->SetMoveDir(Direction);
            }
            else
            {
                // Swing으로 진입
                mAttackPhase = EMeleeAttackPhase::Swing;
                mDeltaAngle = mSwingAngle - mSpread;

                MoveComp->SetSpeed(mDefaultSpeed * 1.2f);

                auto BoxCollision = mBoxCollision.lock();
                if (BoxCollision)
                {
                    BoxCollision->SetEnable(true);
                }
            }
        }
    }
        break;
    case EMeleeAttackPhase::Swing:
    {
        auto Owner = mOwner.lock();
        auto MoveComp = mMoveComp.lock();
        if(MoveComp && Owner)
        {
            // DeltaAngle을 토대로 Direciton 생성
            const float Radian = DirectX::XMConvertToRadians(mDeltaAngle);
            FVector3 Direction = FVector3(cosf(Radian), sinf(Radian), GetWorldPos().z);
            Direction.Normalize();
            FVector3 GoalPos = Owner->GetWorldPos() + Direction * mDistance;

            SetWorldPos(GoalPos);

            /*auto World = mWorld.lock();
            if (World)
            {
                auto Bullet = World->CreateActor<CEnemyBullet>("Bullet").lock();

                if (Bullet)
                {
                    Bullet->SetWorldPos(GoalPos);
                    Bullet->SetMoveDir(FVector3::Zero);
                }
            }*/

            mDeltaAngle += DeltaTime * MoveComp->GetMoveSpeed();
            if (mDeltaAngle >= mSwingAngle + mSpread)
            {
                // Return으로 진입
                mAttackPhase = EMeleeAttackPhase::Return;

                auto BoxCollision = mBoxCollision.lock();
                if (BoxCollision)
                {
                    BoxCollision->SetEnable(false);
                }
            }
        }
    }
        break;
    case EMeleeAttackPhase::Return:
    {
        auto MoveComp = mMoveComp.lock();
        auto Owner = mOwner.lock();
        if (Owner)
        {
            FVector3 SpawnPos = Owner->GetWorldPos() + mSpawnDirection * 50.f;
            FVector3 Direction = SpawnPos - GetWorldPos();
            Direction.Normalize();

            MoveComp->SetMoveDir(Direction);

            if (SpawnPos.Distance(GetWorldPos()) <= 10.f)
            {
                MoveComp->SetMoveDir(FVector3::Zero);
                MoveComp->SetSpeed(mDefaultSpeed);
                SetWorldPos(SpawnPos);
                mAttackPhase = EMeleeAttackPhase::None;
            }
        }
    }
        break;
    default:
        break;
    }
}

void CMeleeWeapon::OnHitCollisionBeginOverlap(const FVector3& HitPoint, const FVector3& Normal, std::weak_ptr<class CCollider> Collider)
{
    auto _Collider = Collider.lock();
    if (_Collider)
    {
        auto Owner = _Collider->GetOwner().lock();
        if (Owner && Owner->GetActorTag() == "Enemy")
        {
            Owner->TakeDamage(mBaseDamage, "Player", GetThisPtr<CActor>());
        }
    }
}

void CMeleeWeapon::SetTexture(const std::string& Name, const std::wstring& TexturePath, float SizeX, float SizeY)
{
    CWeapon::SetTexture(Name, TexturePath, SizeX, SizeY);

    auto MeshComp = mMeshComp.lock();
    auto BoxCollision = mBoxCollision.lock();
    if (BoxCollision && MeshComp)
    {
        FVector2 BoxSize = FVector2(MeshComp->GetRelativeScale().x, MeshComp->GetRelativeScale().y);

        BoxCollision->SetBoxSize(BoxSize);
        BoxCollision->SetCollisionProfile("DetectEnemy");
        BoxCollision->SetEnable(false);
        BoxCollision->SetDebugDraw(true);
        BoxCollision->SetBeginOverlapFunc(this, &CMeleeWeapon::OnHitCollisionBeginOverlap);
    }
}
