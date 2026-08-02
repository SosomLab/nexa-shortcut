#!/usr/bin/env bash
# docs/wiki/ 의 내용을 GitHub Wiki 저장소에 발행한다.
#
# 사용법:  ./tools/publish-wiki.sh
#
# 사전 조건 (최초 1회):
#   GitHub은 Wiki 첫 페이지 생성 API를 제공하지 않는다. 웹 UI에서 아무 페이지나 한 번
#   저장해 wiki 저장소를 초기화해야 이 스크립트가 동작한다.
#     https://github.com/SosomLab/nexa-shortcut/wiki/_new
#   (제목 Home, 내용 아무거나 → Save Page. 어차피 이 스크립트가 덮어쓴다.)
set -euo pipefail

REPO="SosomLab/nexa-shortcut"
SRC="$(cd "$(dirname "${BASH_SOURCE[0]}")/../docs/wiki" && pwd)"
WIKI_URL="https://github.com/${REPO}.wiki.git"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

echo "==> 원본: $SRC"

if ! git ls-remote "$WIKI_URL" >/dev/null 2>&1; then
  cat >&2 <<'EOF'
[!] Wiki 저장소가 아직 없습니다 (첫 페이지 미생성).

    아래 주소에서 페이지 하나를 만들어 저장한 뒤 다시 실행하세요:
      https://github.com/SosomLab/nexa-shortcut/wiki/_new

    저장소 설정에서 Wiki가 꺼져 있다면 먼저 켜야 합니다:
      Settings → General → Features → Wikis
EOF
  exit 1
fi

echo "==> Wiki 저장소 클론"
git clone --quiet "$WIKI_URL" "$WORK/wiki"

echo "==> 기존 페이지 제거 후 재생성"
find "$WORK/wiki" -maxdepth 1 -name '*.md' -delete
cp "$SRC"/*.md "$WORK/wiki/"

cd "$WORK/wiki"
if git diff --quiet && git diff --cached --quiet && [ -z "$(git status --porcelain)" ]; then
  echo "==> 변경 없음. 발행할 내용이 없습니다."
  exit 0
fi

git add -A
git status --short
git commit --quiet -m "docs: docs/wiki/ 내용으로 Wiki 갱신"
git push --quiet origin HEAD

echo "==> 완료: https://github.com/${REPO}/wiki"
