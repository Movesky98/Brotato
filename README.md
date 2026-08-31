# Brotato

DirectX 11 기반 자체 2D 게임 엔진과, 그 엔진 위에 구현한 **Brotato** 모작입니다.
엔진(`GameEngine`)을 정적 라이브러리로 빌드하고 클라이언트(`Client`)가 이를 링크하는 구조입니다.

> 학습·포트폴리오 목적의 개인 프로젝트입니다.
> 원작 [Brotato](https://store.steampowered.com/app/1942280/Brotato/)는 Blobfish의 저작물이며, 본 리포지토리는 원작과 무관합니다.

## 환경

| 항목 | 값 |
|---|---|
| 언어 | C++20 (`stdcpp20`) |
| 그래픽 | Direct3D 11 |
| 플랫폼 | Windows x64, Win32 desktop |
| 툴셋 | MSVC `v145` |
| 외부 의존성 | FMOD (사운드), DirectXTex (텍스처 로딩) |

## 구조

```
GameEngine/          엔진 소스 → GameEngine.lib
  World/             액터·컴포넌트·충돌·UI·타일맵
  Math/              벡터 / 행렬
  Asset/             서드파티 헤더 (FMOD, DirectXTex)
  Copy.bat           헤더를 Engine/Include 로, EngineAsset 을 Binary/ 로 복사

Client/              게임 소스 → Client.exe
  World/             게임플레이 (플레이어·적·무기·상점·UI·에디터)

Engine/
  Include/           Copy.bat 이 생성 (커밋 대상 아님)
  Binary/            링크용 라이브러리 + EngineAsset(셰이더 원본, 폰트)

Binary/              실행 디렉터리 (작업 폴더)
  Asset/CSV/         게임 데이터 테이블
```

## 엔진 주요 기능

- **액터 / 컴포넌트 모델** — `Actor` + `ActorComponent` / `SceneComponent` 계층, `CharacterMovementComponent`·`ProjectileMovementComponent`·`CameraComponent`·`WidgetComponent`
- **충돌** — AABB / OBB(분리축) / 원, `CollisionQuadTree` 로 브로드 페이즈 분할, `CollisionInfoManager` 로 채널별 응답 관리
- **렌더링** — `Device`(스왑체인·렌더타겟), `RenderManager`, 블렌드/뎁스스텐실 상태 객체, HLSL 셰이더 6종
- **2D 애니메이션** — `Animation2DComponent` + `Animation2DSequence` 시퀀스 재생
- **타일맵** — `TileMapActor` / `TileMapComponent` / `TileMapRender`
- **UI** — `Widget` 트리 (`Button`, `ProgressBar`, `TextBlock`, `Image`, `NumberWidget`, `WidgetContainer`)와 `UIManager`
- **AI** — `AIComponent` + `AIState` + `BlackBoard`
- **사운드** — FMOD 래핑 `SoundComponent`
- **데이터 주도 설계** — `CSVReader` / `DataManager` / `DataAsset` 로 CSV 테이블을 파싱해 아이템·무기·적·스킬·상점 데이터를 구성

## 게임 구현

`StartWorld`(타이틀) → `MainWorld`(전투) → `ShopMenu`(상점) 루프.
플레이어 스탯·근접/원거리 무기·적 스폰과 웨이브·투사체·아이템 슬롯·상점 리롤·일시정지 메뉴·`EditorWorld`(레벨 배치 에디터)를 구현했습니다.

## 빌드

1. `GameEngine.slnx` 를 Visual Studio 로 엽니다.
2. 구성은 `Debug` / `x64`.
3. `GameEngine` → `Client` 순으로 빌드합니다. (`GameEngine` 빌드 후 `Copy.bat` 이 헤더와 엔진 에셋을 배치합니다.)
4. 작업 디렉터리는 `Binary/` 입니다.

### 에셋

원작 저작물이라 텍스처(`Binary/Asset/Texture/`)와 사운드(`Binary/Asset/Sound/`)는 리포지토리에 포함하지 않았습니다.
해당 폴더를 채워야 실행됩니다. CSV 데이터 테이블은 포함되어 있습니다.
