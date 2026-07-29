#!/usr/bin/env bash
set -euo pipefail

SOURCE="${BASH_SOURCE[0]}"
while [ -L "$SOURCE" ]; do
  DIR="$(cd -P "$(dirname "$SOURCE")" && pwd)"
  SOURCE="$(readlink "$SOURCE")"
  [[ "$SOURCE" != /* ]] && SOURCE="$DIR/$SOURCE"
done
SCRIPT_DIR="$(cd -P "$(dirname "$SOURCE")" && pwd)"

# 仓库根目录：脚本在 <repo>/scripts/ 下时适用
ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

QT_VERSION="${QT_VERSION:-6.11.0}"
QT_ROOT="${QT_ROOT:-$HOME/Qt/$QT_VERSION/gcc_64}"

BUILD_DIR="${BUILD_DIR:-$ROOT/build_rel}"
BIN="${CLIENT_BIN:-$BUILD_DIR/bin/gen-tau}"

if [[ ! -x "$BIN" ]]; then
  echo "找不到可执行文件：$BIN"
  exit 1
fi

cd "$ROOT"
# mkdir -p logs

# 关键：Qt 没装到系统路径时，补齐运行时库/插件/QML 路径
# export LD_LIBRARY_PATH="$QT_ROOT/lib:$BUILD_DIR/lib:${LD_LIBRARY_PATH:-}"
# export QT_PLUGIN_PATH="$QT_ROOT/plugins"
# export QML_IMPORT_PATH="$QT_ROOT/qml${QML_IMPORT_PATH:+:$QML_IMPORT_PATH}"
# export QML2_IMPORT_PATH="$QT_ROOT/qml${QML2_IMPORT_PATH:+:$QML2_IMPORT_PATH}"

# 可选：需要时打开 Qt/QML 调试输出
# export QML_IMPORT_TRACE=1
# export QT_LOGGING_RULES="qt.qml.import=true"

# 可选：一键 gdb（用：GDB=1 ./你的脚本名）
export GST_DEBUG="${GST_DEBUG:-3}"

if [[ "${GDB:-0}" == "1" ]]; then
  exec gdb --args "$BIN" "$@"
else
  exec "$BIN" "$@"
fi