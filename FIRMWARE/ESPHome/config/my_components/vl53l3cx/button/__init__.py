import esphome.codegen as cg
from esphome.components import button
import esphome.config_validation as cv
from esphome.const import (
    ENTITY_CATEGORY_CONFIG,
)

from .. import CONF_VL53L3CX_ID, VL53L3CXComponent, vl53l3cx_ns

RefSpadCalibrationButton = vl53l3cx_ns.class_("RefSpadCalibrationButton", button.Button)
CrosstalkCalibrationButton = vl53l3cx_ns.class_("CrosstalkCalibrationButton", button.Button)
OffsetCalibrationButton = vl53l3cx_ns.class_("OffsetCalibrationButton", button.Button)
ZeroDistanceCalibrationButton = vl53l3cx_ns.class_("ZeroDistanceCalibrationButton", button.Button)

CONF_REFSPAD_CALIBRATION = "refspad_calibration"
CONF_CROSSTALK_CALIBRATION = "crosstalk_calibration"
CONF_OFFSET_CALIBRATION = "offset_calibration"
CONF_ZERO_DISTANCE_CALIBRATION = "zero_distance_calibration"

CONFIG_SCHEMA = {
    cv.GenerateID(CONF_VL53L3CX_ID): cv.use_id(VL53L3CXComponent),
    cv.Optional(CONF_REFSPAD_CALIBRATION): button.button_schema(
        RefSpadCalibrationButton,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon="mdi:crosshairs",
    ),
    cv.Optional(CONF_CROSSTALK_CALIBRATION): button.button_schema(
        CrosstalkCalibrationButton,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon="mdi:tune",
    ),
    cv.Optional(CONF_OFFSET_CALIBRATION): button.button_schema(
        OffsetCalibrationButton,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon="mdi:format-align-center",
    ),
    cv.Optional(CONF_ZERO_DISTANCE_CALIBRATION): button.button_schema(
        ZeroDistanceCalibrationButton,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon="mdi:target-variant",
    ),
}


async def to_code(config):
    vl53l3cx_component = await cg.get_variable(config[CONF_VL53L3CX_ID])
    
    if refspad_config := config.get(CONF_REFSPAD_CALIBRATION):
        b = await button.new_button(refspad_config)
        await cg.register_parented(b, config[CONF_VL53L3CX_ID])
        cg.add(vl53l3cx_component.set_refspad_calibration_button(b))
        
    if crosstalk_config := config.get(CONF_CROSSTALK_CALIBRATION):
        b = await button.new_button(crosstalk_config)
        await cg.register_parented(b, config[CONF_VL53L3CX_ID])
        cg.add(vl53l3cx_component.set_crosstalk_calibration_button(b))
        
    if offset_config := config.get(CONF_OFFSET_CALIBRATION):
        b = await button.new_button(offset_config)
        await cg.register_parented(b, config[CONF_VL53L3CX_ID])
        cg.add(vl53l3cx_component.set_offset_calibration_button(b))

    if zero_config := config.get(CONF_ZERO_DISTANCE_CALIBRATION):
        b = await button.new_button(zero_config)
        await cg.register_parented(b, config[CONF_VL53L3CX_ID])
        cg.add(vl53l3cx_component.set_zero_distance_calibration_button(b))