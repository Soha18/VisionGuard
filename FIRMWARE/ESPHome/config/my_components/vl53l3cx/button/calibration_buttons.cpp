#include "calibration_buttons.h"
#include "esphome/core/log.h"

namespace esphome {
namespace vl53l3cx {

static const char *const TAG = "vl53l3cx.button";

void RefSpadCalibrationButton::press_action() {
  ESP_LOGD(TAG, "RefSPAD calibration button pressed");
  this->parent_->perform_refspad_calibration();
}

void CrosstalkCalibrationButton::press_action() {
  ESP_LOGD(TAG, "Crosstalk calibration button pressed");
  this->parent_->perform_crosstalk_calibration();
}

void OffsetCalibrationButton::press_action() {
  ESP_LOGD(TAG, "Offset calibration button pressed");
  this->parent_->perform_offset_calibration();
}

void ZeroDistanceCalibrationButton::press_action() {
  ESP_LOGD(TAG, "Zero-distance calibration button pressed");
  this->parent_->perform_zero_distance_calibration();
}

}  // namespace vl53l3cx
}  // namespace esphome