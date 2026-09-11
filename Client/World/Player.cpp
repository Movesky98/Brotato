#include "Player.h"

#include "World/CharacterMovementComponent.h"
#include "World/MeshComponent.h"
#include "World/CameraComponent.h"
#include "World/CameraManager.h"
#include "World/Input.h"
#include "World/ColliderBox2D.h"
#include "World/ColliderSphere2D.h"

#include "TimeManager.h"

#include "MainWorld.h"
#include "Weapon.h"
#include "Bullet.h"
#include "RangedWeapon.h"
#include "MeleeWeapon.h"

#include "DataAsset.h"
#include "WeaponData.h"
#include "CharacterData.h"

#include "Component/InventoryComponent.h"
#include "Component/ArenaCameraComponent.h"

#include "CollectableInterface.h"

#include "LogManager.h"

CPlayer::CPlayer()
{
}

CPlayer::CPlayer(const CPlayer& src)
{
}

CPlayer::CPlayer(CPlayer&& src) noexcept
{
}

CPlayer::~CPlayer()
{
}

void CPlayer::SetHP(float HP)
{
	mHP = HP;

	if(mUpdateHPFunc)
		mUpdateHPFunc(mHP, mHPMax);
}

bool CPlayer::Init()
{
	CActor::Init();

	SetDeathFunction(this, &CPlayer::OnDeath);

	mActorTag = "Player";

	mMeshComp = CreateComponent<CMeshComponent>("Root");
	mLegMeshComp = CreateComponent<CMeshComponent>("Leg");
	mCamera = CreateComponent<CArenaCameraComponent>("Camera");
	mBox = CreateComponent<CColliderBox2D>("Box");
	mPickupRange = CreateComponent<CColliderSphere2D>("PickupRange");

	mMoveComp = CreateComponent<CCharacterMovementComponent>("MoveComp");
	mInventoryComponent = CreateComponent<CInventoryComponent>("InventoryComp");

	auto Mesh = mMeshComp.lock();

	if (Mesh)
	{
		Mesh->SetMesh("TexRect");
		Mesh->SetShader("Material");
		Mesh->AddTexture(0, "Player", TEXT("Custom/engineer.png"));
		Mesh->AddTexture(0, "DissolveTexture", TEXT("dissolve_smoke.png"), "Texture", 1);
		Mesh->SetBlendState(0, "AlphaBlend");
		Mesh->SetRelativeScale(53.f, 53.f);

		Mesh->SetChromaKeyEnable(0, true);
	}

	auto LegMeshComp = mLegMeshComp.lock();
	if (LegMeshComp)
	{
		LegMeshComp->SetMesh("TexRect");
		LegMeshComp->SetShader("Material");
		LegMeshComp->AddTexture(0, "PlayerLeg", TEXT("Custom/legs.png"));
		LegMeshComp->AddTexture(0, "DissolveTexture", TEXT("dissolve_smoke.png"), "Texture", 1);
		LegMeshComp->SetBlendState(0, "AlphaBlend");
		LegMeshComp->SetRelativePos(0.f, -23.2f);
		LegMeshComp->SetRelativeScale(1.f, 0.5f);
	}

	auto Camera = mCamera.lock();

	if (Camera)
	{
		Camera->SetProjection(ECameraProjectionType::Ortho, 0.f, 1280.f, 720.f, 1000.f);
		Camera->SetInheritRot(false);
	}

	auto Box = mBox.lock();

	if (Box)
	{
		Box->SetInheritScale(false);
		Box->SetBoxSize(40.f, 40.f);
		Box->SetDebugDraw(true);
		Box->SetCollisionProfile("Player");
	}

	auto PickupRange = mPickupRange.lock();

	if (PickupRange)
	{
		PickupRange->SetInheritScale(false);
		PickupRange->SetRadius(120.f);
		PickupRange->SetDebugDraw(true);
		PickupRange->SetCollisionProfile("PickupRange");
		PickupRange->SetBeginOverlapFunc(this, &CPlayer::OnPickupRangeBeginOverlap);
	}

	auto Input = mWorld.lock()->GetInput().lock();

	if (Input)
	{
		Input->AddBindKey("MoveForward", 'W');
		Input->SetBindFunction("MoveForward", EInputType::Hold, this, &CPlayer::MoveForward);
		Input->AddBindKey("MoveBackward", 'S');
		Input->SetBindFunction("MoveBackward", EInputType::Hold, this, &CPlayer::MoveBackward);
		Input->AddBindKey("MoveLeft", 'A');
		Input->SetBindFunction("MoveLeft", EInputType::Hold, this, &CPlayer::MoveLeft);
		Input->AddBindKey("MoveRight", 'D');
		Input->SetBindFunction("MoveRight", EInputType::Hold, this, &CPlayer::MoveRight);
	}


	auto MoveComp = mMoveComp.lock();

	if (MoveComp)
	{
		MoveComp->SetUpdateComp(mRoot);
		MoveComp->SetSpeed(300.f);
		MoveComp->SetCurrentSpeed(300.f);
	}

	if (mInventoryComponent.expired()) return false;

	return true;
}

