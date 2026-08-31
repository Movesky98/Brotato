#include "Enemy.h"
#include "EnemyData.h"

#include "Player.h"
#include "MainWorld.h"
#include "Component/DashSkillComponent.h"
#include "Component/SpawnProjectileComponent.h"

#include "World/MeshComponent.h"
#include "World/CharacterMovementComponent.h"
#include "World/ColliderBox2D.h"
#include "World/ColliderSphere2D.h"

#include "LogManager.h"

#include <random>
#include <time.h>

CEnemy::CEnemy()
{
}

CEnemy::CEnemy(const CEnemy& src)
	: CActor(src)
{
}

CEnemy::CEnemy(CEnemy&& src) noexcept
	: CActor(std::move(src))
{
}

CEnemy::~CEnemy()
{
}

bool CEnemy::Init()
{
	if (!CActor::Init()) return false;

	mActorTag = "Enemy";

	mMeshComp = CreateComponent<CMeshComponent>("Root");
	mBoxCollision = CreateComponent<CColliderBox2D>("BoxCollision");
	mPerceptionRange = CreateComponent<CColliderSphere2D>("PerceptionRange");
	mMoveComp = CreateComponent<CCharacterMovementComponent>("CharacterMoveComp");

	if (mMoveComp.expired()) return false;
	if (mBoxCollision.expired()) return false;
	if (mPerceptionRange.expired()) return false;
	if (mMoveComp.expired()) return false;

	if (auto World = std::dynamic_pointer_cast<CMainWorld>(mWorld.lock()))
	{
		FArenaDesc Desc = World->GetArenaDesc();
		
		mLimitPos.x = Desc.CountX * Desc.TileSize / 2 - Desc.TileSize * 1.5f;
		mLimitPos.y = Desc.CountY * Desc.TileSize / 2 - Desc.TileSize * 1.5f;
	}

	return true;
}

void CEnemy::Update(float DeltaTime)
{
	CActor::Update(DeltaTime);

	if (!mCanAttack)
	{
		mAttackElapsedTime += DeltaTime;
		if (mAttackElapsedTime >= mAttackDelay)
		{
			mAttackElapsedTime = 0.f;
			mCanAttack = true;
		}
	}
	else
		Attack();

	if (mIsDissolve)
	{
		auto MeshComp = mMeshComp.lock();
		auto BoxCollision = mBoxCollision.lock();
		mDissolveProgress += mDissolveSpeed * DeltaTime;

		if (mDissolveProgress >= 1.f)
		{
			if (MeshComp)
				MeshComp->SetDissolveEnable(0, false);

			mIsDissolve = false;
			mDissolveProgress = 1.f;

			// Destroy();
		}

		if (MeshComp)
		{
			MeshComp->SetDissolveProgress(0, mDissolveProgress);
		}

		return;
	}

	if (!mCanMove) return;
	if (PlayHitEffect(DeltaTime)) return;

	UpdateBehavior();
	PlayMoveAnimation();

	UseSkill(ESkillTrigger::Always);
}

void CEnemy::Destroy()
{
	auto World = GetWorld().lock();
	if (World)
	{
		auto MainWorld = std::dynamic_pointer_cast<CMainWorld>(World);
		if (MainWorld)
		{
			MainWorld->NotifyEnemyDead();
		}
	}

	CActor::Destroy();
}

float CEnemy::TakeDamage(float fDamage, const std::string& Instigator, const std::weak_ptr<CActor>& CauseActor)
{
	float Damage = CActor::TakeDamage(fDamage, Instigator, CauseActor);

	mIsHit = true;
	mHitTime = mHitDuration;

	auto Mesh = mMeshComp.lock();

	if (Mesh)
	{
		Mesh->SetHitEffectColor(0, FVector4(0.8f, 0.8f, 0.8f, 1.f));
		Mesh->SetHitEffectEnable(0, true);
	}

	UseSkill(ESkillTrigger::OnHit);
	AddHP(-Damage);

	return 0.0f;
}

