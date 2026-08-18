#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
SKETCH_DIR="${1:-$ROOT_DIR/Firmware/flower_lamp_mic}"
CONFIG_FILE="${2:-$SKETCH_DIR/lamp_config.conf}"
OUTPUT_FILE="$SKETCH_DIR/FlowerLampConfig.generated.h"

if [[ ! -f "$CONFIG_FILE" ]]; then
  [[ -f "$OUTPUT_FILE" ]] && rm "$OUTPUT_FILE"
  exit 0
fi

declare -A ALLOWED_KEYS=()
ALLOWED_KEY_LIST=(
  lamp_pin
  mic_pin
  mode_button_pin
  pwm_frequency_hz
  pwm_resolution_bits
  max_brightness_percent
  breath_floor_percent
  breath_peak_percent
  steady_brightness_percent
  sample_window_ms
  calibration_ms
  noise_floor_stddev_multiplier
  noise_floor_margin_rms
  noise_floor_adapt_alpha
  noise_floor_adapt_max_rise_rms
  sound_reactive_pre_calibration_settle_ms
  sound_reactive_settle_ms
  sample_rate_hz
  dc_center_alpha
  pressure_attack_alpha
  pressure_release_alpha
  brightness_attack_alpha
  brightness_release_alpha
  ceiling_decay_alpha
  noise_gate_multiplier
  noise_gate_offset
  ceiling_headroom_multiplier
  min_pressure_range
  loudness_activation_rms
  loudness_first_level_rms
  loudness_level_count
  brightness_level_count
  loudness_hysteresis_levels
  brightness_level_gamma
  transient_enabled
  transient_baseline_alpha
  transient_threshold_multiplier
  transient_min_rise_rms
  transient_pulse_percent
  transient_pulse_decay_alpha
  transient_cooldown_ms
  button_debounce_ms
  breath_period_ms
  debug_serial_enabled
  debug_serial_interval_ms
)

for key in "${ALLOWED_KEY_LIST[@]}"; do
  ALLOWED_KEYS["$key"]=1
done

trim() {
  local value="$1"
  value="${value#"${value%%[![:space:]]*}"}"
  value="${value%"${value##*[![:space:]]}"}"
  printf '%s' "$value"
}

tmp_file="$(mktemp)"
trap 'rm -f "$tmp_file"' EXIT

{
  echo "#pragma once"
  echo
  echo "// Generated from ${CONFIG_FILE#$ROOT_DIR/}. Do not edit by hand."
} >"$tmp_file"

line_number=0
while IFS= read -r raw_line || [[ -n "$raw_line" ]]; do
  line_number=$((line_number + 1))
  line="$(trim "${raw_line%%#*}")"
  [[ -z "$line" ]] && continue

  if [[ ! "$line" =~ ^([a-z][a-z0-9_]*)[[:space:]]*=[[:space:]]*([0-9]+([.][0-9]+)?)$ ]]; then
    echo "Invalid config line $line_number in $CONFIG_FILE: $raw_line" >&2
    exit 1
  fi

  key="${BASH_REMATCH[1]}"
  value="${BASH_REMATCH[2]}"

  if [[ -z "${ALLOWED_KEYS[$key]+x}" ]]; then
    echo "Unknown config key '$key' in $CONFIG_FILE:$line_number" >&2
    exit 1
  fi

  macro_key="${key^^}"
  echo "#define FLOWER_LAMP_${macro_key} ${value}" >>"$tmp_file"
done <"$CONFIG_FILE"

mv "$tmp_file" "$OUTPUT_FILE"
trap - EXIT

echo "Generated ${OUTPUT_FILE#$ROOT_DIR/}"
