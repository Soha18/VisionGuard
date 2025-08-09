# VL53L3CX ESPHome Component

ESPHome component for the ST VL53L3CX Time-of-Flight sensor with multi-target support.

## Features
- I2C communication interface using ESPHome's I2C abstraction
- Multi-target detection (up to 4 targets simultaneously)
- Configurable distance modes (SHORT, MEDIUM, LONG)
- Configurable timing budget (20ms to 1000ms)
- GPIO control for XSHUT and interrupt pins
- Binary sensor for data ready status
- Sensor hub architecture with individual sensors per target
- Critical sensor operation compliance (crosstalk compensation, range validation, etc.)

## Configuration (comprehensive example)

```yaml
esphome:
  name: vl53l3cx-demo

esp32:
  board: esp32-c6-devkitm-1
  framework:
    type: esp-idf

logger:
  level: DEBUG

i2c:
  - id: bus_a
    sda: GPIO6
    scl: GPIO7
    scan: true
    frequency: 100kHz

external_components:
  - source:
      type: local
      path: components
    components: [vl53l3cx]

vl53l3cx:
  - id: tof_sensor
    address: 0x29
    distance_mode: MEDIUM      # SHORT | MEDIUM | LONG
    timing_budget: 33ms        # 20..1000ms
    inter_measurement_period: 40ms  # >= timing_budget
    update_interval: 100ms          # >= inter_measurement_period

    # Optional accuracy/SNR tuning
    signal_rate_limit: 0.1     # MCPS
    sigma_threshold: 60.0      # mm

    # Multi-target tuning (advanced)
    target_order: DISTANCE     # or SIGNAL_STRENGTH
    # merge_threshold: 12000   # lower reduces pulse merging
    # hist_noise_threshold: 30 # lower to keep secondary peaks
    # hist_merge: true         # default
    # hist_merge_max_size: 8   # advanced

    # Optional ROI to narrow the field of view
    # roi:
    #   top_left_x: 6
    #   top_left_y: 14
    #   bottom_right_x: 9
    #   bottom_right_y: 5

    xshut_pin:
      number: GPIO3
      mode: { output: true, pullup: true }
    interrupt_pin:
      number: GPIO2
      mode: { input: true, pullup: false }

sensor:
  - platform: vl53l3cx
    vl53l3cx_id: tof_sensor
    name: "ToF Target 0 Distance"
    target_number: 0
  - platform: vl53l3cx
    vl53l3cx_id: tof_sensor
    name: "ToF Target 1 Distance"
    target_number: 1
  - platform: vl53l3cx
    vl53l3cx_id: tof_sensor
    name: "ToF Target 2 Distance"
    target_number: 2
  - platform: vl53l3cx
    vl53l3cx_id: tof_sensor
    name: "ToF Target 3 Distance"
    target_number: 3

binary_sensor:
  - platform: vl53l3cx
    vl53l3cx_id: tof_sensor
    name: "ToF Data Ready"
```

## Configuration Options (with defaults)

### Main Component (`vl53l3cx`)
- **distance_mode** (Optional, default: `MEDIUM`): Distance measurement mode
  - `SHORT`: Up to 1.36m, better ambient immunity
  - `MEDIUM`: Up to 2.90m (default), maximum distance
  - `LONG`: Up to 3.60m, lower power consumption
- **timing_budget** (Optional, default: `33ms`): Measurement timing budget, 20ms to 1000ms
- **inter_measurement_period** (Optional): Time between measurement starts (ms). Must be >= timing_budget. If omitted the component now applies a deterministic default of `timing_budget + 5ms` to avoid overlap while maximizing throughput.
- **update_interval** (Optional, default: `100ms`): How often to poll for new data (must be >= inter_measurement_period). The sensor free-runs internally; polling only fetches ready results.
- **xshut_pin** (Optional): GPIO pin for XSHUT control (hardware reset)
- **interrupt_pin** (Optional): GPIO pin for interrupt input (data ready signal)
- **address** (Optional): I2C address (default: 0x29)
- **merge_threshold** (Optional, default: `15000`): Multi-target separation threshold. Lower (e.g. 12000) separates nearby targets more aggressively. Applied deterministically even if not set.
- **hist_noise_threshold** (Optional, default: `50`): Histogram noise floor. Lower (e.g. 30) preserves weaker secondary peaks at cost of more noise.
- **hist_merge** (Optional, default: `true`): Enable histogram merge algorithm. Disable only for debugging or very specialized scenes.
- **hist_merge_max_size** (Optional, default: `8`): Limit on histogram merge window size (1..16). Larger allows broader pulse grouping.
- **signal_rate_limit** (Optional, default: `0.1` MCPS): Minimum return rate; lower is more permissive.
- **sigma_threshold** (Optional, default: `60.0` mm): Maximum allowed sigma (measurement uncertainty). Higher tolerates noisier data.
- **smudge_correction_mode** (Optional, default: `CONTINUOUS`): One of `DISABLE`, `CONTINUOUS`, `SINGLE`, `DEBUG`. (`DISABLE` maps to vendor `NONE`).
- **target_order** (Optional, default: `DISTANCE`): Sort multi-target results by distance or signal strength.
- **roi** (Optional): Restrict field-of-view. Coordinates validated so that `top_left_x <= bottom_right_x` and `top_left_y <= bottom_right_y` in the SPAD array (0..15 each axis).

