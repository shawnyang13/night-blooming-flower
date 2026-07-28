#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
ARDUINO_CLI="${ARDUINO_CLI:-$SCRIPT_DIR/tools/bin/arduino-cli}"
PORT="${PORT:-/dev/ttyUSB0}"
FQBN="${FQBN:-esp32:esp32:esp32}"
SKETCH_DIR="${SKETCH_DIR:-$ROOT_DIR/Firmware/flower_lamp_timer}"
DO_COMPILE=1
DO_UPLOAD=1

usage() {
  cat <<EOF
Usage:
  bash burst/flash.sh [options]

Compile and upload the ESP32 firmware.

Options:
  -p, --port <path>          Serial port for the ESP32.
                             Default: $PORT
  -f, --fqbn <fqbn>          Arduino board FQBN.
                             Default: $FQBN
  -s, --sketch <path>        Sketch directory.
                             Default: $SKETCH_DIR
  -a, --arduino-cli <path>   arduino-cli executable.
                             Default: $ARDUINO_CLI
  -c, --compile-only         Compile but do not upload.
  -u, --upload-only          Upload without compiling first.
  -h, --help                 Show this help message.

Examples:
  bash burst/flash.sh
  bash burst/flash.sh -p /dev/ttyUSB0
  bash burst/flash.sh -c
  bash burst/flash.sh -s Firmware/flower_lamp_timer -p /dev/ttyUSB1
EOF
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    -p|--port)
      [[ $# -ge 2 ]] || { echo "Missing value for $1"; exit 2; }
      PORT="$2"
      shift 2
      ;;
    -f|--fqbn)
      [[ $# -ge 2 ]] || { echo "Missing value for $1"; exit 2; }
      FQBN="$2"
      shift 2
      ;;
    -s|--sketch)
      [[ $# -ge 2 ]] || { echo "Missing value for $1"; exit 2; }
      SKETCH_DIR="$2"
      shift 2
      ;;
    -a|--arduino-cli)
      [[ $# -ge 2 ]] || { echo "Missing value for $1"; exit 2; }
      ARDUINO_CLI="$2"
      shift 2
      ;;
    -c|--compile-only)
      DO_UPLOAD=0
      shift
      ;;
    -u|--upload-only)
      DO_COMPILE=0
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

if [[ ! -x "$ARDUINO_CLI" ]]; then
  echo "arduino-cli not found at $ARDUINO_CLI"
  echo "Run: burst/prepare.sh"
  exit 1
fi

if [[ "$DO_UPLOAD" -eq 1 && ! -e "$PORT" ]]; then
  echo "Serial port not found: $PORT"
  echo "In Windows PowerShell, attach the board to WSL first:"
  echo "  usbipd list"
  echo "  usbipd attach --wsl --busid <ESP32_BUSID>"
  exit 1
fi

if [[ "$DO_UPLOAD" -eq 1 && ! -w "$PORT" ]]; then
  if command -v wsl.exe >/dev/null 2>&1; then
    echo "Granting temporary serial permission for $PORT ..."
    wsl.exe -u root -- chmod a+rw "$PORT"
  else
    echo "No write permission for $PORT"
    echo "Try: sudo chmod a+rw $PORT"
    exit 1
  fi
fi

if [[ "$DO_COMPILE" -eq 1 ]]; then
  echo "Compiling $SKETCH_DIR ..."
  "$ARDUINO_CLI" compile --fqbn "$FQBN" "$SKETCH_DIR"
fi

if [[ "$DO_UPLOAD" -eq 1 ]]; then
  echo
  echo "Uploading to $PORT ..."
  "$ARDUINO_CLI" upload -p "$PORT" --fqbn "$FQBN" "$SKETCH_DIR"
fi

echo
echo "Done."
