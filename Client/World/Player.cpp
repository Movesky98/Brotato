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

#include "WeaponData.h"
#include "DataAsset.h"

#include "Component/InventoryComponent.h"
#include "Component/ArenaCameraComponent.h"

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

float CPlayer::GetSpeed()
{
	auto MoveComp = mMoveComp.lock();

	if (MoveComp)
	{
		return MoveComp->GetMoveSpeed();
	}

	return 0.0f;
}

float CPlayer::GetCurrentSpeed()
{
	auto MoveComp = mMoveComp.lock();

	if (MoveComp)
	{
		return MoveComp->GetCurrentSpeed();
	}

	return 0.0f;
}

void CPlayer::SetSpeed(float Speed)
{
	auto MoveComp = mMoveComp.lock();

	if (MoveComp)
	{
		MoveComp->SetSpeed(Speed);
	}
}

void CPlayer::SetCurrentSpeed(float Speed)
{
	auto MoveComp = mMoveComp.lock();

	if (MoveComp)
	{
		MoveComp->SetCurrentSpeed(Speed);
	}
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

	SetHPMax(100.f);
	SetDeathFunction(this, &CPlayer::OnDeath);

	mActorTag = "Player";

	mMeshComp = CreateComponent<CMeshComponent>("Root");
	mLegMeshComp = CreateComponent<CMeshComponent>("Leg");
	mCamera = CreateComponent<CArenaCameraComponent>("Camera");
	mBox = CreateComponent<CColliderBox2D>("Box");
	mSphere = CreateComponent<CColliderSphere2D>("Sphere");

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
		Mesh->SetRelativeScale(40.f, 40.f);

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
		LegMeshComp->SetRelativePos(0.f, -17.5f);
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

	auto Sphere = mSphere.lock();

	if (Sphere)
	{
		Sphere->SetInheritScale(false);
		Sphere->SetRadius(120.f);
		Sphere->SetDebugDraw(true);
		Sphere->SetCollisionProfile("PlayerTrigger");
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

		//회전
		Input->AddBindKey("RotateLeft", 'E');
		Input->SetBindFunction("RotateLeft", EInputType::Hold, this, &CPlayer::RotateLeft);
		Input->AddBindKey("RotateRight", 'Q');
		Input->SetBindFunction("RotateRight", EInputType::Hold, this, &CPlayer::RotateRight);

		Input->AddBindKey("Fire", VK_LBUTTON);
		Input->SetBindFunction("Fire", EInputType::Hold, this, &CPlayer::Fire);

		Input->AddBindKey("Test", 'T');
		Input->SetBindFunction("Test", EInputType::Press, this, &CPlayer::TestInput);

		Input->AddBindKey("Hit", 'Y');
		Input->SetBindFunction("Hit", EInputType::Press, this, &CPlayer::Hit);
	}


	auto MoveComp = mMoveComp.lock();

	if (MoveComp)
	{
		MoveComp->SetUpdateComp(mRoot);
		MoveComp->SetSpeed(300.f);
		MoveComp->SetCurrentSpeed(200.f);
	}

	if (mInventoryComponent.expired()) return false;

	/*for (int i = 0; i < 6; i++)
	{
		TestInput();
	}*/

	return true;
}

void CPlayer::Update(float DeltaTime)
{
	CActor::Update(DeltaTime);

	mUVScrollTime += DeltaTime;

	auto MeshComp = mMeshComp.lock();

	if (MeshComp)
	{
		MeshComp->SetUVScrollTime(0, mUVScrollTime);
	}

	if (mIsDissolve)
	{
		mDissolveProgress += mDissolveSpeed * DeltaTime;

		if (mDissolveProgress >= 1.f)
		{
			mDissolveProgress = 1.f;

			if (MeshComp)
				MeshComp->SetDissolveEnable(0, false);

			mIsDissolve = false;
			mDissolveProgress = 1.f;

			Destroy();
		}

		if (MeshComp)
		{
			MeshComp->SetDissolveProgress(0, mDissolveProgress);
		}
	}

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
	if (mIsDissolve) return;

	mIsDissolve = true;
	mDissolveProgress = 0.f;

	auto Mesh = mMeshComp.lock();

	if (Mesh)
	{
		Mesh->SetDissolveEnable(0, true);
	}

	auto BoxCollision = mBox.lock();
	if (BoxCollision)
	{
		BoxCollision->SetEnable(false);
	}

	mIsDead = true;
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

void CPlayer::RotateLeft()
{
	AddRelativeRotationZ(360.f * CTimeManager::GetDeltaTime());
}

void CPlayer::RotateRight()
{
	AddRelativeRotationZ(-360.f * CTimeManager::GetDeltaTime());
}

void CPlayer::Fire()
{
	auto World = mWorld.lock();

	if (World)
	{
		auto Bullet = World->CreateActor<CBullet>("Bullet").lock();

		auto Input = World->GetInput().lock();

		if (Bullet && Input)
		{
			const FVector3 MousePos(Input->GetWorldMousePos().x, Input->GetWorldMousePos().y, 0.f);

			FVector3 FireDirection = MousePos - GetWorldPos();
			FireDirection.Normalize();
			Bullet->SetWorldPos(FireDirection * 50.f + GetWorldPos());
			Bullet->InitializeProjectile(FireDirection, 1000.f);
		}
	}
}

void CPlayer::Dissolve()
{
	mIsDissolve = true;
	mDissolveProgress = 0.f;

	auto Mesh = mMeshComp.lock();

	if (Mesh)
	{
		Mesh->SetDissolveEnable(0, true);
	}

}

void CPlayer::TestDamage()
{
	TakeDamage(1.f, "Player", GetThisPtr<CActor>());
}

void CPlayer::TestInput()
{
	// Player - Create and attach to weapon.
	if (CurrentSpawnNum >= MaxSpawnNum) return;

	auto World = GetWorld().lock();
	if (World)
	{
		std::shared_ptr<CWeapon> NewWeapon;

		// NewWeapon = World->CreateActor<CMeleeWeapon>("MeleeWeapon").lock();
		/*if (rand() % 2)
		{
		}
		else*/
		NewWeapon = World->CreateActor<CRangedWeapon>("RangedWeapon").lock();

		if (NewWeapon)
		{
			NewWeapon->AttachActorToActor(GetThisPtr<CActor>());
			NewWeapon->SetOwner(GetThisPtr<CActor>());
			
			Weapons.push_back(NewWeapon);

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
					Weapon->SetWorldScale(40.f, 40.f);
					Weapon->SetRelativeRotationZ(Angle * Index);
				}
			}

			CurrentSpawnNum++;
		}
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
				NewWeapon->InitializeSettings(WeaponData);
			
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
			Weapon->SetWorldScale(40.f, 40.f);
			Weapon->SetRelativeRotationZ(Angle * Index);
		}
	}
}
