#include "vl53l3cx_binary_sensor.h"
#include "../vl53l3cx.h"
#include "esphome/core/log.h"

namespace esphome {
namespace vl53l3cx {

static const char *const TAG = "vl53l3cx.binary_sensor";

void VL53L3CXBinarySensor::dump_config() {
  LOG_BINARY_SENSOR("", "VL53L3CX Data Ready", this);
}

}  // namespace vl53l3cx
}  // namespace esphome