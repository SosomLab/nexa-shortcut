# nexa-shortcut Wiki

윈도우용 **초경량 단축키 유틸리티 모음**. GUI 프레임워크와 CRT 없이 순수 Win32 API로만
작성해 실행 파일 크기와 메모리 사용을 최소화했습니다.

- 제품 소개: https://sosomlab.com/apps/nexa-shortcut/
- 저장소: https://github.com/SosomLab/nexa-shortcut
- 다운로드: [GitHub Releases](https://github.com/SosomLab/nexa-shortcut/releases)
- 라이선스: MIT

---

## nShiftSpace

**Shift+Space로 한/영을 전환**하는 단일 기능 상주 프로그램입니다.

| 항목 | 값 |
|---|---|
| 파일 크기 | **4.5KB** (원조 jwShiftSpaceKey.exe는 12KB) |
| 지원 | Windows 10 / 11, x64 · x86 |
| 현재 버전 | 0.1.0 |
| 상주 방식 | 트레이 아이콘 ('가·A' 픽셀 아트, 198바이트 아이콘 내장) |

### 30초 설치

```powershell
winget install SosomLab.nShiftSpace
nshiftspace
```

Chocolatey를 쓴다면 **관리자 권한** PowerShell에서:

```powershell
choco install nshiftspace -y
```

설치 후 로그인할 때마다 자동으로 뜨게 하려면 → **[자동 실행 설정](Autostart)** (한 번만 하면 됩니다)

---

## 문서 안내

| 페이지 | 내용 |
|---|---|
| **[설치하기](Installation)** | winget · Chocolatey · zip 직접 다운로드 3가지 방법과 채널별 차이 |
| **[자동 실행 설정](Autostart)** | 로그인 시 자동 실행. winget 경로 함정과 관리자 권한 실행까지 |
| **[사용법](Usage)** | 동작 방식, 트레이 메뉴, 알아둘 제약 |
| **[문제 해결](Troubleshooting)** | 설치·실행·단축키가 안 될 때 |
| **[직접 빌드하기](Build)** | macOS 크로스 컴파일 / MSYS2 / MSVC |
| **[릴리스와 배포](Releasing)** | (메인테이너용) 태그 → Release → 패키지 자동 게시 |
| **[로드맵](Roadmap)** | 앞으로 만들 것 — nexa-mapper, exe 내보내기 |

---

## 설치 채널 상태

| 채널 | 식별자 | 상태 |
|---|---|---|
| winget | `SosomLab.nShiftSpace` | ✅ 등록 완료 (2026-07-17) |
| Chocolatey | `nshiftspace` | ✅ 승인 완료 (2026-07-30) |

---

> 이 Wiki는 **사용자용 안내**입니다. 설계 근거·변경 이력 등 개발 문서는 저장소의
> [docs/](https://github.com/SosomLab/nexa-shortcut/tree/main/docs) 폴더에 있습니다.