void CPlayer::Update(float DeltaTime)
{
	CActor::Update(DeltaTime);

	auto MeshComp = mMeshComp.lock();

	if (mIsHit)
	{
		mHitTime -= DeltaTime;

		if (mHitTime <= 0.f)
		{
			mIsHit = false;
			mHitTime = 0.f;
			if (MeshComp)
			{
				MeshComp->SetHitEffectEnable(0, false);
			}
		}
		else
		{
			if (MeshComp)
			{
				float Intensity = mHitTime / mHitDuration;
				MeshComp->SetHitIntensity(0, Intensity);
			}
		}
	}
}

void CPlayer::Destroy()
{
	CActor::Destroy();
}

float CPlayer::TakeDamage(float fDamage, const std::string& Instigator, const std::weak_ptr<CActor>& CauseActor)
{
	float Damage = CActor::TakeDamage(fDamage, Instigator, CauseActor);

	if (Damage > 0)
	{
		AddHP(-Damage);
	}

	mIsHit = true;
	mHitTime = mHitDuration;

	auto Mesh = mMeshComp.lock();

	if (Mesh)
	{
		Mesh->SetHitEffectEnable(0, true);
	}

	return Damage;
}

void CPlayer::OnDeath()
{
	auto BoxCollision = mBox.lock();
	if (BoxCollision)
	{
		BoxCollision->SetEnable(false);
	}

	// mIsDead = true;
}

void CPlayer::OnPickupRangeBeginOverlap(const FVector3& HitPoint, const FVector3& Normal, std::weak_ptr<class CCollider> Collider)
{
	auto ColliderComp = Collider.lock();
	if (!ColliderComp) return;

	auto ColliderOwner = ColliderComp->GetOwner().lock();
	if (!ColliderOwner) return;

	auto CollectableActor = std::dynamic_pointer_cast<ICollectableInterface>(ColliderOwner);
	auto InvComp = mInventoryComponent.lock();
	if (CollectableActor && InvComp)
	{
		std::function<void(int)> BindFunc = std::bind(&CInventoryComponent::AddMaterial, InvComp, std::placeholders::_1);

		CollectableActor->Collect(GetThisPtr<CActor>(), BindFunc);
	}
}

void CPlayer::MoveForward()
{
	auto MoveComp = mMoveComp.lock();

	if (MoveComp)
	{
		MoveComp->SetMoveDir(GetAxis(EAxis::Y));
	}
}

void CPlayer::MoveBackward()
{
	auto MoveComp = mMoveComp.lock();

	if (MoveComp)
	{
		MoveComp->SetMoveDir(-GetAxis(EAxis::Y));
	}
}

void CPlayer::MoveRight()
{
	auto MoveComp = mMoveComp.lock();

	if (MoveComp)
	{
		MoveComp->SetMoveDir(GetAxis(EAxis::X));
	}
}

