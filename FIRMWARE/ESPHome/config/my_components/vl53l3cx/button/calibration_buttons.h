#pragma once

#include "esphome/components/button/button.h"
#include "../vl53l3cx.h"

namespace esphome {
namespace vl53l3cx {

class RefSpadCalibrationButton : public button::Button, public Parented<VL53L3CXComponent> {
 public:
  RefSpadCalibrationButton() = default;

 protected:
  void press_action() override;
};

class CrosstalkCalibrationButton : public button::Button, public Parented<VL53L3CXComponent> {
 public:
  CrosstalkCalibrationButton() = default;

 protected:
  void press_action() override;
};

class OffsetCalibrationButton : public button::Button, public Parented<VL53L3CXComponent> {
 public:
  OffsetCalibrationButton() = default;

 protected:
  void press_action() override;
};

class ZeroDistanceCalibrationButton : public button::Button, public Parented<VL53L3CXComponent> {
 public:
  ZeroDistanceCalibrationButton() = default;

 protected:
  void press_action() override;
};

}  // namespace vl53l3cx
}  // namespace esphome