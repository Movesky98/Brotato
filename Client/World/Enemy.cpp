#include "Enemy.h"

#include "DataAsset.h"
#include "EnemyData.h"

#include "Player.h"
#include "MainWorld.h"
#include "Component/DashSkillComponent.h"
#include "Component/SpawnProjectileComponent.h"
#include "Component/SpawnSlashComponent.h"
#include "Component/SpawnEnemyComponent.h"

#include "World/MeshComponent.h"
#include "World/ColliderBox2D.h"
#include "World/ColliderSphere2D.h"
#include "World/Animation2DComponent.h"
#include "World/CharacterMovementComponent.h"

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
	mHitMeshComp = CreateComponent<CMeshComponent>("HitMeshComp");
	mAnimationComp = CreateComponent<CAnimation2DComponent>("AnimationComp");

	if (mMoveComp.expired()) return false;
	if (mBoxCollision.expired()) return false;
	if (mPerceptionRange.expired()) return false;
	if (mMoveComp.expired()) return false;
	if (mHitMeshComp.expired()) return false;
	if (mAnimationComp.expired()) return false;

	auto AnimationComp = mAnimationComp.lock();
	if (AnimationComp)
	{
		AnimationComp->SetUpdateComponent(mHitMeshComp);
		AnimationComp->AddAnimation("HitFrame", 0.1f, 1.f, false);
	}

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

	if (mIsDead)
	{
		PlayDeathAnimation(DeltaTime);
		return;
	}

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

	UseSkill(ESkillTrigger::Always);

	if (CurrentRangeState == ERangeState::InRange)
		UseSkill(ESkillTrigger::InPerceptionRange);

	if (!mCanMove) return;
	if (PlayHitEffect(DeltaTime)) return;

	UpdateBehavior();
	PlayMoveAnimation(DeltaTime);
}

void CEnemy::Destroy()
{

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

	auto AnimationComp = mAnimationComp.lock();
	if (AnimationComp)
	{
		AnimationComp->PlayAnimation("HitFrame");
	}

	UseSkill(ESkillTrigger::OnHit);

	AddHP(-Damage);

	return 0.0f;
}

void CEnemy::InitializeMeshComponent(const std::string Name, const TCHAR* Path, float SizeX, float SizeY)
{
	auto MeshComp = mMeshComp.lock();
	if (MeshComp)
	{
		MeshComp->SetMesh("TexRect");
		MeshComp->SetShader("Material");
		MeshComp->AddTexture(0, Name, Path);
		MeshComp->SetBlendState(0, "AlphaBlend");
		MeshComp->SetRelativeScale(SizeX, SizeY);
		MeshComp->SetBaseColor(0, FVector4::White);
	}

	auto HitMeshComp = mHitMeshComp.lock();
	if (HitMeshComp)
	{
		HitMeshComp->SetMesh("TexRect");
		HitMeshComp->SetShader("Material");
		HitMeshComp->AddTexture(0, "BaseHitImage", TEXT("Custom/Hit/Hit_0.png"));
		HitMeshComp->SetBlendState(0, "AlphaBlend");
		// HitMeshComp->SetRelativeScale(1, 1);
		HitMeshComp->SetBaseColor(0, FVector4::White);
	}
}

void CEnemy::InitializeBoxCollision(float SizeX, float SizeY)
{
	auto BoxCollision = mBoxCollision.lock();
	if (BoxCollision)
	{
		BoxCollision->SetInheritScale(true);
		BoxCollision->SetBoxSize(SizeX, SizeY);
		BoxCollision->SetDebugDraw(false);
		BoxCollision->SetCollisionProfile("Enemy");
		BoxCollision->SetBeginOverlapFunc(this, &CEnemy::OnCollisionBeginOverlap);
		BoxCollision->SetEndOverlapFunc(this, &CEnemy::OnCollisionEndOverlap);
	}
}

void CEnemy::InitializePerceptionRange(float Range, bool IsDebugDraw)
{
	auto PerceptionRange = mPerceptionRange.lock();
	if (PerceptionRange)
	{
		PerceptionRange->SetInheritScale(false);
		PerceptionRange->SetRadius(Range);
		PerceptionRange->SetDebugDraw(IsDebugDraw);
		PerceptionRange->SetCollisionProfile("DetectPlayer");
		PerceptionRange->SetBeginOverlapFunc(this, &CEnemy::OnPerceptionRangeBeginOverlap);
		PerceptionRange->SetEndOverlapFunc(this, &CEnemy::OnPerceptionRangeEndOverlap);
	}
}

void CEnemy::InitializeSpeed(float Speed)
{
	auto MoveComp = mMoveComp.lock();
	if (MoveComp)
	{
		MoveComp->SetUpdateComp(mRoot);
		MoveComp->SetSpeed(Speed);
		MoveComp->SetCurrentSpeed(Speed);
	}
}

