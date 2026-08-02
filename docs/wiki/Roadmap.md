# 로드맵

nexa-shortcut은 **초경량 단축키 유틸리티 모음**을 목표로 합니다.
목표 1은 완료되었고, 2·3은 개발 예정입니다.

| # | 목표 | 산출물 | 상태 |
|---|---|---|---|
| 1 | **nShiftSpace** — Shift+Space 한/영 전환 | `nShiftSpace-x64/x86.exe` (4.5KB) | ✅ 완료 (v0.1.0) |
| 2 | **nexa-mapper** — 사용자 정의 키 재매핑 엔진 | `nMapper.exe` + `mappings.ini` | ⬜ 예정 |
| 3 | **exe 내보내기** — 설정 내장 독립 exe 생성 | 엔진 내 "실행파일 만들기" | ⬜ 예정 |
| — | 배포 채널 (winget · Chocolatey) | 두 채널 설치 명령 동작 | ✅ 완료 |

---

## 목표 2 — nexa-mapper

**"엔진 하나 + 텍스트 설정 파일"** 구조. 설정 GUI는 만들지 않습니다 (초경량 유지).
exe 옆의 `mappings.ini`를 메모장으로 편집하고, 트레이 메뉴에는 "설정 열기 / 다시 읽기"만 둡니다.

```ini
; 원본키 = 대상키   (수식키는 + 로 조합)
Shift+Space = Hangul
CapsLock    = Esc
RCtrl       = Hanja
F1          = VolumeMute
```

매핑 종류에 따라 키 가로채기 방식을 나눕니다.

| 매핑 형태 | 방식 | 이유 |
|---|---|---|
| 수식키+일반키 (`Shift+Space`) | `RegisterHotKey` | 코드 단순, CPU 0 |
| 단일 키 (`CapsLock` → `Esc`) | `SetWindowsHookEx(WH_KEYBOARD_LL)` | `RegisterHotKey`는 단일 무수식 키에 부적합 |

훅은 **단일 키 매핑이 하나라도 있을 때만** 설치합니다 — 조합 매핑만 쓰면 훅 없이 동작합니다.

## 목표 3 — exe 내보내기

컴파일러 없이 **스텁 복사 + 설정 덧붙이기** 방식입니다.

```
[nexa-mapper.exe 스텁] + ["NEXA1" 마커] + [설정 텍스트] + [길이 4바이트]
```

엔진이 시작할 때 자기 exe의 끝을 검사해서, 마커가 있으면 내장 설정으로(잠금 모드),
없으면 옆의 `mappings.ini`로 동작합니다. 생성기도 별도 스텁 파일도 필요 없고,
결과물 크기는 **엔진 크기 + 설정 몇백 바이트**입니다.

## 모든 목표에 적용되는 불변 원칙

- C + Win32 API만 사용, GUI 프레임워크·CRT 미링크
- 링크 라이브러리는 kernel32 / user32 / shell32 로 제한
- **exe 1개당 8KB 이내** (CI가 초과 시 빌드 실패)
- 설정 GUI를 만들지 않는다 (설정은 텍스트 파일 편집)

---

## 상세 문서

- 할 일 목록과 진행 상태: [docs/ROADMAP.md](https://github.com/SosomLab/nexa-shortcut/blob/main/docs/ROADMAP.md)
- 설계 근거: [docs/DESIGN.md](https://github.com/SosomLab/nexa-shortcut/blob/main/docs/DESIGN.md)
- 변경 이력: [docs/CHANGELOG.md](https://github.com/SosomLab/nexa-shortcut/blob/main/docs/CHANGELOG.md)

기능 제안이나 의견은 [Issues](https://github.com/SosomLab/nexa-shortcut/issues)로 남겨주세요.