void CPlayer::MoveLeft()
{
	auto MoveComp = mMoveComp.lock();

	if (MoveComp)
	{
		MoveComp->SetMoveDir(-GetAxis(EAxis::X));
	}
}

void CPlayer::Hit()
{
	mIsHit = true;
	mHitTime = mHitDuration;

	auto Mesh = mMeshComp.lock();

	if (Mesh)
	{
		Mesh->SetHitEffectEnable(0, true);
	}
}

void CPlayer::SpawnWeapons()
{
	auto InvComp = mInventoryComponent.lock();
	auto World = mWorld.lock();
	if (InvComp && World)
	{
		auto WeaponDataList = InvComp->GetWeaponData();

		for (auto WeakWeaponData : WeaponDataList)
		{
			auto WeaponData = std::dynamic_pointer_cast<CWeaponData>(WeakWeaponData.lock());
			if (!WeaponData) continue;

			// 최대 6개의 무기가 들어오고 그걸 가지고 동적 생성해야 하나?
			std::shared_ptr<CWeapon> NewWeapon;

			if (WeaponData->Type == EWeaponType::Ranged)
			{
				NewWeapon = World->CreateActor<CRangedWeapon>("RangedWeapon").lock();
			}
			else
			{
				NewWeapon = World->CreateActor<CMeleeWeapon>("MeleeWeapon").lock();
			}

			if (NewWeapon)
			{
				NewWeapon->AttachActorToActor(GetThisPtr<CActor>());
				NewWeapon->SetOwner(GetThisPtr<CActor>());
				NewWeapon->InitializeSettings(WeaponData, InvComp->GetStat());
			
				Weapons.push_back(NewWeapon);
			}
		}
	}

	// 생성이 끝나면 위치, 스케일 설정
	SetWeaponSettings();
}

void CPlayer::DestroyWeapons()
{
	if (!Weapons.size()) return;

	auto iter = Weapons.begin();
	auto iterEnd = Weapons.end();

	for (; iter != iterEnd;)
	{
		auto Weapon = iter->lock();
		if (Weapon)
		{
			Weapon->Destroy();
		}

		iter = Weapons.erase(iter);
		iterEnd = Weapons.end();
	}
}

void CPlayer::SetWeaponSettings()
{
	if (!Weapons.size()) return;

	float Angle = (float)(360 / Weapons.size());

	auto iter = Weapons.begin();
	auto iterEnd = Weapons.end();
	int Index = 0;
	for (; iter != iterEnd; Index++, iter++)
	{
		auto Weapon = iter->lock();
		if (Weapon)
		{
			const float Radian = DirectX::XMConvertToRadians(Angle * Index);
			FVector3 SpawnDirection = FVector3(cosf(Radian), sinf(Radian), GetWorldPos().z);
			SpawnDirection.Normalize();

			Weapon->SetWorldPos(GetWorldPos() + SpawnDirection * 50.f);
			Weapon->SetRelativeRotationZ(Angle * Index);
		}
	}
}

void CPlayer::SetPlayerDesc(std::weak_ptr<CCharacterData> InData)
{
	auto Data = InData.lock();
	if (!Data) return;

	auto MeshComp = mMeshComp.lock();
	if (MeshComp)
	{
		MeshComp->AddTexture(0, Data->Name, Data->TextureInfo.Path.c_str());
	}

	auto InvComp = GetInventoryComponent().lock();
	if (InvComp)
	{
		InvComp->AddStat(Data->Stats);
		InvComp->AddWeapon(Data->StartWeapon);
	}
}

void CPlayer::ActiveForWave()
{
	SetRenderEnable(true);
	SetEnable(true);
	SetWorldPos(0.f, 0.f);
	
	auto InvComp = mInventoryComponent.lock();
	if (InvComp)
	{
		SetHPMax(InvComp->GetStatValue(EPrimaryStats::MaxHP));
		SetHP(GetMaxHP());
	}

	SpawnWeapons();
}

void CPlayer::DeactiveForWave()
{
	DestroyWeapons();

	SetEnable(false);
	SetRenderEnable(false);
}