bool CEnemy::PostInitializeProperties(const std::shared_ptr<class CDataAsset>& Data, const std::weak_ptr<CActor> Target)
{
	auto EnemyData = std::dynamic_pointer_cast<CEnemyData>(Data);
	if (!EnemyData) return false;

	mHP = EnemyData->HP;
	mSpeed = EnemyData->Speed;
	mDamage = EnemyData->Damage;

	SetHP(mHP);
	SetDeathFunction(this, &CEnemy::OnDeath);

	InitializeSpeed(mSpeed);

	switch (EnemyData->Movement.size())
	{
	case 1:
		MovementsByRange.push_back(EnemyData->Movement[0]);
		MovementsByRange.push_back(EnemyData->Movement[0]);
		break;
	case 2:
		MovementsByRange.push_back(EnemyData->Movement[0]);
		MovementsByRange.push_back(EnemyData->Movement[1]);
		break;
	default:
		// ERROR
		break;
	}

	SetSkills(EnemyData->Skills);

	mTarget = Target;

	return true;
}

bool CEnemy::InitializeProperties(const std::shared_ptr<class CDataAsset>& Data, const std::weak_ptr<CActor> Target, bool IsCountedSpawn)
{
	const FTextureDesc& Texture = Data->TextureInfo;

	const TCHAR* Path = Texture.Path.c_str();

	InitializeMeshComponent(Data->Name, Path, Texture.SizeX, Texture.SizeY);
	InitializeBoxCollision(Texture.SizeX, Texture.SizeY);

	mCurrentScale.x = Texture.SizeX;
	mCurrentScale.y = Texture.SizeY;

	mMaxScale = mCurrentScale;
	mMinScale.x = mCurrentScale.x;
	mMinScale.y = mCurrentScale.y * 0.85f;

	mIsCountedSpawn = IsCountedSpawn;

	if (!PostInitializeProperties(Data, Target)) return false;

	return true;
}

void CEnemy::SetSkills(const std::unordered_multimap<ESkillTrigger, std::shared_ptr<CSkillData>>& SkillData)
{
	if (SkillData.empty()) return;

	if (SkillData.contains(ESkillTrigger::InPerceptionRange) || SkillData.contains(ESkillTrigger::Always))
	{
		InitializePerceptionRange(mRange, false);
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
	case ESkillComponent::SpawnSlash:
		TempComp = CreateComponent<CSpawnSlashComponent>("SpawnSlashComponent");
		break;
	case ESkillComponent::SpawnEnemy:
		TempComp = CreateComponent<CSpawnEnemyComponent>("SpawnEnemyComponent");
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

	mIsDead = true;
	auto BoxCollision = mBoxCollision.lock();
	if (BoxCollision)
	{
		BoxCollision->SetEnable(false);
	}
}

void CEnemy::PlayMoveAnimation(float DeltaTime)
{
	if (!mCanScaleSize) return;

	auto MeshComp = mMeshComp.lock();
	auto BoxCollision = mBoxCollision.lock();

	if (MeshComp && BoxCollision)
	{
		int ScaleSign = mIsGrowing ? 1 : -1;
		mScaleProgress += ScaleSign * DeltaTime / mScalePeriod;

		if (mScaleProgress > 1.f)
		{
			mScaleProgress = 1.f;
			mIsGrowing = false;
		}

		if (mScaleProgress < 0.f)
		{
			mScaleProgress = 0.f;
			mIsGrowing = true;
		}

		mCurrentScale.y = mMinScale.y + mScaleProgress * (mMaxScale.y - mMinScale.y);

		MeshComp->SetWorldScale(mCurrentScale);
		BoxCollision->SetBoxSize(mCurrentScale);
	}
}

void CEnemy::PlayDeathAnimation(float DeltaTime)
{
	mDeathRotation += DeltaTime * 720.f / mDeathDuration;
	mDeathScaleRatio -= DeltaTime / mDeathDuration;

	if (mDeathScaleRatio <= 0.f)
	{
		if (mNotifyDeathFunc)
			mNotifyDeathFunc(GetWorldPos(), mIsCountedSpawn);

		Destroy();
	}

	auto MeshComp = mMeshComp.lock();
	if (MeshComp)
	{
		MeshComp->SetRelativeRotationZ(mDeathRotation);
		MeshComp->SetWorldScale(mCurrentScale * mDeathScaleRatio);
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
	
	if (MoveComp)
	{
		EEnemyMovement CurrentMovementType = MovementsByRange[(int)CurrentRangeState];

		if (Target && CurrentMovementType != EEnemyMovement::Random)
		{
			Direction = Target->GetWorldPos() - GetWorldPos();

			if (MovementsByRange[(int)CurrentRangeState] == EEnemyMovement::FleeFromTarget)
				Direction = -Direction;
		}
		else
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
