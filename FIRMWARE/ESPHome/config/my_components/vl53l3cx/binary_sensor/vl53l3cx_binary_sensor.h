#pragma once

#include "esphome/core/component.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "../vl53l3cx.h"

namespace esphome {
namespace vl53l3cx {

class VL53L3CXBinarySensor : public binary_sensor::BinarySensor, public Component, public VL53L3CXBinarySensorBase {
 public:
  void setup() override {}
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }
  void publish_state(bool state) override { binary_sensor::BinarySensor::publish_state(state); }
};

}  // namespace vl53l3cx
}  // namespace esphome