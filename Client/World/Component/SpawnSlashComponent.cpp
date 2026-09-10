#include "SpawnSlashComponent.h"

#include "../SpawnSlashSkillData.h"
#include "../Slash.h"

#include "../Enemy.h"

#include "World/World.h"

CSpawnSlashComponent::CSpawnSlashComponent()
{
}

CSpawnSlashComponent::CSpawnSlashComponent(const CSpawnSlashComponent& src)
    : CSkillComponent(src)
{
}

CSpawnSlashComponent::CSpawnSlashComponent(CSpawnSlashComponent&& src) noexcept
    : CSkillComponent(std::move(src))
{
}

CSpawnSlashComponent::~CSpawnSlashComponent()
{
}

bool CSpawnSlashComponent::Init()
{
    if (!CSkillComponent::Init()) return false;


    return true;
}

void CSpawnSlashComponent::Update(float DeltaTime)
{
    CSkillComponent::Update(DeltaTime);

}

bool CSpawnSlashComponent::SkillProgress()
{
	if (!CSkillComponent::SkillProgress()) return false;
	
	switch (SlashMoveMode)
	{
	case ESlashMoveMode::Stop:
		CreateStaticSlash();
		break;
	case ESlashMoveMode::Move:
		CreateMovableSlash();
		break;
	default:
		break;
	}

	SkillEnd();

    return true;
}

void CSpawnSlashComponent::SkillEnd()
{
    CSkillComponent::SkillEnd();

}

void CSpawnSlashComponent::CreateStaticSlash()
{
	auto World = mWorld.lock();
	auto Owner = std::dynamic_pointer_cast<CEnemy>(mOwner.lock());
	if (World && Owner)
	{
		auto Target = Owner->GetTarget().lock();
		if (!Target) return;

		FVector3 SpawnPos = Target->GetWorldPos();

		for (int i = 0; i < mSpawnCount; i++)
		{

			auto Slash = World->CreateActor<CSlash>("Slash").lock();
			if (Slash)
			{
				const float Angle = (float)(rand() % mSpawnSpread);
				const float Radian = DirectX::XMConvertToRadians(Angle);
				FVector3 Direction = FVector3(cosf(Radian), sinf(Radian), 0.f);

				Slash->SetWorldPos(SpawnPos);
				Slash->InitializeProperties(Direction, 2.f, SlashMoveMode);
			}
		}
	}
}

void CSpawnSlashComponent::CreateMovableSlash()
{
	auto World = mWorld.lock();
	auto Owner = std::dynamic_pointer_cast<CEnemy>(mOwner.lock());
	if (World && Owner)
	{
		auto Target = Owner->GetTarget().lock();
		if (!Target) return;

		FVector3 BaseDirection = Target->GetWorldPos() - Owner->GetWorldPos();
		BaseDirection.Normalize();

		float Radian = std::atan2f(BaseDirection.y, BaseDirection.x);
		float Angle = DirectX::XMConvertToDegrees(Radian);

		for (int i = 0; i < mSpawnCount; i++)
		{
			auto Slash = World->CreateActor<CSlash>("Slash").lock();
			if (Slash)
			{
				float CurrentAngle = GetAimAngleToTarget(Angle, i);
				float CurrentRadian = DirectX::XMConvertToRadians(CurrentAngle);
				FVector3 Direction = FVector3(cosf(CurrentRadian), sinf(CurrentRadian), 0.f);
				Direction.Normalize();

				FVector3 BasePos = Target->GetWorldPos() - BaseDirection * 150.f;
				FVector3 SpawnPos = BasePos + Direction * 75.f;

				Slash->SetWorldPos(SpawnPos);
				Slash->InitializeProperties(Direction, 2.f, SlashMoveMode);
			}
		}
	}
}

float CSpawnSlashComponent::GetAimAngleToTarget(float BaseAngle, int Index)
{
	const int MaxIndex = mSpawnCount - 1;
	if (!MaxIndex) return BaseAngle;

	const float StartAngle = BaseAngle - mSpawnSpread;
	const float TotalAngle = mSpawnSpread * 2.f;

	const float AngleStep = TotalAngle / (float)MaxIndex;

	return StartAngle + AngleStep * Index;
}

void CSpawnSlashComponent::InitializeProperties(const std::shared_ptr<const CSkillData>& SkillData)
{
	auto SkillProps = std::dynamic_pointer_cast<const CSpawnSlashSkillData>(SkillData);
	if (SkillProps)
	{
		mSkillCoolTime	= SkillProps->Cooldown;
		mSpawnCount		= SkillProps->Count;
		mSpawnInterval	= SkillProps->Interval;
		mSpawnSpread	= SkillProps->Spread;
		mTargetType		= SkillProps->Target;
		SlashMoveMode	= (ESlashMoveMode)SkillProps->MoveType;
	}
}
