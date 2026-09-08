#pragma once
#include "World/Actor.h"

enum class ESpawnType
{
	None = -1,
	Enemy,
	Structure,
	Boss,
	MAX
};

class CSpawnPoint : public CActor
{
public:
	CSpawnPoint();
	CSpawnPoint(const CSpawnPoint& src);
	CSpawnPoint(CSpawnPoint&& src) noexcept;
	virtual ~CSpawnPoint();

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void Destroy();

protected:
	virtual CSpawnPoint* Clone()
	{
		return new CSpawnPoint(*this);
	}

protected:
	std::weak_ptr<class CMeshComponent> mMeshComponent;
	std::weak_ptr<class CColliderBox2D> mBoxCollision;
	std::weak_ptr<class CDataAsset> mDataAsset;

	ESpawnType mSpawnType = ESpawnType::None;

	std::function<void(const FVector3 /* SpawnPos */, ESpawnType /* Type */, std::weak_ptr<class CDataAsset> /* Data */)> mOnSpawnEntityDelegate;

	/* 약 4번 정도 깜빡인 듯 */
	float mSpawnDelayTime = 1.5f;
	float mElapsedTime = 0.f;
	float mBlinkInterval;

	void OnCollisionBeginOverlap(const FVector3& HitPoint, const FVector3& Normal, std::weak_ptr<class CCollider> Collider);

public:
	void InitializeSpawnSettings(ESpawnType SpawnType, std::weak_ptr<class CDataAsset> Data);

	template<typename T>
	void SetOnSpawnEntityFunction(T* Object, void (T::* Func)(const FVector3, ESpawnType, std::weak_ptr<class CDataAsset>))
	{
		mOnSpawnEntityDelegate = std::bind(Func, Object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	}

};