bool CEnemy::InitializeEnemyInfo(const std::shared_ptr<const CEnemyData>& Data, const std::weak_ptr<CActor> Target)
{
	if (!Data)
		return false;

	//auto Info = EnemyInfo.lock();
	//auto BaseData = Info->BaseData;
	//auto TextureData = Info->TextureData;
	//auto SkillData = Info->SkillData;
	const FTextureDesc& Texture = Data->TextureInfo;
	mSpeed = Data->Speed;
	mHP = Data->HP;
	mDamage = Data->Damage;

	auto MeshComp = mMeshComp.lock();
	if (MeshComp)
	{
		const std::string& Name = Data->Name;

		MeshComp->SetMesh("TexRect");
		MeshComp->SetShader("Material");
		MeshComp->AddTexture(0, Name, Texture.Path.c_str());
		MeshComp->AddTexture(0, "DissolveTexture", TEXT("dissolve_smoke.png"), "Texture", 1);
		MeshComp->SetBlendState(0, "AlphaBlend");
		MeshComp->SetRelativeScale(Texture.SizeX, Texture.SizeY);
		MeshComp->SetBaseColor(0, FVector4(1.f, 1.f, 1.f, 1.f));
	}

	auto BoxCollision = mBoxCollision.lock();
	if (BoxCollision)
	{
		BoxCollision->SetInheritScale(true);
		BoxCollision->SetBoxSize(Texture.SizeX, 40.f);
		BoxCollision->SetDebugDraw(true);
		BoxCollision->SetCollisionProfile("Enemy");
		BoxCollision->SetBeginOverlapFunc(this, &CEnemy::OnCollisionBeginOverlap);
		BoxCollision->SetEndOverlapFunc(this, &CEnemy::OnCollisionEndOverlap);
	}

	auto MoveComp = mMoveComp.lock();
	if (MoveComp)
	{
		MoveComp->SetUpdateComp(mRoot);
		MoveComp->SetSpeed(mSpeed);
		MoveComp->SetCurrentSpeed(mSpeed);
	}

	SetHP(mHP);
	SetDeathFunction(this, &CEnemy::OnDeath);

	mCurrentScaleWidth = Texture.SizeX;
	mCurrentScaleHeight = Texture.SizeY;
	mMaxScaleHeight = mCurrentScaleHeight * 1.2f;
	mMinScaleHeight = mCurrentScaleHeight * 0.9f;

	switch (Data->Movement.size())
	{
	case 1:
		MovementsByRange.push_back(Data->Movement[0]);
		MovementsByRange.push_back(Data->Movement[0]);
		break;
	case 2:
		MovementsByRange.push_back(Data->Movement[0]);
		MovementsByRange.push_back(Data->Movement[1]);
		break;
	default:
		// ERROR
		break;
	}

	SetSkills(Data->Skills);

	if (MovementsByRange[(int)CurrentRangeState] != EEnemyMovement::Random)
		mTarget = Target;

	return true;
}

void CEnemy::SetSkills(const std::unordered_multimap<ESkillTrigger, std::shared_ptr<CSkillData>>& SkillData)
{
	if (SkillData.empty()) return;

	if (SkillData.contains(ESkillTrigger::InPerceptionRange) || SkillData.contains(ESkillTrigger::Always))
	{
		auto PerceptionRange = mPerceptionRange.lock();
		if (PerceptionRange)
		{
			PerceptionRange->SetInheritScale(false);
			PerceptionRange->SetRadius(200.f);
			PerceptionRange->SetDebugDraw(true);
			PerceptionRange->SetCollisionProfile("DetectPlayer");
			PerceptionRange->SetBeginOverlapFunc(this, &CEnemy::OnPerceptionRangeBeginOverlap);
			PerceptionRange->SetEndOverlapFunc(this, &CEnemy::OnPerceptionRangeEndOverlap);
		}
	}

	// SkillComponent 생성
	auto iter = SkillData.begin();
	auto iterEnd = SkillData.end();

	for (;iter != iterEnd; ++iter)
	{
		ESkillTrigger Trigger = iter->first;

		auto [begin, End] = SkillData.equal_range(Trigger);
		
		for (auto It = begin; It != End; It++)
		{
			auto SkillProps = It->second;
			if (!SkillProps) continue;

			// SkillComponent 만들고
			auto SkillComp = CreateSkillComponent(SkillProps->Component).lock();
			if (SkillComp)
			{
				// SkillComponent 초기 설정 세팅해주고 저장
				SkillComp->InitializeProperties(SkillProps);
				TriggerSkillMaps.insert(std::make_pair(Trigger, SkillComp));
			}
		}
	}
}

