# 목표와 할 일 (ROADMAP)

nexa-shortcut 프로젝트의 **목표**와 그 목표를 이루기 위한 **할 일**을 한곳에 모은 문서.
설계 근거는 [DESIGN.md](DESIGN.md), 완료된 작업의 상세 이력은 [CHANGELOG.md](CHANGELOG.md),
패키지 배포 현황은 [PACKAGING.md](PACKAGING.md)에 있다. 이 문서는 **앞으로 할 일**에 집중한다.

- 기준일: 2026-07-30
- 표기: `[ ]` 미착수 · `[~]` 진행 중 · `[x]` 완료

---

## 1. 목표 한눈에 보기

| # | 목표 | 산출물 | 상태 |
|---|---|---|---|
| 1 | **nShiftSpace** — Shift+Space 한/영 전환 단일 기능 초경량 상주 프로그램 | `nShiftSpace-x64/x86.exe` (4.5KB) | ✅ 완료 (v0.1.0 배포) |
| 2 | **nexa-mapper** — `mappings.ini`로 사용자가 정의하는 단순 키 재매핑 엔진 | `nMapper-x64/x86.exe` + `mappings.ini` | ⬜ 미착수 |
| 3 | **exe 내보내기** — 현재 매핑만 내장한 독립 exe 생성 (스텁 복사 + payload 덧붙이기) | 엔진 내 "실행파일 만들기" 기능 | ⬜ 미착수 (목표 2 선행) |
| — | **배포 채널 확보** — winget / Chocolatey 정식 등록 | 두 채널 설치 명령 동작 | ✅ 완료 (2026-07-30) |

### 모든 목표에 공통으로 적용되는 불변 원칙

DESIGN.md의 "공통 초경량 원칙"이 곧 완료 기준의 일부다. 새 기능을 넣을 때마다 아래를 만족해야 한다.

- C + Win32 API만 사용, GUI 프레임워크·CRT 미링크 (`-nostdlib`, 진입점 `start`)
- 링크 라이브러리는 kernel32 / user32 / shell32 로 제한
- **exe 1개당 8KB 이내** (CI가 초과 시 빌드 실패 — [.github/workflows/build.yml](../.github/workflows/build.yml) 크기 회귀 검사)
- 설정 GUI를 만들지 않는다 (설정은 텍스트 파일 편집)
- 모든 변경은 CHANGELOG.md에 요청/분석/설계/개발 4개 절로 기록

---

## 2. 배포 채널 — 등록 완료, 남은 검증 1건

두 채널 모두 최초 등록 심사를 통과해 설치 명령이 동작한다. 상세는 [PACKAGING.md](PACKAGING.md) 참고.