Note: Multi-target detection is always enabled (up to 4 targets). Use merge_threshold and histogram tuning to adjust separation aggressiveness.

### Sensor Platform
- **vl53l3cx_id** (Required): Reference to the main component
- **target_number** (Optional): Target index for multi-target mode (0-3, default: 0)
- All standard ESPHome sensor options (name, filters, etc.)

### Binary Sensor Platform
- **vl53l3cx_id** (Required): Reference to the main component
- All standard ESPHome binary sensor options (name, filters, etc.)

## Key Features

### Critical Sensor Operation Compliance & Deterministic Defaults
- **Crosstalk Compensation**: Forced enabled at startup (ST library leaves it disabled by default)
- **Smudge Correction**: Multi-mode; default CONTINUOUS. Modes: DISABLE (off), CONTINUOUS (recommended), SINGLE (one-shot), DEBUG (diagnostic without auto updates).
- **Offset Correction**: Per-VCSEL mode after calibration for optimal accuracy
- **Deterministic Advanced Defaults**: merge_threshold=15000, hist_noise_threshold=50, hist_merge=true, hist_merge_max_size=8 always applied for consistent behavior across firmware versions
- **Range1 Discard**: First measurement discarded per ST guidance
- **StreamCount Rollover Handling**: Detects missed measurements
- **Robust Error Recovery**: Context-specific retry/backoff strategies
- **I2C Chunking**: Large transactions split into 32-byte segments

### ESPHome Integration
- **PollingComponent**: Proper ESPHome polling component with configurable update intervals
- **Sensor Hub Architecture**: Up to 4 individual distance sensors per device
- **I2C Abstraction**: Full integration with ESPHome's I2C system including transaction chunking
- **GPIO Control**: Optional XSHUT (reset) and interrupt pin support
- **Binary Sensor**: Data ready status indication

## Technical Architecture

The component consists of:
- **Main Hub Component** (`vl53l3cx.cpp/h`): PollingComponent that manages sensor hardware and implements critical ST VL53L3CX operations
- **Distance Sensor Platform** (`sensor/`): Individual ESPHome sensors for each of the 4 possible targets
- **Binary Sensor Platform** (`binary_sensor/`): Data ready status indicator
- **ST Core Drivers** (`vl53lx_*.c/h`): ST's VL53LX driver library integrated with full functionality
- **ESPHome Platform Bridge** (`vl53lx_platform.cpp`): Custom I2C implementation with chunking support for large transactions

### Integration Strategy
- **Core Driver Integration**: Uses ST's complete VL53LX driver for device control
- **Platform Abstraction Replacement**: Custom ESPHome-compatible platform layer
- **I2C Transaction Chunking**: Handles large register writes (up to 135 bytes) via 32-byte chunks
  and large reads using the same chunking strategy
- **Memory Management**: Dynamic allocation for 9432-byte device structure

## Hardware Requirements

- **ESP32** with ESP-IDF framework (Arduino framework not supported)
- **I2C Bus**: 400 kHz recommended, pull-up resistors required
- **Power**: 2.6V to 3.5V supply
- **Optional GPIO**: XSHUT (reset) and GPIO1 (interrupt) pins

## Software Requirements

- **ESPHome**: 2023.8.0 or newer
- **Framework**: ESP-IDF only (Arduino framework not compatible)
- **I2C Component**: Must be configured in ESPHome

## Important Notes

### Factory Calibration Required
For optimal accuracy, the sensor requires factory calibration:
1. **RefSPAD Calibration**: No target required
2. **Crosstalk Calibration**: Target at 600mm in dark environment
3. **Offset Calibration**: Known target distance, signal rate 2-80 MCps

#### Calibration buttons (ESPHome/HA)
- RefSPAD: "ToF RefSPAD Calibration" (ensure clear path; no target)
- Crosstalk: "ToF Crosstalk Calibration (600mm)" (17% target at exactly 600 mm, dark)
- Offset: "ToF Offset Calibration" (known distance; component uses Per-VCSEL mode)
- Zero-distance (field): "ToF Zero-Distance Calibration" (target touching cover glass)

Flow: Run in the above order. The component stops ranging, performs calibration via ST APIs, re-enables crosstalk compensation when needed, then restarts ranging. Persistence of calibration data is planned.
 
#### Persistence
- Calibration data is saved automatically to ESPHome preferences after each calibration step and reloaded on boot.
- Data is keyed by I2C address, so multiple sensors on the same node are supported.
- To clear stored calibration, use ESPHome’s preferences reset or change the I2C address.

### Multi-Target Detection
- Always enabled, can detect up to 4 targets simultaneously
- Targets are automatically sorted by distance (closest first)
- Each target gets its own sensor entity in ESPHome
- "No target" condition reports 8.191m distance

### Performance Characteristics
- **Update Rate**: Configurable via `update_interval` (default 100ms)
- **Timing Budget**: 20ms to 1000ms affects accuracy vs. speed
- **Distance Modes**: SHORT/MEDIUM/LONG affect range vs. ambient immunity
- **Memory Usage**: 9432 bytes allocated for device structure

## License

This component incorporates ST's VL53LX driver library under ST's license terms.