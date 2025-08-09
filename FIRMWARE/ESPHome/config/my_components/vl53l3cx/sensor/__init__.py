"""VL53L3CX distance sensor platform for ESPHome."""

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_DISTANCE,
    STATE_CLASS_MEASUREMENT,
    UNIT_METER,
)

from .. import vl53l3cx_ns, VL53L3CXComponent, CONF_VL53L3CX_ID

DEPENDENCIES = ["vl53l3cx"]

CONF_TARGET_NUMBER = "target_number"

VL53L3CXSensor = vl53l3cx_ns.class_("VL53L3CXSensor", sensor.Sensor, cg.Component)

CONFIG_SCHEMA = (
    sensor.sensor_schema(
        VL53L3CXSensor,
        unit_of_measurement=UNIT_METER,
        accuracy_decimals=3,
        device_class=DEVICE_CLASS_DISTANCE,
        state_class=STATE_CLASS_MEASUREMENT,
    )
    .extend(
        {
            cv.GenerateID(CONF_VL53L3CX_ID): cv.use_id(VL53L3CXComponent),
            cv.Optional(CONF_TARGET_NUMBER, default=0): cv.int_range(min=0, max=3),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    """Generate the C++ code for the VL53L3CX sensor."""
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await sensor.register_sensor(var, config)

    # Get the parent component and register with cast to base class
    hub = await cg.get_variable(config[CONF_VL53L3CX_ID])
    cg.add(hub.register_distance_sensor(cg.RawExpression(f"static_cast<esphome::vl53l3cx::VL53L3CXSensorBase*>({var})"), config[CONF_TARGET_NUMBER]))