std::weak_ptr<class CSkillComponent> CEnemy::CreateSkillComponent(const ESkillComponent Component)
{
	std::weak_ptr<CSkillComponent> TempComp;

	switch (Component)
	{
	case ESkillComponent::Dash:
		TempComp = CreateComponent<CDashSkillComponent>("DashSkillComponent");
		break;
	case ESkillComponent::SpawnProjectile:
		TempComp = CreateComponent<CSpawnProjectileComponent>("SpwnProjectileComponent");
		break;
	default:
		break;
	}

	return TempComp;
}

void CEnemy::UseSkill(const ESkillTrigger Trigger)
{
	if (TriggerSkillMaps.contains(Trigger))
	{
		auto [begin, End] = TriggerSkillMaps.equal_range(Trigger);
		for (auto It = begin; It != End; It++)
		{
			auto SkillComp = It->second.lock();
			if (SkillComp)
			{
				SkillComp->SkillProgress();
			}
		}
	}

}

void CEnemy::SetHP(float HP)
{
	mHPMax = HP;
	mHP = HP;
}

void CEnemy::OnCollisionBeginOverlap(const FVector3& HitPoint, const FVector3& Normal, std::weak_ptr<class CCollider> Collider)
{
	auto _Collider = Collider.lock();
	if (_Collider && _Collider->GetCollisionProfile()->Name == "Player")
	{
		auto Owner = _Collider->GetOwner().lock();
		auto Target = mTarget.lock();
		if (Owner && Target)
		{
			if (Owner.get() == Target.get())
				mIsTargetInRange = true;
		}
	}
}

void CEnemy::OnCollisionEndOverlap(std::weak_ptr<class CCollider> Collider)
{
	auto _Collider = Collider.lock();
	if (_Collider && _Collider->GetCollisionProfile()->Name == "Player")
	{
		auto Owner = _Collider->GetOwner().lock();
		auto Target = mTarget.lock();
		if (Owner && Target)
		{
			if (Owner.get() == Target.get())
				mIsTargetInRange = false;
		}
	}
}

void CEnemy::OnPerceptionRangeBeginOverlap(const FVector3& HitPoint, const FVector3& Normal, std::weak_ptr<class CCollider> Collider)
{
	auto _Collider = Collider.lock();
	if (_Collider && _Collider->GetCollisionProfile()->Name == "Player")
	{
		auto Owner = _Collider->GetOwner().lock();
		
		if (mTarget.expired())
			mTarget = Owner;
		
		auto Target = mTarget.lock();

		if (Owner && Target)
		{
			if (Owner.get() == Target.get())
			{
				CurrentRangeState = ERangeState::InRange;

				// 스킬 사용
				UseSkill(ESkillTrigger::InPerceptionRange);
			}
		}
	}
}

void CEnemy::OnPerceptionRangeEndOverlap(std::weak_ptr<class CCollider> Collider)
{
	auto _Collider = Collider.lock();
	if (_Collider && _Collider->GetCollisionProfile()->Name == "Player")
	{
		auto Owner = _Collider->GetOwner().lock();
		auto Target = mTarget.lock();
		if (Owner && Target)
		{
			if (Owner.get() == Target.get())
				CurrentRangeState = ERangeState::OutOfRange;
		}
	}
}

void CEnemy::OnDeath()
{
	UseSkill(ESkillTrigger::OnDeath);

	Destroy();
}

void CEnemy::PlayMoveAnimation()
{
	if (!mCanScaleSize) return;

	auto MoveComp = mMoveComp.lock();
	auto MeshComp = mMeshComp.lock();
	auto BoxCollision = mBoxCollision.lock();

	if (MeshComp && BoxCollision && MoveComp)
	{
		if (mCurrentScaleHeight >= mMaxScaleHeight)
		{
			mToggleScale = false;
		}
		else if (mCurrentScaleHeight <= mMinScaleHeight)
		{
			mToggleScale = true;
		}

		mCurrentScaleHeight += (mToggleScale ? 0.005f : -0.005f) * MoveComp->GetMoveSpeed();

		MeshComp->SetWorldScale(mCurrentScaleWidth, mCurrentScaleHeight);
		BoxCollision->SetBoxSize(mCurrentScaleWidth, mCurrentScaleHeight);
	}
}

