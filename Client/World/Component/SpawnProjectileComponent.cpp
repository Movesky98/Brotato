#include "SpawnProjectileComponent.h"
#include "../SpawnProjectileSkillData.h"

#include "../EnemyBullet.h"
#include "../Enemy.h"

#include "World/World.h"
#include "World/MeshComponent.h"
#include "LogManager.h"

CSpawnProjectileComponent::CSpawnProjectileComponent()
{
}

CSpawnProjectileComponent::CSpawnProjectileComponent(const CSpawnProjectileComponent& src)
	: CSkillComponent(src)
{
}

CSpawnProjectileComponent::CSpawnProjectileComponent(CSpawnProjectileComponent&& src) noexcept
	: CSkillComponent(std::move(src))
{
}

CSpawnProjectileComponent::~CSpawnProjectileComponent()
{
}

bool CSpawnProjectileComponent::Init()
{
	if (!CSkillComponent::Init()) return false;

	mSkillEnable = true;

	auto Owner = GetOwner().lock();
	if (Owner)
	{
		auto MeshComp = Owner->GetComponent<CMeshComponent>().lock();
		if (MeshComp)
		{
			mDefaultScale2D = FVector2(MeshComp->GetRelativeScale().x, MeshComp->GetRelativeScale().y);
		}
	}

	return true;
}

void CSpawnProjectileComponent::Update(float DeltaTime)
{
	CSkillComponent::Update(DeltaTime);

	if (mSkillStart && mSkillEnable)
	{
		HandleSkillByTargetType(mTargetType, DeltaTime);
	}
}

void CSpawnProjectileComponent::PostUpdate(float DeltaTime)
{
	CSkillComponent::PostUpdate(DeltaTime);
}

void CSpawnProjectileComponent::Destroy()
{
	CSkillComponent::Destroy();
}

bool CSpawnProjectileComponent::SkillProgress()
{
	if (!CSkillComponent::SkillProgress() || mSkillStart) return false;

	mSkillStart = true;

	// 어떤 Enemy 캐릭터는 타겟이 없을 수도 있음 (랜덤 뿌림)
	auto EnemyOwner = std::dynamic_pointer_cast<CEnemy>(GetOwner().lock());
	if (EnemyOwner)
	{
		EnemyOwner->SetCanScaleSize(false);
		auto Target = EnemyOwner->GetTarget().lock();
		if (Target)
		{
			mTarget = Target;
		}

		if (mTargetType == ESkillTarget::Target)
		{
			if (mExpansionMode == EExpansionMode::None)
			{
				mIsInProgressExpansion = true;
				mExpansionMode = EExpansionMode::Inflate;
			}
		}
	}

	return true;
}

void CSpawnProjectileComponent::SkillEnd()
{
	CSkillComponent::SkillEnd();

	mSpawnedBulletCount = 0;
	mElapsedSpawnTime = 0.f;
	mTarget.reset();
	mIsInProgressExpansion = false;
}

void CSpawnProjectileComponent::CalculateDirectionBySpread(FVector3& Direction)
{
	// TODO : 
	// 1. atan2f(y, x)를 이용해 현재 Direciton의 Radian을 구함.
	// 2. Radian * 180.f / π = Angle
	// 3. Angle = Angle ± Spread
	// 4. 변경된 Angle을 가지고 Direction 재 계산.

	FVector3 ZeroAngleVector(1.f, 0.f, 0.f);
	Direction.Normalize();

	float CurrentRadian = std::atan2f(Direction.y, Direction.x);
	float Angle = CurrentRadian * 180.f / 3.14159265f;

	float Spread = (float)(rand() % mSpawnSpread);
	int IsNegative = (rand() % 2) ? 1 : -1;

	Angle += IsNegative * Spread;
	const float GoalRadian = DirectX::XMConvertToRadians(Angle);
	
	Direction = FVector3(cosf(GoalRadian), sinf(GoalRadian), Direction.z);
}

