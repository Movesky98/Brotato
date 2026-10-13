# Brotato

**Brotato** 모작입니다. 강의를 따라 구현한 DirectX 11 2D 엔진(`GameEngine`)을 정적 라이브러리로 두고,
그 위에 게임플레이 전반(`Client`)을 구현했습니다.

> 학습·포트폴리오 목적의 개인 프로젝트입니다.
> 엔진(`GameEngine/`)은 강의 과정에서 작성한 코드이며, 이 리포지토리에서 봐주실 부분은 `Client/` 입니다.
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
Client/              게임 소스 → Client.exe
  World/             게임플레이 (플레이어·적·무기·스킬·상점·UI)

GameEngine/          엔진 소스 → GameEngine.lib  ※ 강의 과정에서 작성
  World/             액터·컴포넌트·충돌·UI·타일맵
  Math/              벡터 / 행렬
  Asset/             서드파티 헤더 (FMOD, DirectXTex)
  Copy.bat           헤더를 Engine/Include 로, EngineAsset 을 Binary/ 로 복사

Engine/
  Include/           Copy.bat 이 생성 (커밋 대상 아님)
  Binary/            링크용 라이브러리 + EngineAsset(셰이더 원본, 폰트)

Binary/              실행 디렉터리 (작업 폴더)
  Asset/CSV/         게임 데이터 테이블
  Asset/Texture/     텍스처 (비공개 서브모듈)
```

## 빌드

1. `GameEngine.slnx` 를 Visual Studio 로 엽니다.
2. 구성은 `Debug` / `x64`.
3. `GameEngine` → `Client` 순으로 빌드합니다. (`GameEngine` 빌드 후 `Copy.bat` 이 헤더와 엔진 에셋을 배치합니다.)
4. 작업 디렉터리는 `Binary/` 입니다.

### 에셋

텍스처는 원작 저작물이라 비공개 리포지토리를 서브모듈(`Binary/Asset/Texture/`)로 연결해 두었습니다.
접근 권한이 없으면 이 폴더가 비어 있어 빌드는 되지만 실행은 되지 않습니다. CSV 데이터 테이블은 포함되어 있습니다.