bool CEnemy::PlayHitEffect(float DeltaTime)
{
	if (mIsHit)
	{
		mHitTime -= DeltaTime;
		auto MeshComp = mMeshComp.lock();
		auto MoveComp = mMoveComp.lock();
		if (mHitTime <= 0.f)
		{
			mIsHit = false;
			mHitTime = 0.f;
			if (MeshComp) MeshComp->SetHitEffectEnable(0, false);
			if (MoveComp) MoveComp->SetSpeed(MoveComp->GetCurrentSpeed());
		}
		else
		{
			// Hit Effect
			if (MeshComp)
			{
				float Intensity = mHitTime / mHitDuration;
				MeshComp->SetHitIntensity(0, Intensity);
			}

			// Knockback
			// 속도를 조금씩 깎아야 함. 언제까지 -> 0까지
			// HitTime에 맞추는 게 좋을 듯.
			if (MoveComp)
			{
				float KnockBackIntensity = mHitTime / mHitDuration;
				float Speed = MoveComp->GetMoveSpeed() - KnockBackIntensity * MoveComp->GetCurrentSpeed();

				MoveComp->SetSpeed(Speed);
			}
		}

		return true;
	}

	return false;
}

void CEnemy::UpdateBehavior()
{
	if (!mCanMove) return;

	auto Target = mTarget.lock();
	auto MoveComp = mMoveComp.lock();
	FVector3 Direction;
	
	if (Target && MoveComp)
	{
		EEnemyMovement CurrentMovementType = MovementsByRange[(int)CurrentRangeState];

		Direction = Target->GetWorldPos() - GetWorldPos();

		if (MovementsByRange[(int)CurrentRangeState] == EEnemyMovement::FleeFromTarget)
			Direction = -Direction;
	}
	else if (mTarget.expired())
	{
		if (!mIsRandomMove)
		{
			// 랜덤 위치 지정
			float Angle = (float)(rand() % 360);
			const float Radian = DirectX::XMConvertToRadians(Angle);
			Direction = FVector3(cosf(Radian), sinf(Radian), GetWorldPos().z);
			Direction.Normalize();

			FVector2 MovePos;
			MovePos.x = GetWorldPos().x + Direction.x * 200.f;
			MovePos.y = GetWorldPos().y + Direction.y * 200.f;

			if (std::abs(MovePos.x) > mLimitPos.x)
				Direction.x *= -1;
			if (std::abs(MovePos.y) > mLimitPos.y)
				Direction.y *= -1;

			RandomPos = GetWorldPos() + Direction * 200.f;
			mIsRandomMove = true;
		}
		else
		{
			FVector3 Distance = RandomPos - GetWorldPos();
			Direction = Distance;

			if (Distance.Length() <= 45.f)
			{
				RandomPos = FVector3::Zero;
				mIsRandomMove = false;
			}
		}
	}

	MoveComp->SetMoveDir(Direction);
}

void CEnemy::Attack()
{
	if (mCanAttack && mIsTargetInRange)
	{

		auto Target = mTarget.lock();
		if (Target)
		{
			auto Player = std::dynamic_pointer_cast<CPlayer>(Target);
			if (!Player->IsDead())
			{
				Target->TakeDamage(mDamage, GetActorTag(), GetThisPtr<CActor>());
				mCanAttack = false;
			}
			else 
				mTarget.reset();
		}
	}
}

void CEnemy::NotifyKnockBack(const FVector3& HitPoint, const FVector3& Normal, float Force)
{
	if (!mIsKnockBack) return;

	FVector3 KnockBackDirection = Normal;

	auto MoveComp = mMoveComp.lock();
	if (MoveComp)
	{
		MoveComp->SetMoveDir(KnockBackDirection);
		MoveComp->SetSpeed(Force);
	}
}