void CSpawnProjectileComponent::ScaleOwnerExpansion(const EExpansionMode Mode, float DeltaTime)
{
	auto EnemyOwner = std::dynamic_pointer_cast<CEnemy>(mOwner.lock());
	if (!EnemyOwner) return;;

	auto MeshComp = EnemyOwner->GetComponent<CMeshComponent>().lock();
	if (!MeshComp) return;

	FVector2 CurrentScale2D(MeshComp->GetRelativeScale().x, MeshComp->GetRelativeScale().y);

	switch (Mode)
	{
	case EExpansionMode::None:
		CurrentScale2D = mDefaultScale2D;
		mIsInProgressExpansion = false;
		EnemyOwner->SetCanScaleSize(true);
		
		break;
	case EExpansionMode::Inflate:
	{
		CurrentScale2D += mDefaultScale2D * (mExpansionScale - 1.f) * (DeltaTime / mInflateTime);

		if (CurrentScale2D.x >= mDefaultScale2D.x * mExpansionScale || CurrentScale2D.y >= mDefaultScale2D.y * mExpansionScale)
		{
			CurrentScale2D = mDefaultScale2D * mExpansionScale;
			mSkillUse = true;
			mExpansionMode = EExpansionMode::Deflate;
		}
	}
		break;
	case EExpansionMode::Deflate:
		CurrentScale2D -= mDefaultScale2D * (mExpansionScale - 1.f) * (DeltaTime / mDeflateTime);

		if (CurrentScale2D.x <= mDefaultScale2D.x || CurrentScale2D.y <= mDefaultScale2D.y)
		{
			mExpansionMode = EExpansionMode::None;

		}
		break;
	default:
		break;
	}

	MeshComp->SetRelativeScale(CurrentScale2D);
}

void CSpawnProjectileComponent::HandleSkillByTargetType(ESkillTarget Type, float DeltaTime)
{
	switch (Type)
	{
	case ESkillTarget::None:
		mSkillUse = true;
		break;
	case ESkillTarget::Target:
		if (mIsInProgressExpansion)
			ScaleOwnerExpansion(mExpansionMode, DeltaTime);
		break;
	case ESkillTarget::Owner:
		mSkillUse = true;
		break;
	default:
		break;
	}


	if (mSkillUse)
	{
		mElapsedSpawnTime += DeltaTime;
		if (mElapsedSpawnTime >= mSpawnInterval)
		{
			mElapsedSpawnTime = 0.f;

			auto World = mWorld.lock();
			auto Owner = GetOwner().lock();
			if (!World || !Owner) return;

			if (mSpawnedBulletCount < mSpawnCount)
			{
				auto Bullet = World->CreateActor<CEnemyBullet>("Bullet").lock();
				if (!Bullet) return;

				// 랜덤, 타겟 유무에 따른 방향 설정과 Spread 적용해야 함.
				FVector3 Direction;

				switch (mTargetType)
				{
				case ESkillTarget::None:
				{
					float Angle = (float)(rand() % 360);
					const float Radian = DirectX::XMConvertToRadians(Angle);
					Direction = FVector3(cosf(Radian), sinf(Radian), Owner->GetWorldPos().z);
				}
				break;
				case ESkillTarget::Target:
				{
					// 현재 Direction에 대한 각도를 구한 후 ±Spread 만큼 더해주어 다시 Direction 계산하면 될 것 같기도 하고.

					auto Target = mTarget.lock();
					if (Target)
						Direction = Target->GetWorldPos() - Owner->GetWorldPos();
					else
						Direction = Owner->GetWorldPos();

					CalculateDirectionBySpread(Direction);
				}
				break;
				case ESkillTarget::Owner:
					// 따라다니는 Projectile을 만들어야 함.
					break;
				default:
					break;
				}

				Bullet->InitializeProjectile(Direction, 400.f);
				Bullet->SetWorldPos(Owner->GetWorldPos());

				++mSpawnedBulletCount;
			}
			else if (mExpansionMode == EExpansionMode::None)
			{
				SkillEnd();
			}
		}
	}
}

void CSpawnProjectileComponent::InitializeProperties(const std::shared_ptr<const CSkillData>& SkillData)
{
	if (!SkillData) return;

	auto SP_SkillProps = std::dynamic_pointer_cast<const CSpawnProjectileSkillData>(SkillData);
	if (SP_SkillProps)
	{
		mSkillCoolTime = SP_SkillProps->Cooldown;
		mSpawnCount = SP_SkillProps->Count;
		mSpawnInterval = SP_SkillProps->Interval;
		mSpawnSpread = SP_SkillProps->Spread;
		mTargetType = SP_SkillProps->Target;

		// 나중에는 TargetType도 저장해야 함.
		// 언제? 보스 구현 때 -> FollowProjectile 생성해야 함.
		// Target 설정할 때, Target, Owner, Random을 구분하도록
	}
}
