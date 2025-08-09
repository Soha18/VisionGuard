"""VL53L3CX Time-of-Flight sensor hub component for ESPHome."""

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c
from esphome import pins
from esphome.const import CONF_ID, CONF_UPDATE_INTERVAL
from esphome.core import TimePeriod

CODEOWNERS = ["@mssaleh"]
DEPENDENCIES = ["i2c"]
MULTI_CONF = True

# Component is built for ESP32 with ESP-IDF only
ESP_PLATFORMS = ["esp32"]

# Configuration keys
CONF_VL53L3CX_ID = "vl53l3cx_id"
CONF_DISTANCE_MODE = "distance_mode"
CONF_TIMING_BUDGET = "timing_budget"
CONF_XSHUT_PIN = "xshut_pin"
CONF_INTERRUPT_PIN = "interrupt_pin"
CONF_SIGNAL_RATE_LIMIT = "signal_rate_limit"
CONF_SIGMA_THRESHOLD = "sigma_threshold"
CONF_SMUDGE_CORRECTION_MODE = "smudge_correction_mode"
CONF_TARGET_ORDER = "target_order"
CONF_INTER_MEASUREMENT_PERIOD = "inter_measurement_period"
CONF_MERGE_THRESHOLD = "merge_threshold"
CONF_HIST_MERGE = "hist_merge"
CONF_HIST_NOISE_THRESHOLD = "hist_noise_threshold"
CONF_HIST_MERGE_MAX_SIZE = "hist_merge_max_size"
CONF_ROI = "roi"
CONF_ROI_TOP_LEFT_X = "top_left_x"
CONF_ROI_TOP_LEFT_Y = "top_left_y"
CONF_ROI_BOTTOM_RIGHT_X = "bottom_right_x"
CONF_ROI_BOTTOM_RIGHT_Y = "bottom_right_y"

# Distance modes
DISTANCE_MODES = {"SHORT": 1, "MEDIUM": 2, "LONG": 3}

# Smudge correction modes (user friendly) mapped to vendor API
SMUDGE_CORRECTION_MODES = {
    "DISABLE": 0,      # maps to vendor NONE
    "CONTINUOUS": 1,
    "SINGLE": 2,
    "DEBUG": 3,
}

# Target ordering modes for multi-target detection
TARGET_ORDER_MODES = {
    "DISTANCE": 1,  # Closest targets first (default)
    "SIGNAL_STRENGTH": 2,  # Strongest signal first
}

# Namespace and class declarations
vl53l3cx_ns = cg.esphome_ns.namespace("vl53l3cx")
VL53L3CXComponent = vl53l3cx_ns.class_(
    "VL53L3CXComponent", cg.PollingComponent, i2c.I2CDevice
)

# Main component configuration schema
def _validate_timing(cfg):
    # Provide fallback defaults for validation context
    timing_budget = cfg.get(CONF_TIMING_BUDGET, TimePeriod(milliseconds=33))
    tb_us = timing_budget.total_microseconds
    # Ceil to milliseconds
    timing_budget_ms = (tb_us + 999) // 1000
    ui_ms = None
    if CONF_UPDATE_INTERVAL in cfg and cfg[CONF_UPDATE_INTERVAL] is not None:
        ui_ms = cfg[CONF_UPDATE_INTERVAL].total_milliseconds

    if CONF_INTER_MEASUREMENT_PERIOD in cfg:
        imp_ms = cfg[CONF_INTER_MEASUREMENT_PERIOD].total_milliseconds
        if imp_ms < timing_budget_ms:
            raise cv.Invalid(
                f"inter_measurement_period ({imp_ms} ms) must be >= timing_budget ({timing_budget_ms} ms)"
            )
        if ui_ms is not None and ui_ms < imp_ms:
            raise cv.Invalid(
                f"update_interval ({ui_ms} ms) must be >= inter_measurement_period ({imp_ms} ms)"
            )
    else:
        # If IMP not set, ensure update_interval isn't shorter than timing_budget to avoid overlap
        if ui_ms is not None and ui_ms < timing_budget_ms:
            raise cv.Invalid(
                f"update_interval ({ui_ms} ms) must be >= timing_budget ({timing_budget_ms} ms) when inter_measurement_period is not set"
            )

    return cfg