- [x] Chocolatey `nshiftspace` 0.1.0 모더레이션 승인 (2026-07-30 21:01)
- [x] winget `SosomLab.nShiftSpace` 0.1.0 PR 병합 —
  [microsoft/winget-pkgs#397365](https://github.com/microsoft/winget-pkgs/pull/397365) (2026-07-17 05:23)
- [x] README·PACKAGING.md의 "심사 중" 안내 제거, 상태 표 갱신
- [x] Chocolatey 자동 게시 경로 검증 — v0.1.0 태그 런에서 CI `chocolatey` 잡이 `choco push`까지 성공
- [ ] **다음 버전(v0.1.1) 태그 시 winget 자동 제출 검증** — `wingetcreate` 업데이트 PR 자동 제출은
  아직 한 번도 실행된 적이 없다 (0.1.0 최초 등록은 수동 PR이었고, CI 잡은 병합 이후 버전부터 동작)

> ⚠️ 운영 규칙: **이미 게시한 태그는 재발행 금지** (릴리스 zip의 SHA256이 매니페스트에 고정됨).
> 수정이 필요하면 새 버전으로 배포한다.

---

## 3. 목표 2 — nexa-mapper (다음 개발 대상)

설계는 [DESIGN.md "목표 2 아키텍처"](DESIGN.md)에 이미 확정되어 있다. 남은 것은 구현.
아래 순서대로 진행하면 각 단계마다 동작하는 결과물이 나온다.

### 2-1. 기반 (엔진 골격)

- [ ] `src/nMapper.c` 신설 — nShiftSpace의 골격(뮤텍스·숨은 윈도우·트레이·메시지 루프) 재사용
- [ ] 공통 코드 정리 방침 결정: 헤더 분리(`src/win32_min.h`) vs 파일 복제
      — 초경량·단순성을 해치지 않는 쪽 선택
- [ ] `Makefile` / `build.bat`에 nMapper 타깃 추가, CI 크기 검사 대상에 포함

### 2-2. 설정 파일 파서

- [ ] exe 옆 `mappings.ini` 읽기 (`CreateFile`+`ReadFile`, 최대 크기 제한 — CRT 미사용)
- [ ] 줄 단위 파서: `원본키 = 대상키`, `;` 주석, 공백 무시
- [ ] 키 이름 → VK 코드 정적 테이블 (Hangul, Hanja, Esc, Tab, F1–F24, 볼륨/미디어 키, A–Z, 0–9)
      — 비교는 `lstrcmpiW`
- [ ] 매핑 테이블 정적 배열(최대 32개), 파싱 실패 줄은 무시하되 트레이 툴팁/메시지로 알림

### 2-3. 키 가로채기 이원화

- [ ] 수식키+일반키 조합 → `RegisterHotKey` 경로 (nShiftSpace 로직 일반화)
- [ ] 단일 키 재매핑 → `SetWindowsHookEx(WH_KEYBOARD_LL)` 경로
- [ ] **훅은 단일 키 매핑이 하나 이상 있을 때만 설치** (조합 전용 설정이면 훅 없음)
- [ ] `SendInput` 합성 시 `dwExtraInfo` 시그니처로 훅 재진입(무한 루프) 차단
- [ ] 원본 키 삼키기(훅 콜백 1 반환) 동작 확인

### 2-4. 트레이 UX

- [ ] 트레이 메뉴: 설정 열기(메모장) / 다시 읽기 / 종료
- [ ] 설정 파일이 없으면 주석 달린 기본 `mappings.ini` 생성
- [ ] nMapper 전용 아이콘 생성 (`tools/make_icon.py` 확장, 1bpp 16×16 유지)

### 2-5. 검증·문서

- [ ] 수동 테스트 시나리오 문서화 (조합 매핑 / 단일 키 매핑 / 잘못된 설정 / 다시 읽기 / explorer 재시작)
- [ ] 크기 실측 후 8KB 예산 내 확인, 초과 시 감량
- [ ] README에 nMapper 사용법 절 추가, DESIGN.md의 "설계" → "구현됨"으로 갱신
- [ ] 릴리스: 태그 배포에 nMapper exe 포함 여부 결정 (별도 패키지 vs 동일 zip)

---

## 4. 목표 3 — exe 내보내기 (목표 2 완료 후)

방식은 확정: `[스텁 exe] + ["NEXA1" 마커] + [설정 텍스트] + [길이 4바이트]`.

- [ ] 시작 시 자기 exe 끝에서 **역방향 스캔**으로 마커 탐색 (코드서명 뒤따름 대비)
- [ ] 마커 있음 → 내장 설정으로 동작(잠금 모드, 트레이 메뉴는 "종료"만) / 없음 → `mappings.ini` 모드
- [ ] 트레이 메뉴 "실행파일 만들기": 자기 exe 복사 + 현재 설정 덧붙이기 (`GetModuleFileName`, `CopyFile`)
- [ ] 저장 위치 선택 방법 결정 — 파일 대화상자(comdlg32 추가 링크) vs 고정 경로 생성 (초경량 원칙과 저울질)
- [ ] 생성된 exe 실행 검증: 설정 없이 단독 동작, 크기 = 엔진 + 수백 바이트
- [ ] 백신 오탐 여부 확인 (payload append 방식 특성상 점검 필요)
- [ ] README/DESIGN 문서화

---

## 5. 상시 운영 과제 (기한 없음)

- [ ] Windows 11 / 관리자 권한 창 포커스 시 동작 한계 재확인 및 README 명시 유지
- [ ] 신규 릴리스마다 크기 회귀 검사 통과 확인 (CI 자동)
- [ ] 제품 소개 페이지 https://sosomlab.com/apps/nexa-shortcut/ 와 README 내용 동기화
- [ ] 사용자 이슈·요청 대응 (GitHub Issues)

---

## 6. 완료된 일 (요약)

상세 이력은 [CHANGELOG.md](CHANGELOG.md).

- [x] 목표 1 구현 — `src/nShiftSpace.c`, RegisterHotKey 기반, x64/x86 각 4.5KB
- [x] 자체 아이콘 내장 (198바이트 1bpp ICO, `tools/make_icon.py`)
- [x] 중복 실행 방지, explorer 재시작 시 트레이 아이콘 복구
- [x] 크로스 빌드(macOS mingw-w64) + Windows 빌드(MSYS2, MSVC) 경로 확보
- [x] GitHub Actions: 빌드 + 8KB 크기 회귀 검사 + `v*` 태그 시 Release 자동 첨부
- [x] v0.1.0 릴리스 및 Chocolatey / winget 패키지 명세·자동 게시 파이프라인 구성
- [x] winget(2026-07-17) · Chocolatey(2026-07-30) 최초 등록 심사 통과 — 두 채널 설치 명령 동작
- [x] 문서화 — README, DESIGN, CHANGELOG, PACKAGING, DEV-ENV-WINDOWS
