"""VL53L3CX binary sensor platform for ESPHome."""

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import CONF_ID

from .. import vl53l3cx_ns, VL53L3CXComponent, CONF_VL53L3CX_ID

DEPENDENCIES = ["vl53l3cx"]

VL53L3CXBinarySensor = vl53l3cx_ns.class_(
    "VL53L3CXBinarySensor", binary_sensor.BinarySensor, cg.Component
)

CONFIG_SCHEMA = (
    binary_sensor.binary_sensor_schema(VL53L3CXBinarySensor)
    .extend(
        {
            cv.GenerateID(CONF_VL53L3CX_ID): cv.use_id(VL53L3CXComponent),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    """Generate the C++ code for the VL53L3CX binary sensor."""
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await binary_sensor.register_binary_sensor(var, config)

    # Get the parent component and register with cast to base class
    hub = await cg.get_variable(config[CONF_VL53L3CX_ID])
    cg.add(hub.register_binary_sensor(cg.RawExpression(f"static_cast<esphome::vl53l3cx::VL53L3CXBinarySensorBase*>({var})")))