# nexa-shortcut

윈도우용 초경량 단축키 유틸리티 모음. GUI 프레임워크와 CRT 없이 순수 Win32 API로만 작성해
실행 파일 크기와 메모리 사용을 최소화한다.

## nShiftSpace (목표 1 — 완료)

**Nexa ShiftSpace** — Shift+Space로 한/영 전환만 하는 단일 기능 상주 프로그램.

- 파일 크기: **4KB** (참고: 원조 jwShiftSpaceKey.exe는 12KB)
- 동작: Shift+Space 입력 시 한/영 키(VK_HANGUL)를 대신 눌러줌
- 트레이 아이콘 우클릭 → 종료
- 중복 실행 방지, 탐색기 재시작 시 트레이 아이콘 자동 복구

### 빌드 (macOS/Linux에서 크로스 컴파일)

```bash
brew install mingw-w64   # 또는 apt install gcc-mingw-w64
make                     # dist/nShiftSpace-x64.exe, dist/nShiftSpace-x86.exe 생성
```

### 시작 프로그램 등록

`Win+R` → `shell:startup` → 폴더에 exe 바로가기를 넣으면 로그인 시 자동 실행.

## 로드맵

- **목표 2 — nexa-mapper**: `mappings.ini`로 사용자가 직접 정의하는 단순 키 재매핑 엔진
- **목표 3 — exe 내보내기**: 현재 매핑만 내장한 독립 exe 생성 (스텁 복사 + 설정 덧붙이기 방식)

상세 설계는 [docs/DESIGN.md](docs/DESIGN.md), 변경 이력은 [docs/CHANGELOG.md](docs/CHANGELOG.md) 참고.

## 라이선스

[MIT License](LICENSE) — 누구나 무료로 사용·복사·수정·배포할 수 있습니다.