def _validate_roi(cfg):
    if CONF_ROI in cfg:
        roi = cfg[CONF_ROI]
        # Basic geometric sanity: top-left must be left of bottom-right
        if roi[CONF_ROI_TOP_LEFT_X] > roi[CONF_ROI_BOTTOM_RIGHT_X]:
            raise cv.Invalid("ROI top_left_x must be <= bottom_right_x")
        # Coordinate system for VL53L3CX: (0,0) top-left, Y increases downward in SPAD array.
        # Enforce top_left_y <= bottom_right_y.
        if roi[CONF_ROI_TOP_LEFT_Y] > roi[CONF_ROI_BOTTOM_RIGHT_Y]:
            raise cv.Invalid("ROI top_left_y must be <= bottom_right_y")
    return cfg

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(VL53L3CXComponent),
            # Keep original working default style for core options
            cv.Optional(CONF_DISTANCE_MODE): cv.enum(DISTANCE_MODES, upper=True),
            cv.Optional(CONF_TIMING_BUDGET): cv.All(
                cv.positive_time_period_microseconds,
                cv.Range(
                    min=TimePeriod(microseconds=20000),
                    max=TimePeriod(microseconds=1000000),
                ),
            ),
            cv.Optional(CONF_SIGNAL_RATE_LIMIT): cv.All(
                cv.float_range(min=0.1, max=100.0),
                cv.positive_float,
            ),
            cv.Optional(CONF_SIGMA_THRESHOLD): cv.All(
                cv.float_range(min=1.0, max=1000.0),
                cv.positive_float,
            ),
            cv.Optional(CONF_SMUDGE_CORRECTION_MODE): cv.enum(
                SMUDGE_CORRECTION_MODES, upper=True
            ),
            cv.Optional(CONF_TARGET_ORDER): cv.enum(
                TARGET_ORDER_MODES, upper=True
            ),
            cv.Optional(CONF_INTER_MEASUREMENT_PERIOD): cv.All(
                cv.positive_time_period_milliseconds,
            ),
            # Advanced tuning (no schema defaults; we inject deterministic defaults in to_code)
            cv.Optional(CONF_MERGE_THRESHOLD): cv.int_range(min=1000, max=30000),
            # Advanced histogram tuning (optional)
            cv.Optional(CONF_HIST_MERGE): cv.boolean,
            cv.Optional(CONF_HIST_NOISE_THRESHOLD): cv.int_range(min=10, max=200),
            cv.Optional(CONF_HIST_MERGE_MAX_SIZE): cv.int_range(min=1, max=16),
            cv.Optional(CONF_ROI): cv.Schema(
                {
                    cv.Required(CONF_ROI_TOP_LEFT_X): cv.int_range(min=0, max=15),
                    cv.Required(CONF_ROI_TOP_LEFT_Y): cv.int_range(min=0, max=15),
                    cv.Required(CONF_ROI_BOTTOM_RIGHT_X): cv.int_range(min=0, max=15),
                    cv.Required(CONF_ROI_BOTTOM_RIGHT_Y): cv.int_range(min=0, max=15),
                }
            ),
            cv.Optional(CONF_XSHUT_PIN): pins.gpio_output_pin_schema,
            cv.Optional(CONF_INTERRUPT_PIN): pins.gpio_input_pin_schema,
        }
    )
    .extend(cv.polling_component_schema("100ms"))
    .extend(i2c.i2c_device_schema(0x29)),
    _validate_timing,
    _validate_roi,
)


async def to_code(config):
    """Generate the C++ code for the VL53L3CX component."""
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    # Core defaults if omitted
    distance_mode = config.get(CONF_DISTANCE_MODE, "MEDIUM")
    cg.add(var.set_distance_mode(DISTANCE_MODES[distance_mode]))

    timing_budget_period = config.get(CONF_TIMING_BUDGET, TimePeriod(milliseconds=33))
    timing_budget_us = timing_budget_period.total_microseconds
    cg.add(var.set_timing_budget(int(timing_budget_us)))
    
    # Set signal rate limit (in MCPS)
    cg.add(var.set_signal_rate_limit(config.get(CONF_SIGNAL_RATE_LIMIT, 0.1)))
    
    # Set sigma threshold (in mm)
    cg.add(var.set_sigma_threshold(config.get(CONF_SIGMA_THRESHOLD, 60.0)))
    
    # Smudge correction mode (default CONTINUOUS)
    mode_name = config.get(CONF_SMUDGE_CORRECTION_MODE, "CONTINUOUS")
    mode_val = SMUDGE_CORRECTION_MODES[mode_name]
    cg.add(var.set_smudge_correction_mode(mode_val))
    
    # Set target order
    target_order_key = config.get(CONF_TARGET_ORDER, "DISTANCE")
    cg.add(var.set_target_order(TARGET_ORDER_MODES[target_order_key]))

    # Optional inter-measurement period (ms)
    if CONF_INTER_MEASUREMENT_PERIOD in config and config[CONF_INTER_MEASUREMENT_PERIOD] is not None:
        imp_ms = int(config[CONF_INTER_MEASUREMENT_PERIOD].total_milliseconds)
        cg.add(var.set_inter_measurement_period_ms(imp_ms))

    # Deterministic advanced tuning: apply explicit defaults if user omitted
    merge_threshold = config.get(CONF_MERGE_THRESHOLD, 15000)
    cg.add(var.set_merge_threshold(merge_threshold))
    hist_merge_enabled = config.get(CONF_HIST_MERGE, True)
    # Advanced histogram tuning
    cg.add(var.set_hist_merge_enabled(hist_merge_enabled))
    hist_noise_threshold = config.get(CONF_HIST_NOISE_THRESHOLD, 50)
    cg.add(var.set_hist_noise_threshold(hist_noise_threshold))
    hist_merge_max_size = config.get(CONF_HIST_MERGE_MAX_SIZE, 8)
    cg.add(var.set_hist_merge_max_size(hist_merge_max_size))
    
    # Set ROI if configured
    if CONF_ROI in config:
        roi = config[CONF_ROI]
        cg.add(var.set_roi(roi[CONF_ROI_TOP_LEFT_X], roi[CONF_ROI_TOP_LEFT_Y], 
                          roi[CONF_ROI_BOTTOM_RIGHT_X], roi[CONF_ROI_BOTTOM_RIGHT_Y]))

    # Configure GPIO pins if specified
    if CONF_XSHUT_PIN in config:
        xshut_pin = await cg.gpio_pin_expression(config[CONF_XSHUT_PIN])
        cg.add(var.set_xshut_pin(xshut_pin))

    if CONF_INTERRUPT_PIN in config:
        int_pin = await cg.gpio_pin_expression(config[CONF_INTERRUPT_PIN])
        cg.add(var.set_interrupt_pin(int_pin))

    # Add build flags for ESP-IDF framework
    cg.add_platformio_option("framework", "espidf")