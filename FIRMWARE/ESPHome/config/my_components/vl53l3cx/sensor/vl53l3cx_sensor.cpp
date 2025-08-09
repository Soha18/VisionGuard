#include "vl53l3cx_sensor.h"
#include "../vl53l3cx.h"
#include "esphome/core/log.h"

namespace esphome {
namespace vl53l3cx {

static const char *const TAG = "vl53l3cx.sensor";

void VL53L3CXSensor::dump_config() {
  LOG_SENSOR("", "VL53L3CX Distance", this);
  ESP_LOGCONFIG(TAG, "  Target Number: %u", this->target_number_);
}

}  // namespace vl53l3cx
}  // namespace esphome