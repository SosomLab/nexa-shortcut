# 릴리스와 배포 (메인테이너용)

## 한 줄 요약

```bash
git tag v0.1.1 && git push origin v0.1.1
```

이 한 번으로 **GitHub Release → Chocolatey 게시 → winget 업데이트 PR**까지 자동 진행됩니다.

## 자동화 흐름

```
git push origin v0.1.1
   │
   ├─ build    빌드 + 8KB 크기 회귀 검사 → Release에 zip 2종 첨부
   ├─ chocolatey   체크섬 주입 → choco pack → choco push  (시크릿 CHOCO_API_KEY)
   └─ winget       wingetcreate update → winget-pkgs PR 자동 제출 (시크릿 WINGET_TOKEN)
```

정의: [`.github/workflows/build.yml`](https://github.com/SosomLab/nexa-shortcut/blob/main/.github/workflows/build.yml)

## 자동인 것과 자동이 아닌 것

| 단계 | 자동 여부 |
|---|---|
| GitHub Release 생성·자산 첨부 | ✅ 자동 |
| Chocolatey **게시**(`choco push`) | ✅ 자동 (v0.1.0에서 검증됨) |
| Chocolatey **승인**(모더레이션) | ❌ 별개 — 새 버전마다 큐를 다시 거침 (0.1.0 실적 약 27일) |
| winget PR **제출** | ✅ 자동 — 단, **아직 한 번도 실행된 적 없음** (0.1.0은 수동 PR이었음) |
| winget PR **병합** | ❌ 별개 — 검증 파이프라인 통과 후 모더레이터 승인 |

> ⚠️ v0.1.1 배포 때 winget 자동 제출이 실제로 동작하는지 **처음 확인**하게 됩니다.

## ⛔ 운영 규칙 — 게시된 태그는 재발행 금지

winget 매니페스트와 Chocolatey 설치 스크립트에 릴리스 zip의 **SHA256이 고정**됩니다.
태그를 지웠다 다시 만들면 zip이 새로 빌드되어 체크섬이 달라지고, **이미 배포된 패키지의
설치가 깨집니다.**

수정이 필요하면 반드시 **새 버전**(v0.1.1, v0.1.2 …)으로 배포하세요.

## 상태 확인

| 채널 | 확인처 |
|---|---|
| Chocolatey | https://community.chocolatey.org/packages/nshiftspace |
| winget | [winget-pkgs의 매니페스트 경로](https://github.com/microsoft/winget-pkgs/tree/master/manifests/s/SosomLab/nShiftSpace) |
| CI | [Actions 탭](https://github.com/SosomLab/nexa-shortcut/actions) |

명령줄로:

```bash
# Chocolatey 승인 상태
curl -s "https://community.chocolatey.org/api/v2/Packages()?\$filter=Id%20eq%20'nshiftspace'" \
  | tr '>' '>\n' | grep -E "d:(Version|IsApproved|PackageStatus)"

# winget 등록 버전
gh api repos/microsoft/winget-pkgs/contents/manifests/s/SosomLab/nShiftSpace --jq '.[].name'
```

## 패키지 명세 위치

| 파일 | 역할 |
|---|---|
| [`packaging/chocolatey/nshiftspace.nuspec`](https://github.com/SosomLab/nexa-shortcut/blob/main/packaging/chocolatey/nshiftspace.nuspec) | 패키지 메타데이터 (`__VERSION__` 템플릿) |
| [`packaging/chocolatey/tools/chocolateyinstall.ps1`](https://github.com/SosomLab/nexa-shortcut/blob/main/packaging/chocolatey/tools/chocolateyinstall.ps1) | zip 다운로드·설치, GUI shim 마커 |
| [`packaging/chocolatey/tools/chocolateybeforemodify.ps1`](https://github.com/SosomLab/nexa-shortcut/blob/main/packaging/chocolatey/tools/chocolateybeforemodify.ps1) | 업그레이드/제거 전 프로세스 종료 |
| [`packaging/winget/manifests/`](https://github.com/SosomLab/nexa-shortcut/tree/main/packaging/winget/manifests) | winget 매니페스트 3종 사본 (스키마 1.6) |

템플릿 플레이스홀더(`__VERSION__`, `__CHECKSUM32__`, `__CHECKSUM64__`)는 저장소 파일에
**그대로 보존**되며 치환은 CI 러너에서만 일어납니다 — 반복 배포에 안전합니다.

---

전체 진행 이력과 심사 기록은
[docs/PACKAGING.md](https://github.com/SosomLab/nexa-shortcut/blob/main/docs/PACKAGING.md) 참고.
