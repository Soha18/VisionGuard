#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "../vl53l3cx.h"

namespace esphome {
namespace vl53l3cx {

class VL53L3CXSensor : public sensor::Sensor, public Component, public VL53L3CXSensorBase {
 public:
  void setup() override {}
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }
  
  void set_target_number(uint8_t target) override { target_number_ = target; }
  uint8_t get_target_number() const { return target_number_; }
  void publish_distance_state(float distance_m) override { this->publish_state(distance_m); }
  
 protected:
  uint8_t target_number_{0};
};

}  // namespace vl53l3cx
}  // namespace esphome