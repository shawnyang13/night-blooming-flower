#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TOOLS_DIR="$SCRIPT_DIR/tools/bin"
ARDUINO_CLI="$TOOLS_DIR/arduino-cli"
SOURCE_ARDUINO_CLI=""
ESP32_INDEX_URL="https://espressif.github.io/arduino-esp32/package_esp32_index.json"
INSTALL_CORE=1

usage() {
  cat <<EOF
Usage:
  bash burst/prepare.sh [options]

Prepare the local ESP32 flashing toolchain for this repository.

Options:
  -a, --arduino-cli <path>   Use or copy arduino-cli from the given path.
                             Default: burst/tools/bin/arduino-cli
  -u, --core-url <url>       ESP32 Arduino package index URL.
                             Default: $ESP32_INDEX_URL
  -S, --skip-core-install    Only prepare arduino-cli, skip ESP32 core install.
  -h, --help                 Show this help message.

Examples:
  bash burst/prepare.sh
  bash burst/prepare.sh -S
  bash burst/prepare.sh -a ~/.local/bin/arduino-cli
EOF
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    -a|--arduino-cli)
      [[ $# -ge 2 ]] || { echo "Missing value for $1"; exit 2; }
      SOURCE_ARDUINO_CLI="$2"
      shift 2
      ;;
    -u|--core-url)
      [[ $# -ge 2 ]] || { echo "Missing value for $1"; exit 2; }
      ESP32_INDEX_URL="$2"
      shift 2
      ;;
    -S|--skip-core-install)
      INSTALL_CORE=0
      shift
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      echo "Unknown option: $1"
      echo
      usage
      exit 2
      ;;
  esac
done

mkdir -p "$TOOLS_DIR"

if [[ -n "$SOURCE_ARDUINO_CLI" && -x "$SOURCE_ARDUINO_CLI" ]]; then
  echo "Copying arduino-cli from $SOURCE_ARDUINO_CLI ..."
  cp "$SOURCE_ARDUINO_CLI" "$ARDUINO_CLI"
  chmod +x "$ARDUINO_CLI"
elif [[ -n "$SOURCE_ARDUINO_CLI" ]]; then
  echo "arduino-cli source is not executable: $SOURCE_ARDUINO_CLI"
  exit 1
elif [[ ! -x "$ARDUINO_CLI" && -x "$HOME/.local/bin/arduino-cli" ]]; then
  echo "Copying existing arduino-cli from $HOME/.local/bin ..."
  cp "$HOME/.local/bin/arduino-cli" "$ARDUINO_CLI"
  chmod +x "$ARDUINO_CLI"
fi

if [[ ! -x "$ARDUINO_CLI" ]]; then
  echo "Installing arduino-cli into $TOOLS_DIR ..."
  curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | BINDIR="$TOOLS_DIR" sh
else
  echo "arduino-cli already exists: $ARDUINO_CLI"
fi

if [[ ! -f "$HOME/.arduino15/arduino-cli.yaml" ]]; then
  "$ARDUINO_CLI" config init
fi

if [[ "$INSTALL_CORE" -eq 0 ]]; then
  echo
  echo "Preparation complete. ESP32 core install was skipped."
  echo "Tool: $ARDUINO_CLI"
  exit 0
fi

echo "Updating Arduino indexes ..."
"$ARDUINO_CLI" core update-index --additional-urls "$ESP32_INDEX_URL"

echo "Installing ESP32 Arduino core ..."
"$ARDUINO_CLI" core install esp32:esp32 --additional-urls "$ESP32_INDEX_URL"

echo
echo "Preparation complete."
echo "Tool: $ARDUINO_CLI"
echo "Next: burst/flash.sh"
