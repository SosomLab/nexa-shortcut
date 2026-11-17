# 패키지 관리자 등록 (Chocolatey / winget)

nShiftSpace의 Windows 패키지 관리자 배포 현황, 등록 절차, 상태 확인 방법을 기록한다.

## 한눈에 보기

| 채널 | 패키지 식별자 | 설치 명령 | 상태 (2026-11-18 00:28 조회) |
|---|---|---|---|
| Chocolatey | `nshiftspace` | `choco install nshiftspace` | 0.1.0·0.2.0·0.3.0 승인 / **0.3.1 제출 — 자동 검증·설치 테스트 통과, 모더레이터 심사 대기** (`Submitted`, 검증·테스트 `Passing`, 스캔 `Pending`) / **0.3.2는 guard가 보류** (0.3.1 심사 대기) — 0.3.1 승인 뒤 `force=true` 수동 실행으로 제출 |
| winget | `SosomLab.nShiftSpace` | `winget install SosomLab.nShiftSpace` | 0.1.0·0.2.0·0.3.0 등록 — 0.2.0 PR [#454746](https://github.com/microsoft/winget-pkgs/pull/454746)·0.3.0 PR [#454771](https://github.com/microsoft/winget-pkgs/pull/454771) 2026-10-20 병합 / **0.3.1 PR [#482170](https://github.com/microsoft/winget-pkgs/pull/482170) 2026-11-17 20:49 병합 — `winget show` 0.3.1** / **0.3.2 PR [#482402](https://github.com/microsoft/winget-pkgs/pull/482402) 제출 — 검증·승인 대기** (라벨 `New-Manifest`) |
| Chocolatey (LED판) | `nshiftspace-led` | `choco install nshiftspace-led` | 0.3.0 승인 (2026-10-21) / **0.3.1 제출 — 자동 검증·설치 테스트 통과, 모더레이터 심사 대기** / **0.3.2는 guard가 보류** |
| winget (LED판) | `SosomLab.nShiftSpace.LED` | `winget install SosomLab.nShiftSpace.LED` | **미등록** — 0.3.0 최초 등록 PR [microsoft/winget-pkgs#454772](https://github.com/microsoft/winget-pkgs/pull/454772) 검증 통과, 모더레이터 승인 대기. **0.3.1·0.3.2는 guard가 보류** (직전 버전 PR이 열려 있음) — #454772 병합 뒤 태그 `v0.3.2`에서 `force=true` 수동 실행으로 제출 |

설치되는 버전은 **winget 일반판 0.3.1**, **Chocolatey 일반판·LED판 0.3.0**이다 (Chocolatey는 0.3.1이 승인되면 0.3.1). winget LED판은 PR이 병합돼야 설치된다.
v0.3.1(2026-11-17 태그)은 GitHub Release에 zip 4종이 올라가 있다.
0.3.1은 프로그램이 0.3.0과 같고 Chocolatey 패키지의 설치 방식만 바뀐 버전이다.
v0.3.2(2026-11-18)는 프로그램에 메뉴 머리 줄과 "시작 메뉴에 바로가기" 메뉴를 넣은 버전이다 — GitHub Release에 zip 4종이 올라갔고, 채널로는 winget 일반판만 제출됐다 (Chocolatey 2종과 winget LED판은 직전 버전 심사가 끝나야 낸다).

두 채널 모두 GitHub Release의 zip(`nShiftSpace-x64.zip` / `nShiftSpace-x86.zip`, LED판은 `nShiftSpace-led-*.zip`)을
다운로드해 설치하는 원격형 패키지다. LED판은 **별도 패키지 이름**으로 게시하고 기존 이름은 일반판을 유지한다
(두 판이 같은 단축키·뮤텍스를 쓰므로 한 PC에 하나만 설치하도록 설명에 명시).
[GitHub Releases](https://github.com/SosomLab/nexa-shortcut/releases)에서 zip을 직접 받아 쓰는 것도 여전히 가능.

### 이미 등록된 버전은 제외, 직전 버전 검수 대기 중이면 보류 (2026-10-17 도입)

`v*` 태그 CI는 `guard` 잡에서 패키지별로 같은 버전이 이미 있는지 조회하고, 있으면 그 패키지의 게시만 건너뛴다.
GitHub Release 생성과 nupkg·매니페스트 아티팩트는 그대로 남는다. 방식은 nexa-dir3·nexa-sql의 guard 잡을 따랐다.

| 채널 | 제외 조건 |
|---|---|
| Chocolatey | 피드에 같은 Id+Version 항목이 있다 (승인됨·모더레이션 대기·반려 모두 포함) |
| winget | winget-pkgs에 `manifests/s/SosomLab/<패키지>/<버전>`이 병합됐거나, 제목에 그 식별자·버전이 있는 열린 PR이 있다 |

직전 버전이 아직 검수 대기 중이면 새 버전도 보류한다(nexa-sql 방식). 검수 큐가 엉키는 것을 막기 위해서다.

| 채널 | 보류 조건 |
|---|---|
| Chocolatey | 현재보다 앞선 정식 태그 중 하나라도 피드 상태가 `Submitted`다 (승인·반려·미제출은 막지 않는다) |
| winget | 같은 식별자의 다른 버전 PR이 열려 있다 (`SosomLab.nShiftSpace`와 `.LED`는 따로 판정) |

**첫 실전 — v0.3.1 태그 런(2026-11-17)**: Chocolatey 2종과 winget 일반판은 "미등록 · 대기 중인 직전 버전 없음 — 게시한다",
winget LED판은 "직전 버전 PR이 검수 대기 중 — 0.3.1 은(는) 보류한다"(#454772)로 판정했고, 뒤 잡들이 그대로 따랐다.

**두 번째 실전 — v0.3.2 태그 런(2026-11-18)**: Chocolatey 보류 경로가 처음 돌았다 — "직전 버전(0.3.1)이 검수 대기 중 — 0.3.2 은(는) 보류한다"(2종 모두),
pack은 하고 push 단계만 건너뛰었다. winget 일반판은 게시, LED판은 다시 보류.
제외 판정(같은 버전이 이미 등록됨)과 `force=true` 경로는 아직 실전에서 돌지 않았다.

조회가 실패하면 중복 제출을 피하려고 제외 쪽으로 판정한다. 보류된 버전을 직전 버전 승인 뒤에 내거나,
반려 뒤 같은 버전을 다시 내야 하면 Actions에서 태그 ref를 골라 `build` 워크플로를 수동 실행하고 `force=true`를 켠다.

---

## 설치 안내 (사용자용)

> 📖 **사용자에게 안내할 때는 [Wiki](https://github.com/SosomLab/nexa-shortcut/wiki)를 가리킨다.**
> 같은 내용을 읽기 쉽게 페이지로 나눠 두었다
> ([설치하기](https://github.com/SosomLab/nexa-shortcut/wiki/Installation) ·
> [자동 실행 설정](https://github.com/SosomLab/nexa-shortcut/wiki/Autostart) ·
> [문제 해결](https://github.com/SosomLab/nexa-shortcut/wiki/Troubleshooting)).
> **Wiki 원고의 정본은 [docs/wiki/](wiki/)** 이며, 이 절의 내용을 고치면
> 해당 원고도 함께 고치고 `./tools/publish-wiki.sh`로 재발행해야 한다.

README에도 요약이 있으며, 여기서는 채널별 차이와 문제 해결까지 다룬다.

### 명령 대조표

| 작업 | winget | Chocolatey |
|---|---|---|
| 사전 준비 | Windows 11 / Windows 10 1809+ 기본 포함 (없으면 Store의 "앱 설치 관리자") | Chocolatey 설치 필요 — https://chocolatey.org/install |
| 권한 | 일반 사용자 | **관리자 권한 PowerShell** |
| 설치 | `winget install SosomLab.nShiftSpace` | `choco install nshiftspace -y` |
| 실행 | `nshiftspace` (포터블 별칭) | 0.3.0까지 `nShiftSpace-x64` (또는 `-x86`) / 0.3.1부터 시작 메뉴 **nShiftSpace**·**nShiftSpace LED** 또는 tools 폴더 exe |
| 업그레이드 | `winget upgrade SosomLab.nShiftSpace` | `choco upgrade nshiftspace -y` |
| 제거 | `winget uninstall SosomLab.nShiftSpace` | `choco uninstall nshiftspace -y` |
| 설치 경로 | `%LOCALAPPDATA%\Microsoft\WinGet\Packages\` (별칭 링크는 `…\WinGet\Links\`) | `%ChocolateyInstall%\lib\nshiftspace\tools\` |
| 설치되는 아키텍처 | 매니페스트의 x64/x86 중 OS에 맞는 것 | `Install-ChocolateyZipPackage`의 `url`/`url64bit` 중 OS에 맞는 것 하나 |

### 채널별 동작 차이

- **실행 중 프로세스 처리**: Chocolatey는 `chocolateybeforemodify.ps1`이 업그레이드/제거 전에
  실행 중인 nShiftSpace를 종료한다 (LED 패키지는 `nShiftSpace-led*`만). **winget에는 이런 처리가 없으므로** 트레이에서 먼저 종료해야
  파일 잠김으로 실패하지 않는다.
- **shim과 콘솔**: **0.3.1부터 패키지는 shim을 만들지 않는다** (아래 "0.3.1에서 패키지를 고쳤다"). 이하는 0.3.0까지의 동작이다.
  Chocolatey는 tools 폴더의 exe에 shim(`%ChocolateyInstall%\bin\nShiftSpace-*.exe`)을 만든다.
  0.3.0까지의 설치 스크립트가 만들던 `nShiftSpace-*.exe.gui` 마커는 shim이 **대상 종료를 기다리지 않고 바로 빠지게** 할 뿐이다.
  shim 자체는 콘솔 서브시스템 프로그램이라, 콘솔 없이 실행되면(자동 실행 `Run` 값·바로가기·탐색기 더블클릭) **콘솔 창이 잠깐 떴다
  사라진다.** 터미널에서 `nShiftSpace-x64`로 실행할 때는 새 창이 뜨지 않는다.
  - 2026-11-17 실측(`nshiftspace-led` 0.3.0): shim 22,528바이트·서브시스템 3(콘솔), tools 폴더 exe 6,144바이트·서브시스템 2(GUI).
    `Run` 값이 shim을 가리키던 PC에서 로그인마다 콘솔 창이 0.5~1.1초 떴고(Shell-Core 이벤트 9707→9708 간격),
    shim을 `--shimgen-noop`으로 실행해 `conhost.exe`가 자식으로 생기는 것을 확인했다.
  - **자동 실행에는 shim이 아니라 tools 폴더의 exe를 등록한다.** 트레이 메뉴 "로그인 시 자동 실행"은 실행 중인 exe 경로
    (`GetModuleFileName`)를 쓰므로 shim으로 실행했더라도 tools 폴더 경로가 기록된다.
  - **0.3.1에서 패키지를 고쳤다** (릴리스 2026-11-17, Chocolatey 심사 중. 커밋 2e7ae67·b09e736, 두 패키지 공통):
    - 설치 스크립트가 `.gui` 대신 `<exe>.ignore` 마커를 만들어 **shim을 생성하지 않는다.** `nShiftSpace-x64` 명령이 없어지므로
      공용 시작 메뉴에 바로가기를 둔다 (일반판 `nShiftSpace.lnk`, LED판 `nShiftSpace LED.lnk`).
    - **0.3.0 이하에서 올라오는 사용자**: `HKCU\…\Run`의 `nShiftSpace` 값이 `chocolatey\bin`의 이 패키지 shim을 가리키면
      tools 폴더 exe 경로(`"…"` 따옴표 형식)로 옮긴다. tools 폴더·다른 판(일반↔LED)·무관한 경로를 가리키는 값은 건드리지 않는다.
    - `chocolateyuninstall.ps1` 신설 — 제거할 때 시작 메뉴 바로가기를 지우고, `Run` 값이 이 패키지의 tools 폴더 exe나
      옛 shim을 가리킬 때만 그 값을 지운다. 업그레이드 때는 이 스크립트가 돌지 않으므로 자동 실행 설정은 유지된다
      (Chocolatey 문서: uninstall 스크립트는 제거 때만, beforemodify는 업그레이드·제거 때 실행).
    - 0.3.0 이하가 만든 shim은 설치 스크립트가 `Uninstall-BinFile`로 직접 지운다 (커밋 b09e736). shim이 없으면 조용히 지나간다
      (choco 2.7.5 헬퍼 소스로 확인).
    - CI가 설치 스크립트에 버전·체크섬을 넣을 때 UTF-8 BOM을 유지한다 (`-Encoding utf8BOM`, 커밋 b09e736). 0.3.0까지 게시본은 BOM이 없었다.
    - 한계: `Run` 값은 `choco`를 실행한 계정의 HKCU만 본다. 다른 계정으로 승격해 설치·제거하면 원래 사용자의 값은 그대로다.
    - 2026-11-17 사전 검증(설치 없이): Windows PowerShell 5.1 구문 오류 0, `Run` 값 판정 모의 시험 40건(판 2 × 설치/제거 × 10가지 값) 통과,
      `choco pack` 두 패키지 성공·nupkg에 uninstall 스크립트 포함.
    - 2026-11-17 **실제 업그레이드 시험** (`nshiftspace-led` 0.3.0 → 로컬에서 만든 0.3.1 nupkg, `choco upgrade --source <폴더>`, choco 2.7.5) — 통과:
      - 종료 코드 0 (`--force` 재실행도 0), 체크섬 일치.
      - `chocolatey\bin`의 shim이 없어졌다. 로그의 "Removing shim …"은 설치 스크립트의 `Uninstall-BinFile`이 낸 것으로,
        **Chocolatey는 업그레이드 때 옛 shim을 미리 지우지 않는다** — 직접 제거가 실제로 필요했다.
      - tools 폴더: exe + `.ignore`, `.gui` 없음, uninstall 스크립트 있음. 공용 시작 메뉴에 `nShiftSpace LED.lnk`(대상 = tools 폴더 exe).
      - `Run` 값: tools 경로였을 때는 그대로, 옛 shim 경로로 되돌려 놓고 `--force`로 다시 올리자 tools 경로로 이전됐다.
      - beforemodify가 실행 중 프로세스를 종료했다. 시작 메뉴 바로가기로 다시 실행했을 때 프로세스 경로 = tools 폴더 exe,
        `conhost.exe` 생성 0건. 콘솔 창이 보이지 않는지의 육안 확인과 트레이·단축키 동작은 **미확인**.
      - 경고: "Unable to move directory '…\lib\nshiftspace-led': (5) 액세스가 거부되었습니다: […\lib-bkp\nshiftspace-led\<버전>]" →
        "Retrying by moving individual files" 뒤 정상 진행. 두 번 모두 났고 원인은 미확인, 결과에는 영향이 없었다.
      - 일반판(`nshiftspace`)은 그 PC에 설치돼 있지 않아 모의 시험과 pack으로만 확인했다. 제거 스크립트는 실물로 실행하지 않았다.
- **시작 메뉴 바로가기**:

  | 채널 | 누가 만드나 | 위치 | 제거할 때 |
  |---|---|---|---|
  | Chocolatey (0.3.1부터) | 설치 스크립트 | 공용 `%ProgramData%\Microsoft\Windows\Start Menu\Programs\nShiftSpace.lnk` (LED판 `nShiftSpace LED.lnk`) | uninstall 스크립트가 지운다 |
  | winget | 패키지는 만들지 않는다 (포터블). **0.3.2부터** 트레이 메뉴 "시작 메뉴에 바로가기"로 사용자가 만든다 | 사용자 `%APPDATA%\Microsoft\Windows\Start Menu\Programs\` (같은 이름) | **남는다** — `winget uninstall` 전에 메뉴에서 끈다 |
  | zip 직접 | winget과 같음 | 같음 | exe를 지우기 전에 메뉴에서 끈다 |

  0.3.2부터 프로그램은 공용 쪽에 같은 이름의 바로가기가 있으면(Chocolatey 설치본) 메뉴 항목을 체크된 채 비활성으로 보여 준다.
  메뉴로 만든 바로가기는 실행 중인 exe 경로(`GetModuleFileName`)를 가리킨다 — winget 별칭으로 실행했을 때 어느 경로가 되는지는 미확인.
  알려진 동작: 바로가기를 한 번 만들면 ole32 등이 프로세스에 남아 커밋 메모리가 약 0.6MB 늘어난다. 작업 관리자 표시 수치는 그대로이고
  프로그램을 다시 실행하면 없어진다 (그대로 두기로 결정, 2026-11-18).
- **시작 프로그램 등록**: 두 채널 모두 자동 등록하지 않는다. 사용자가 프로그램 트레이 메뉴
  "로그인 시 자동 실행"으로 켤 수 있다 (HKCU `Run` 값, v0.2.0부터). 수동 절차는 아래
  ["설치 후 자동 실행 설정"](#설치-후-자동-실행-설정) 절 참고.

### 문제 해결

| 증상 | 원인·조치 |
|---|---|
| `winget install` 시 패키지를 찾지 못함 | 소스 캐시가 오래됨 — `winget source update` 후 재시도 |
| `choco install`이 "package not found" | 승인 직후 피드 반영 지연 가능 — 잠시 후 재시도. `choco search nshiftspace`로 노출 확인 |
| 업그레이드/제거가 파일 잠김으로 실패 | 트레이 아이콘 우클릭 → 종료 후 재시도 (winget 경로에서 특히) |
| 실행해도 Shift+Space가 안 먹음 | 관리자 권한 창이 포커스를 가진 경우 일반 권한 프로세스의 단축키는 동작하지 않음 (알려진 제약) |
| 중복 실행된 것 같음 | 뮤텍스로 차단되어 두 번째 인스턴스는 즉시 종료된다. 트레이 아이콘이 사라졌다면 explorer 재시작 후 자동 복구됨 |

---

## 설치 후 자동 실행 설정

nShiftSpace는 상주 프로그램이므로 **로그인할 때 자동으로 떠 있어야** 제 역할을 한다.
두 채널 모두 시작 프로그램에 **자동 등록하지 않으므로** 아래 절차를 한 번 수행한다.

> 💡 v0.2.0부터는 트레이 메뉴 **"로그인 시 자동 실행"** 체크 한 번으로 아래 방법 B와 같은 `Run` 값을
> 프로그램이 직접 쓰고 지운다. winget 사용자는 `nshiftspace` 별칭으로 실행한 상태에서 켜야 한다.
> 아래 수동 절차는 그 외 경우와 관리자 권한 대응(방법 C)을 위해 유지한다.

> ℹ️ 이 절의 명령은 Windows PowerShell 기준으로 작성했다. exe 경로 규칙과 권한 조건은
> 각 패키지 관리자의 공식 동작을 근거로 정리한 것이며, 사용자의 Windows 환경에서
> 1단계 `Test-Path` 확인부터 차례로 실행하면 자기 PC의 실제 경로를 확정할 수 있다.

### 왜 winget 쪽이 까다로운가 — 경로 안정성 차이

| 채널 | 시작 프로그램이 가리킬 경로 | 업그레이드 후에도 유지되나 |
|---|---|---|
| Chocolatey | `%ChocolateyInstall%\lib\nshiftspace\tools\nShiftSpace-x64.exe` | ✅ 유지 (경로에 버전이 들어가지 않음) |
| winget (별칭) | `%LOCALAPPDATA%\Microsoft\WinGet\Links\nshiftspace.exe` | ✅ 유지 — **winget에서는 이 경로를 써야 한다** |
| winget (실제 exe) | `%LOCALAPPDATA%\Microsoft\WinGet\Packages\SosomLab.nShiftSpace_Microsoft.Winget.Source_8wekyb3d8bbwe\nShiftSpace-x64.exe` | ⚠️ 패키지 폴더가 재생성되며 끊길 수 있음 |

winget 자동 실행 설정이 "쉽지 않은" 이유가 여기 있다. 탐색기로 설치 폴더를 찾아 들어가면
버전·소스 ID가 섞인 **Packages 폴더의 실제 exe**를 집게 되는데, 이 경로로 바로가기를 만들면
`winget upgrade` 후 바로가기가 끊긴다. **`Links` 폴더의 별칭(`nshiftspace.exe`)을 가리켜야**
업그레이드에도 살아남는다.

---

### 1단계 — 가리킬 exe 경로 확정 (winget)

일반 PowerShell에서:

```powershell
$exe = "$env:LOCALAPPDATA\Microsoft\WinGet\Links\nshiftspace.exe"
Test-Path $exe
```

- **`True`** → 이 `$exe`를 그대로 2단계에서 쓴다. (권장 경로)
- **`False`** → 별칭이 만들어지지 않은 것이다. 원인과 대응은 아래.

#### 별칭이 없을 때 (`False`인 경우)

winget이 포터블 패키지의 별칭을 만들 때는 **심볼릭 링크**를 생성하므로,
Windows에서 **개발자 모드가 꺼져 있으면** 별칭 생성이 실패하고 설치 로그에 경고가 남는다.
둘 중 하나로 해결한다.

**(a) 개발자 모드를 켜고 재설치 — 권장**

```
설정 → 개인 정보 및 보안 → 개발자용 → "개발자 모드" 켬
```

켠 뒤 재설치하면 별칭이 생성된다:

```powershell
winget uninstall SosomLab.nShiftSpace
winget install  SosomLab.nShiftSpace
Test-Path "$env:LOCALAPPDATA\Microsoft\WinGet\Links\nshiftspace.exe"
```

**(b) 개발자 모드를 켤 수 없는 환경 — 실제 exe 경로를 직접 찾는다**

```powershell
$exe = (Get-ChildItem "$env:LOCALAPPDATA\Microsoft\WinGet\Packages\SosomLab.nShiftSpace_*" `
        -Recurse -Filter "nShiftSpace-*.exe" |
        Select-Object -First 1).FullName
$exe    # 경로 출력 확인
```

> 이 방법을 쓰면 **`winget upgrade` 때마다 시작 프로그램을 다시 등록해야 할 수 있다.**
> 경로 고정이 중요하다면 Chocolatey로 설치하거나, GitHub Releases의 zip을 원하는 폴더에
> 풀어 쓰는 편이 확실하다.

#### Chocolatey로 설치한 경우의 1단계

관리자 권한이 아니어도 경로 확인은 된다:

```powershell
$exe = "$env:ChocolateyInstall\lib\nshiftspace\tools\nShiftSpace-x64.exe"   # 32비트 OS면 -x86
Test-Path $exe
```

`$env:ChocolateyInstall`이 비어 있으면 기본값 `C:\ProgramData\chocolatey`를 쓴다.

---

### 2단계 — 자동 실행 등록 (A·B·C 중 하나만)

#### 방법 A — 시작 프로그램 폴더에 바로가기 (가장 표준적)

`$exe`가 1단계에서 잡혀 있는 **같은 PowerShell 창**에서 이어서 실행한다:

```powershell
$startup  = [Environment]::GetFolderPath('Startup')
$lnk      = Join-Path $startup 'nShiftSpace.lnk'

$ws = New-Object -ComObject WScript.Shell
$sc = $ws.CreateShortcut($lnk)
$sc.TargetPath       = $exe
$sc.WorkingDirectory = Split-Path $exe
$sc.Description      = 'nShiftSpace - Shift+Space 한/영 전환'
$sc.Save()

Test-Path $lnk        # True 면 등록 완료
```

GUI로 하고 싶다면: `Win+R` → `shell:startup` → 열린 폴더에서 마우스 **오른쪽 버튼 드래그**로
`$exe`를 끌어다 놓고 **"여기에 바로 가기 만들기"** 선택. (그냥 끌면 복사/이동이 되어버린다.)

#### 방법 B — 레지스트리 Run 키 (한 줄, 바로가기 파일 없음)

```powershell
New-ItemProperty -Path 'HKCU:\Software\Microsoft\Windows\CurrentVersion\Run' `
  -Name 'nShiftSpace' -Value "`"$exe`"" -PropertyType String -Force
```

경로에 공백이 있을 수 있으므로 **큰따옴표로 감싸는 것**이 중요하다.

#### 방법 C — 작업 스케줄러 (관리자 권한 창에서도 단축키가 먹게 하려면 ★)

nShiftSpace의 알려진 제약 — **관리자 권한 창이 포커스를 가지면 일반 권한 프로세스의 단축키는
동작하지 않는다.** 이걸 피하려면 로그인 시 **높은 권한으로** 실행해야 하는데, 시작 프로그램
폴더나 Run 키로는 불가능하고 작업 스케줄러만 가능하다.

**관리자 권한 PowerShell**에서:

```powershell
$exe = "$env:LOCALAPPDATA\Microsoft\WinGet\Links\nshiftspace.exe"   # 1단계에서 확정한 경로

$action    = New-ScheduledTaskAction  -Execute $exe
$trigger   = New-ScheduledTaskTrigger -AtLogOn -User $env:USERNAME
$principal = New-ScheduledTaskPrincipal -UserId $env:USERNAME `
               -LogonType Interactive -RunLevel Highest
$settings  = New-ScheduledTaskSettingsSet -AllowStartIfOnBatteries `
               -DontStopIfGoingOnBatteries -ExecutionTimeLimit 0

Register-ScheduledTask -TaskName 'nShiftSpace' -Action $action -Trigger $trigger `
  -Principal $principal -Settings $settings -Force
```

- `-RunLevel Highest` 가 핵심 — 이게 있어야 관리자 창 위에서도 Shift+Space가 동작한다.
- `-ExecutionTimeLimit 0` = 시간 제한 없음. 상주 프로그램이므로 반드시 넣는다
  (기본값 3일이 지나면 작업이 종료된다).
- 이 방식은 **UAC 프롬프트 없이** 승격 실행되므로 로그인 때 창이 뜨지 않는다.
- 관리자 계정이 아니거나 조직 정책으로 막힌 PC에서는 실패할 수 있다 — 그럴 땐 방법 A를 쓴다.

---

### 3단계 — 확인

로그아웃 후 다시 로그인(또는 재부팅)해서 확인한다. 재부팅 없이 바로 확인하려면:

```powershell
Start-Process $exe
Get-Process nShiftSpace-x64, nshiftspace -ErrorAction SilentlyContinue   # 프로세스 확인
```

트레이에 '가·A' 아이콘이 보이고 Shift+Space로 한/영이 전환되면 정상이다.
중복 실행은 뮤텍스로 차단되므로 두 번 떠서 충돌할 걱정은 없다.

### 해제 방법

등록에 쓴 방법과 **같은 방법**으로 지운다.

```powershell
# A - 바로가기
Remove-Item (Join-Path ([Environment]::GetFolderPath('Startup')) 'nShiftSpace.lnk')

# B - Run 키
Remove-ItemProperty -Path 'HKCU:\Software\Microsoft\Windows\CurrentVersion\Run' -Name 'nShiftSpace'

# C - 예약 작업 (관리자 권한 PowerShell)
Unregister-ScheduledTask -TaskName 'nShiftSpace' -Confirm:$false
```

### 업그레이드할 때 주의

자동 실행을 걸어 두면 nShiftSpace가 항상 떠 있으므로, 업그레이드 시 **파일 잠김**이 발생한다.

- **Chocolatey**: `chocolateybeforemodify.ps1`이 알아서 종료시킨다 — 그대로 `choco upgrade`.
- **winget**: 종료 처리가 없다. **트레이 아이콘 우클릭 → 종료** 후 `winget upgrade`를 실행한다.

업그레이드 후 방법 A·B를 쓴 경우 1단계 `Test-Path`로 경로가 유효한지 한 번 확인한다
(`Links` 별칭 경로를 썼다면 바뀌지 않는다). 방법 C는 예약 작업이 그대로 유지된다.

---

## Chocolatey

### 등록 상태 확인 방법

- **패키지 페이지**: https://community.chocolatey.org/packages/nshiftspace
  - "This package is under moderation review" 배너가 사라지고 버전에 *Approved* 가 표시되면 완료.
  - Version History 표에서 각 버전의 상태(Submitted → under review → Approved) 확인 가능.
- **계정 알림**: 심사 코멘트/수정 요청은 chocolatey.org 계정 메일로 통지됨.
- 모더레이션 단계: 자동 품질 검사(package-validator) → 설치 테스트(package-verifier) → 사람 심사.
  최초 등록은 통상 수일 소요.

### 구성 요소

- [packaging/chocolatey/nshiftspace.nuspec](../packaging/chocolatey/nshiftspace.nuspec) — 패키지 메타데이터 (`__VERSION__` 템플릿)
- [packaging/chocolatey/tools/chocolateyinstall.ps1](../packaging/chocolatey/tools/chocolateyinstall.ps1) — 릴리스 zip 다운로드·설치, shim 제외(`.ignore`) 마커·시작 메뉴 바로가기·옛 shim 정리 (0.3.1부터. 0.3.0까지는 `.gui` 마커)
- [packaging/chocolatey/tools/chocolateybeforemodify.ps1](../packaging/chocolatey/tools/chocolateybeforemodify.ps1) — 업그레이드/제거 전 프로세스 종료
- CI `chocolatey` 잡 ([.github/workflows/build.yml](../.github/workflows/build.yml)) — `v*` 태그 시
  체크섬 주입 → `choco pack` → `choco push` (시크릿 `CHOCO_API_KEY`, 등록 완료).
  **v0.1.0에서 이미 전 스텝 성공 — 자동 게시 경로 검증 완료.** v0.2.0에서도 `choco push`까지 자동 성공.
  단 게시(push)가 자동일 뿐 **승인은 별개**로, 새 버전도 모더레이션 큐를 다시 거친다
  (0.1.0은 제출→승인에 약 27일 소요).

### 시작 프로그램 바로가기 자동 생성 — 가능하나 미적용 (검토 결과)

**결론: 기술적으로 가능하다.** Chocolatey 헬퍼 `Install-ChocolateyShortcut`으로
`chocolateyinstall.ps1`에서 사용자 시작 프로그램 폴더(`shell:startup`)에 `.lnk`를 만들 수 있다.
winget에는 대응 수단이 없는, **Chocolatey만의 장점**이다.

```powershell
# chocolateyinstall.ps1 에 추가할 형태 (opt-in 예시)
$pp = Get-PackageParameters
if ($pp.Startup) {
  Install-ChocolateyShortcut `
    -ShortcutFilePath (Join-Path ([Environment]::GetFolderPath('Startup')) 'nShiftSpace.lnk') `
    -TargetPath       (Join-Path $toolsDir 'nShiftSpace-x64.exe') `
    -WorkingDirectory $toolsDir `
    -Description      'nShiftSpace - Shift+Space 한/영 전환'
}
```

경로 측면의 궁합도 좋다 — choco의 `tools` 경로에는 버전이 들어가지 않아
업그레이드해도 바로가기가 끊기지 않는다.

다만 **바로 적용하지 않은 이유**가 있다.

| 고려사항 | 내용 |
|---|---|
| 설치 컨텍스트 | `choco install`은 **관리자 권한**으로 실행된다. 같은 사용자가 UAC 승격한 것이라면 `Startup`이 그 사용자 폴더로 잡히지만, **SYSTEM 계정이나 다른 사용자로 실행**되면(배포 도구·CM 연동 등) 엉뚱한 프로필에 바로가기가 생긴다 |
| 기본값 | 사용자가 요청하지 않은 자동 실행 등록은 침습적이다. `--params "/Startup"` **opt-in**으로 두는 것이 Chocolatey 관례에 맞다 |
| 제거 처리 | `chocolateyuninstall.ps1`을 새로 만들어 바로가기를 지워야 한다 (0.3.0까지는 uninstall 스크립트가 없음. 0.3.1에 신설되어 시작 메뉴 바로가기와 `Run` 값을 정리한다) |
| 배포 비용 | 패키지 스크립트 변경 = **새 버전 배포 + 모더레이션 재심사**. 0.1.0 실적 기준 승인까지 약 27일 |
| 채널 간 불일치 | winget에는 같은 기능을 넣을 수 없어 두 채널의 설치 후 상태가 달라진다 |

→ 적용한다면 **v0.1.1에 opt-in 방식**(`choco install nshiftspace --params "/Startup"`)으로
uninstall 스크립트와 함께 넣는 것이 타당하다. [ROADMAP.md](ROADMAP.md)에 후보 과제로 올려 두었다.

### 진행 이력

| 일시 | 내용 |
|---|---|
| 2026-07-04 00:43 | 패키지 명세·CI 자동 게시 파이프라인 구성, pack 단계 검증 |
| 2026-07-04 00:52 | `CHOCO_API_KEY` 시크릿 등록 (사용자) |
| 2026-07-04 00:53 | v0.1.0 `choco push` 성공 — nshiftspace 0.1.0 모더레이션 큐 진입 |
| 2026-07-05 01:38 | 상태 점검: `choco search nshiftspace` 미노출 — 여전히 모더레이션 심사 중 |
| 2026-07-30 21:01 | **모더레이션 승인 완료** — 커뮤니티 피드 API 확인 (`IsApproved=true`, `PackageStatus=Approved`, 누적 다운로드 10) |
| 2026-07-30 22:40 | 재점검: 패키지 페이지에 "approved by moderator flcdrg on 30 Jul 2026" 표시, 검증기 결과 `PackageTestResultStatus=Passing`·`PackageValidationResultStatus=Passing`, 커뮤니티 검색 피드 노출 확인. CI 자동 게시 잡은 v0.1.0 런(28671030609)에서 `choco push`까지 성공했음을 확인 |
| 2026-08-02 | 상태 재확인 — 변동 없음. 피드 API `IsApproved=true`·`PackageStatus=Approved`·`IsLatestVersion=true`, 검증/테스트 모두 `Passing` 유지, 게시 버전은 0.1.0 단일, 누적 다운로드 11(7/30과 동일) |
| 2026-10-17 22:24 | **v0.2.0 `choco push` 성공** (CI 런 39036391096 `chocolatey` 잡) — nuspec 설명을 5KB·자동 실행 메뉴·LED판 안내로 갱신한 버전. 피드 API `Version=0.2.0`, `PackageStatus=Submitted`, `PackageSubmittedStatus=Pending`, 검증/테스트 `Pending` — 모더레이션 큐 진입 |
| 2026-10-17 23:16 | **v0.3.0 `choco push` 성공 — `nshiftspace` 0.3.0과 신규 `nshiftspace-led` 0.3.0** (CI 런 39039940540, `chocolatey`·`chocolatey-led` 잡). 두 패키지 모두 모더레이션 큐 진입. 0.2.0과 0.3.0이 동시에 큐에 있으므로 승인 순서는 모더레이터에 달림 |
| 2026-10-21 00:53 | **0.2.0·0.3.0·`nshiftspace-led` 0.3.0 승인** — 피드 API `PackageApprovedDate` 기준 `nshiftspace` 0.2.0 00:53:42, 0.3.0 00:53:49, `nshiftspace-led` 0.3.0 00:54:19. 제출 약 3일 뒤 (2026-11-17 조회로 확인) |
| 2026-11-17 | 상태 재확인 — `nshiftspace` 0.1.0·0.2.0·0.3.0, `nshiftspace-led` 0.3.0 모두 `IsApproved=true`·`PackageStatus=Approved`. 같은 날 shim 자동 실행 시 콘솔 창 문제 원인 확정(위 "채널별 동작 차이") |
| 2026-11-17 19:37 | **v0.3.1 `choco push` 성공 — `nshiftspace` 0.3.1 · `nshiftspace-led` 0.3.1** (CI 런 44066308486, `chocolatey`·`chocolatey-led` 잡). 게시된 nupkg 확인: 설치 스크립트 UTF-8 BOM 있음(처음으로), 자리표시자 없음, 체크섬이 릴리스 zip의 SHA256과 일치, uninstall 스크립트 포함. 피드 API 둘 다 `PackageStatus=Submitted`, `PackageSubmittedStatus=Pending`, 검증·테스트 `Pending` (19:40 조회) |
| 2026-11-17 23:08 | 0.3.1 상태 조회 — 두 패키지 모두 자동 검증 `PackageValidationResultStatus=Passing`(20:15), 설치 테스트 `PackageTestResultStatus=Passing`(`nshiftspace` 20:29, `nshiftspace-led` 20:31), `PackageScanStatus=Pending`. `PackageStatus=Submitted`·`IsApproved=false` — 모더레이터 심사 대기. 승인 전이라 `choco install`은 0.3.0을 받는다 |
| 2026-11-18 00:18 | **v0.3.2 태그 런(44112548056) — guard가 두 패키지 모두 보류**: "직전 버전(0.3.1)이 검수 대기 중 — 0.3.2 은(는) 보류한다". `chocolatey`·`chocolatey-led` 잡은 pack까지 하고 push 단계를 건너뜀 (nupkg는 아티팩트). 피드에 0.3.2 없음. 00:20 조회에서 0.3.1은 여전히 `Submitted` |

---

## winget

### 등록 상태 확인 방법

- **등록 PR**: https://github.com/microsoft/winget-pkgs/pull/397365
  - `wingetbot`이 단 Validation Pipeline 링크에서 자동 검증(매니페스트 검사 + 설치 테스트) 진행 상황 확인.
  - 라벨 의미: `New-Package`(신규 패키지), `Validation-Completed`(검증 통과, 병합 대기),
    `Needs-Attention`/`Needs-Author-Feedback`(수정 필요 — 대응 필요).
  - PR이 **병합되면 등록 완료**. 수 시간 내 `winget search nshiftspace`로 확인 가능.
- CLA: 체크 통과 (license/cla SUCCESS).

### 구성 요소

- [packaging/winget/manifests/s/SosomLab/nShiftSpace/](../packaging/winget/manifests/s/SosomLab/nShiftSpace/) —
  `TEMPLATE.*.yaml`(CI 렌더링 원본) + 버전별(0.1.0, 0.2.0, 0.3.0, 0.3.1, 0.3.2) 매니페스트 3종 사본 (스키마 1.6). 0.3.1·0.3.2 사본은 CI 렌더 결과 그대로다 (커밋 a7a8faf · f990a3b).
  LED판은 [nShiftSpace/LED/](../packaging/winget/manifests/s/SosomLab/nShiftSpace/LED/) (템플릿 + 0.3.0) —
  **식별자의 점은 폴더 구분**이라 `SosomLab.nShiftSpace.LED`의 경로는 `manifests/s/SosomLab/nShiftSpace/LED/<버전>`이다.
  - zip 안의 포터블 exe: `InstallerType: zip` + `NestedInstallerType: portable`, 별칭 `nshiftspace`
- CI `winget` 잡 — `v*` 태그 시 업데이트 PR 자동 제출 (시크릿 `WINGET_TOKEN`, 등록 완료). 지금 방식은 템플릿 렌더링 +
  `wingetcreate submit`이다. 처음 방식인 `wingetcreate update`는 **v0.2.0·v0.3.0에서 두 번 모두 실패** — x64 zip URL을
  기존 매니페스트의 x86 설치 항목에 짝지어 `Nested installer not found in zip archive: nShiftSpace-x86.exe`.
  URL 뒤 `|x64`/`|x86` 지정도 소용없었다 (zip+중첩 포터블 조합의 한계). **v0.3.0 이후 CI는 `update`를 버리고**
  저장소의 `TEMPLATE.*.yaml`에 버전·SHA256·날짜를 주입해 매니페스트를 만든 뒤 `wingetcreate submit <폴더>`로 PR을
  만든다 (일반판·LED판 모두, 렌더링 결과는 `winget-manifests` 아티팩트). **v0.3.1 태그에서 일반판 제출이 성공했다** (LED판 제출 경로는 아직 미검증).
  0.2.0·0.3.0은 같은 템플릿 방식으로 로컬에서 만들어 포크에서 수동 PR로 제출했다.
- 제출용 포크: https://github.com/kiros33/winget-pkgs (브랜치 `sosomlab-nshiftspace-0.1.0`, `SosomLab.nShiftSpace-0.2.0`,
  `SosomLab.nShiftSpace-0.3.0`, `SosomLab.nShiftSpace.LED-0.3.0`)

### 진행 이력

| 일시 | 내용 |
|---|---|
| 2026-07-04 00:57 | 매니페스트 3종 작성 (v0.1.0 zip SHA256 고정), 포크에 브랜치 생성 |
| 2026-07-04 00:58 | microsoft/winget-pkgs#397365 PR 제출 — 검증 파이프라인 실행 중 |
| 2026-07-04 01:04 | `WINGET_TOKEN` 시크릿 등록 (사용자) — 이후 버전 자동 제출 준비 완료 |
| 2026-07-04 01:22 | 상태 점검: CLA 체크 통과(Needs-CLA 라벨은 갱신 지연, 무해), `New-Package` 분류, Azure 검증 파이프라인 진행 중, 모더레이터 승인 대기 |
| 2026-07-05 01:38 | 상태 점검: PR open·미병합, 라벨 `Azure-Pipeline-Passed`+`Validation-Completed` (검증 통과) — 모더레이터 병합 대기 |
| 2026-07-17 05:23 | **PR #397365 병합 — 등록 완료.** 라벨 `Moderator-Approved`, `Publish-Pipeline-Succeeded` 추가. winget-pkgs master에 매니페스트 반영 확인 |
| 2026-08-02 | 상태 재확인 — 변동 없음. PR #397365 `merged=true`(2026-07-16T20:23:39Z UTC), winget-pkgs master의 `manifests/s/SosomLab/nShiftSpace/0.1.0/`에 매니페스트 3종 존재. 등록된 버전 디렉터리는 `0.1.0` 하나뿐이며, 게시 매니페스트에서 별칭 `PortableCommandAlias: nshiftspace`·`NestedInstallerType: portable` 재확인 |
| 2026-10-17 22:24 | v0.2.0 태그 CI `winget` 잡 **실패** — `wingetcreate` 아키텍처 짝짓기 오류(위 구성 요소 설명). 워크플로에 `\|x64`/`\|x86` 지정 추가 |
| 2026-10-17 22:35 | **0.2.0 수동 PR 제출** — 저장소 사본을 0.2.0으로 갱신(zip SHA256 2종·ReleaseDate·설명·ReleaseNotesUrl), `winget validate` 통과, 포크 브랜치 `SosomLab.nShiftSpace-0.2.0`에서 [microsoft/winget-pkgs#454746](https://github.com/microsoft/winget-pkgs/pull/454746) 제출. 검증 파이프라인·모더레이터 승인 대기 |
| 2026-10-17 23:16 | v0.3.0 태그 CI `winget` 잡 — `|x64` 지정에도 **동일 오류로 재실패**. CI를 템플릿 렌더링 + `wingetcreate submit` 방식으로 교체 |
| 2026-10-17 23:25 | 0.2.0 PR #454746 라벨 `Azure-Pipeline-Passed`·`Validation-Completed`·`New-Manifest` — 검증 통과, 모더레이터 승인 대기 |
| 2026-10-17 23:28 | **0.3.0 수동 PR 2건 제출** — 일반판 [microsoft/winget-pkgs#454771](https://github.com/microsoft/winget-pkgs/pull/454771) (브랜치 `SosomLab.nShiftSpace-0.3.0`), **LED판 신규 패키지** [microsoft/winget-pkgs#454772](https://github.com/microsoft/winget-pkgs/pull/454772) (`SosomLab.nShiftSpace.LED`, 별칭 `nshiftspace-led`, 브랜치 `SosomLab.nShiftSpace.LED-0.3.0`). 둘 다 로컬 `winget validate` 통과 |
| 2026-10-17 23:20 | LED PR #454772 `Manifest-Validation-Error` — 봇 진단: 경로가 `nShiftSpace.LED\\0.3.0`인데 기대 경로는 `nShiftSpace\\LED\\0.3.0` (식별자의 점 = 폴더). 로컬 `winget validate`는 폴더 규칙을 보지 않아 잡지 못했다 |
| 2026-10-18 00:05 | 포크 브랜치에서 3개 파일을 `manifests/s/SosomLab/nShiftSpace/LED/0.3.0/`으로 옮김(삭제+추가) → PR 재검증. 저장소 템플릿 폴더도 `nShiftSpace/LED/`로 이동, CI 렌더·guard 경로를 "점 → /" 규칙으로 수정, 렌더 결과는 `rendered/<id>/<버전>/`에 모아 아티팩트로 |
| 2026-10-20 01:18 | **0.2.0 PR #454746 병합**, 01:22 **0.3.0 PR #454771 병합** — #454771 라벨 `Moderator-Approved`·`Publish-Pipeline-Succeeded` (2026-11-17 조회로 확인) |
| 2026-11-17 | 상태 재확인 — winget-pkgs master의 `manifests/s/SosomLab/nShiftSpace/`에 `0.1.0`·`0.2.0`·`0.3.0`, `winget show SosomLab.nShiftSpace` 버전 0.3.0. **LED판 PR #454772는 열려 있음** — 라벨 `Azure-Pipeline-Passed`·`Validation-Completed`·`New-Package`, 리뷰 필요 상태. 11-17 05:27 모더레이터가 `@wingetbot run`으로 검증을 다시 돌렸고 06:48 봇이 "모더레이터 승인 필요" 안내. 작성자 조치 요구 없음. `winget show SosomLab.nShiftSpace.LED`는 아직 찾지 못함 |
| 2026-11-17 19:37 | **v0.3.1 태그 CI `winget` 잡 성공 — 새 방식(템플릿 렌더링 + `wingetcreate submit`) 첫 실전.** `Manifest validation succeeded: True`, 일반판 PR [microsoft/winget-pkgs#482170](https://github.com/microsoft/winget-pkgs/pull/482170) 자동 생성 (제목 `New version: SosomLab.nShiftSpace version 0.3.1`). 렌더된 매니페스트의 `InstallerSha256` 2개가 릴리스 zip과 일치, `ReleaseDate: 2026-11-17`. LED판은 guard 보류 판정으로 제출하지 않았고 매니페스트만 아티팩트(`winget-manifests`)로 남겼다. 19:40 조회: #482170 열림, 라벨 아직 없음 |
| 2026-11-17 20:49 | **0.3.1 PR #482170 병합** — 제출 약 1시간 10분 뒤. 라벨 `Azure-Pipeline-Passed`·`Validation-Completed`·`Moderator-Approved`·`Publish-Pipeline-Succeeded`·`New-Manifest`. 23:08 조회에서 `winget show SosomLab.nShiftSpace` 버전 0.3.1. 저장소 사본은 커밋 a7a8faf. LED판 #454772는 그대로 열림 (라벨 변화 없음, 마지막 갱신 11-17 06:48) |
| 2026-11-18 00:18 | **v0.3.2 태그 CI `winget` 잡 성공** — 일반판 PR [#482402](https://github.com/microsoft/winget-pkgs/pull/482402) 자동 생성 (`New version: SosomLab.nShiftSpace version 0.3.2`), `Manifest validation succeeded: True`, `InstallerSha256` 2개가 릴리스 zip과 일치. `ReleaseDate: 2026-11-17`(CI는 UTC 날짜). LED판은 guard 보류로 제출하지 않음 (#454772 열림). 00:20 조회: #482402 열림, 라벨 없음 |
| 2026-11-18 00:28 | 상태 조회 — #482402 열림, 라벨 `New-Manifest`·리뷰 필요 (검증 결과 라벨은 아직 없음). 저장소 사본은 커밋 f990a3b. #454772는 그대로 열림 (마지막 갱신 11-17 06:48), `winget show SosomLab.nShiftSpace` 0.3.1 |

---

## 운영 규칙

- **게시된 태그는 재발행 금지.** winget 매니페스트와 Chocolatey 설치 스크립트에 릴리스 zip의
  SHA256이 고정되므로, 태그를 다시 만들면 체크섬 불일치로 설치가 깨진다.
  수정이 필요하면 반드시 새 버전(v0.1.1 …)으로 배포한다.
- 새 버전 배포는 `git tag v0.x.y && git push origin v0.x.y` 한 번으로
  GitHub Release → Chocolatey push → winget PR까지 자동 진행된다.
- 심사 중 수정 요청(양쪽 모두 가능)이 오면 패키지 스크립트/매니페스트를 고쳐 재제출한다.
