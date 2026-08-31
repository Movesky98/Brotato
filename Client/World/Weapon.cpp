#include "Weapon.h"
#include "WeaponData.h"

#include "Enemy.h"
#include "Bullet.h"

#include "Interface/StatusInterface.h"

#include "World/World.h"
#include "World/MeshComponent.h"
#include "World/ColliderSphere2D.h"

CWeapon::CWeapon()
{
}

CWeapon::CWeapon(const CWeapon& src)
	: CActor(src)
{
}

CWeapon::CWeapon(CWeapon&& src) noexcept
	: CActor(std::move(src))
{
}

CWeapon::~CWeapon()
{
}

bool CWeapon::Init()
{
	if (!CActor::Init()) return false;

	mMeshComp = CreateComponent<CMeshComponent>("Root");
	mSphereCollision = CreateComponent<CColliderSphere2D>("SphereCollision");

	auto SphereCollision = mSphereCollision.lock();
	if (SphereCollision)
	{
		SphereCollision->SetRadius(150.f);
		SphereCollision->SetCollisionProfile("DetectEnemy");
		SphereCollision->SetDebugDraw(false);
		SphereCollision->SetBeginOverlapFunc(this, &CWeapon::OnCollisionBeginOverlap);
		SphereCollision->SetEndOverlapFunc(this, &CWeapon::OnCollisionEndOverlap);
	}

	return true;
}

void CWeapon::Update(float DeltaTime)
{
	CActor::Update(DeltaTime);

	if (!mCanAttack && mAttackEnded)
	{
		mElapsedAttackTime += DeltaTime;

		if (mElapsedAttackTime > mCooldown)
		{
			mCanAttack = true;
			mElapsedAttackTime = 0.f;
		}
	}

	if (mTargetList.size())
	{
		// TODO :
		// 가장 가까운 타겟 계산하고		- std::weak_ptr<CEnemy> GetClosestTarget()
		// 회전 돌려서 그 타겟 노려보고	- void LookAtTarget(Target)
		// 때 되면 총 쏘거나 때리고		- void Attack(Target)

		std::weak_ptr<CEnemy> Target = GetClosestTarget();
		LookAtTarget(Target);

		if (mCanAttack) 
		{
			mAttackEnded = false;
			Attack(Target);
			mCanAttack = false;
		}
	}
}

void CWeapon::Destroy()
{
	CActor::Destroy();
}

std::weak_ptr<CEnemy> CWeapon::GetClosestTarget()
{
	if(!mTargetList.size())
		return std::weak_ptr<CEnemy>();

	auto iter = mTargetList.begin();
	auto iterEnd = mTargetList.end();

	float MinDistance = FLT_MAX;
	std::weak_ptr<CEnemy> ClosestTarget;

	for (;iter != iterEnd;)
	{
		if (iter->expired())
		{
			iter = mTargetList.erase(iter);
			iterEnd = mTargetList.end();
			continue;
		}

		auto Target = iter->lock();
		if (Target)
		{
			// 죽은 경우 제거
			auto StatusInterface = std::dynamic_pointer_cast<CStatusInterface>(Target);
			if (StatusInterface && StatusInterface->IsDead())
			{
				iter = mTargetList.erase(iter);
				iterEnd = mTargetList.end();
				continue;
			}

			FVector3 Distance = GetWorldPos() - Target->GetWorldPos();
			if (Distance.Length() < MinDistance)
			{
				MinDistance = Distance.Length();
				ClosestTarget = Target;
			}
		}

		++iter;
	}

	return ClosestTarget;
}

void CWeapon::LookAtTarget(std::weak_ptr<CEnemy> _Target)
{
	FVector3 ZeroAngleVector = FVector3(1.f, 0.f, 0.f);

	auto Target = _Target.lock();
	if (Target)
	{
		FVector3 Direction = Target->GetWorldPos() - GetWorldPos();
		Direction.Normalize();

		FVector3 CrossProduct = ZeroAngleVector.CrossProduct(Direction);
		float Radian = std::acos(ZeroAngleVector.DotProduct(Direction));
		float Angle = Radian * 180.f / 3.14159265f;
		// 0 ~ 180까지는 나오는데 180~360까지 어케 함
		// 외적을 이용해서 할 수 있음.
		// → 0 ~ 180 == 외적 결과.Z > 0
		// → 180 ~ 360 == 외적 결과.Z < 0

		if (CrossProduct.z <= 0)
			Angle = 360.f - Angle;

		SetRelativeRotationZ(Angle);
	}
}

void CWeapon::Attack(std::weak_ptr<CEnemy> _Target)
{

}

void CWeapon::SetTexture(const std::string& Name, const std::wstring& TexturePath, float SizeX, float SizeY)
{
	auto MeshComp = mMeshComp.lock();
	if (MeshComp)
	{
		MeshComp->SetMesh("TexRect");
		MeshComp->SetShader("Material");
		MeshComp->AddTexture(0, Name, TexturePath.c_str());
		MeshComp->AddTexture(0, "DissolveTexture", TEXT("dissolve_smoke.png"), "Texture", 1);
		MeshComp->SetBlendState(0, "AlphaBlend");
		MeshComp->SetRelativeScale(SizeX, SizeY);
	}
}

void CWeapon::InitializeSettings(const std::weak_ptr<CWeaponData>& InData)
{
	auto Data = InData.lock();
	if (Data)
	{
		mBaseDamage = Data->BaseDamage;
		mCooldown = Data->Cooldown;
		mKnockback = Data->Knockback;
		mCritical = Data->Critical;
		mRange = Data->Range;
		mLifeSteal = Data->LifeSteal;
		mPiercing = Data->Piercing;

		SetTexture(Data->Name, Data->TextureInfo.Path, Data->TextureInfo.SizeX, Data->TextureInfo.SizeY);
	}

	auto SphereCollision = mSphereCollision.lock();
	if (SphereCollision)
	{
		SphereCollision->SetRadius(mRange);
	}
}

void CWeapon::OnCollisionBeginOverlap(const FVector3& HitPoint, const FVector3& Normal, std::weak_ptr<CCollider> Collider)
{
	auto _Collider = Collider.lock();
	if (_Collider)
	{
		auto Owner = _Collider->GetOwner().lock();
		if (Owner && Owner->GetActorTag() == "Enemy")
		{
			mTargetList.push_back(std::dynamic_pointer_cast<CEnemy>(Owner));
		}
	}
}

void CWeapon::OnCollisionEndOverlap(std::weak_ptr<CCollider> Collider)
{
	auto _Collider = Collider.lock();
	if (_Collider)
	{
		auto Owner = _Collider->GetOwner().lock();
		if (Owner && Owner->GetActorTag() == "Enemy")
		{
			auto iter = mTargetList.begin();
			auto iterEnd = mTargetList.end();

			for (; iter != iterEnd;)
			{
				if (iter->expired())
				{
					iter = mTargetList.erase(iter);
					iterEnd = mTargetList.end();
					continue;
				}

				// 아 뭐였지 비교하는 방법
				auto Enemy = iter->lock();
				if (Enemy.get() == Owner.get())
				{
					mTargetList.erase(iter);
					break;
				}

				++iter;
			}
		}
	}

	if(mTargetList.empty())
		SetRelativeRotationZ(0);
}