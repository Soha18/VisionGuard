#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/core/preferences.h"
#include <array>

// Include the VL53LX device structure definition
extern "C" {
#include "vl53lx_platform_user_data.h"
// For VL53LX_CalibrationData_t
#include "vl53lx_def.h"
}

namespace esphome {
namespace vl53l3cx {

// Forward declarations (actual classes defined in platform files)
class VL53L3CXSensor;
class VL53L3CXBinarySensor;

// Forward declarations for button classes
class RefSpadCalibrationButton;
class CrosstalkCalibrationButton;
class OffsetCalibrationButton;
class ZeroDistanceCalibrationButton;

// Base class for sensors to avoid incomplete type issues
class VL53L3CXSensorBase {
 public:
  virtual ~VL53L3CXSensorBase() = default;
  virtual void set_target_number(uint8_t target) = 0;
  virtual void publish_distance_state(float distance_m) = 0;
};

class VL53L3CXBinarySensorBase {
 public:
  virtual ~VL53L3CXBinarySensorBase() = default;
  virtual void publish_state(bool state) = 0;
};

// Main sensor hub component
class VL53L3CXComponent : public PollingComponent, public i2c::I2CDevice {
 public:
  // Component lifecycle methods
  void setup() override;
  void update() override;  // Changed from loop() to update() for PollingComponent
  void dump_config() override;
  float get_setup_priority() const override;

  // Configuration setters
  void set_distance_mode(uint8_t distance_mode) { this->distance_mode_ = distance_mode; }
  void set_timing_budget(uint32_t timing_budget_us) { this->timing_budget_us_ = timing_budget_us; }
  void set_signal_rate_limit(float signal_rate_mcps) { this->signal_rate_limit_mcps_ = signal_rate_mcps; }
  void set_sigma_threshold(float sigma_mm) { this->sigma_threshold_mm_ = sigma_mm; }
  void set_inter_measurement_period_ms(uint32_t period_ms) { this->inter_measurement_period_ms_ = period_ms; inter_measurement_period_set_ = true; }
  void set_roi(uint8_t top_left_x, uint8_t top_left_y, uint8_t bottom_right_x, uint8_t bottom_right_y) {
    this->roi_top_left_x_ = top_left_x;
    this->roi_top_left_y_ = top_left_y;
    this->roi_bottom_right_x_ = bottom_right_x;
    this->roi_bottom_right_y_ = bottom_right_y;
    this->roi_configured_ = true;
  }
  void set_smudge_correction_mode(uint8_t mode) { this->smudge_correction_mode_ = mode; }
  void set_target_order(uint8_t order) { this->target_order_ = order; }  // 1=distance, 2=signal strength
  void set_merge_threshold(uint32_t threshold) { this->merge_threshold_ = threshold; }
  void set_hist_merge_enabled(bool enabled) { this->hist_merge_enabled_ = enabled; }
  void set_hist_noise_threshold(uint16_t threshold) { this->hist_noise_threshold_ = threshold; }
  void set_hist_merge_max_size(uint8_t size) { this->hist_merge_max_size_ = size; }
  void set_xshut_pin(GPIOPin *pin) { this->xshut_pin_ = pin; }
  void set_interrupt_pin(GPIOPin *pin) { this->interrupt_pin_ = pin; }

  // Sensor registration
  void register_distance_sensor(VL53L3CXSensorBase *sensor, uint8_t target_number);
  void register_binary_sensor(VL53L3CXBinarySensorBase *sensor);
  
  // Button registration
  void set_refspad_calibration_button(RefSpadCalibrationButton *button) { this->refspad_calibration_button_ = button; }
  void set_crosstalk_calibration_button(CrosstalkCalibrationButton *button) { this->crosstalk_calibration_button_ = button; }
  void set_offset_calibration_button(OffsetCalibrationButton *button) { this->offset_calibration_button_ = button; }
  void set_zero_distance_calibration_button(ZeroDistanceCalibrationButton *button) { this->zero_distance_calibration_button_ = button; }
  
  // Calibration methods (called by buttons)
  void perform_refspad_calibration();
  void perform_crosstalk_calibration();
  void perform_offset_calibration();
  void perform_zero_distance_calibration();  // Simplified field calibration

  // Get distance reading in millimeters
  uint16_t get_distance_mm();
  
  // Check if data is ready
  bool is_data_ready();

 protected:
  // Device structure for ST library
  VL53LX_Dev_t *device_ = nullptr;

  // Configuration parameters
  uint8_t distance_mode_{2};  // Default MEDIUM
  uint32_t timing_budget_us_{33000};  // 33ms default
  float signal_rate_limit_mcps_{0.1f};  // 0.1 MCPS (more permissive for multi-target)
  float sigma_threshold_mm_{60.0f};  // 60mm default sigma threshold
  uint32_t inter_measurement_period_ms_{0};  // If 0, compute from timing budget
  uint8_t target_order_{1};  // 1=closest first (default), 2=strongest first
  bool roi_configured_{false};
  uint8_t roi_top_left_x_{0};
  uint8_t roi_top_left_y_{0};
  uint8_t roi_bottom_right_x_{15};
  uint8_t roi_bottom_right_y_{15};
  uint8_t smudge_correction_mode_{1};  // Default CONTINUOUS
  uint32_t merge_threshold_{15000};  // ST default. Lower value separates targets more.
  bool hist_merge_enabled_{true};     // Default per ST tuning
  uint16_t hist_noise_threshold_{50}; // Default per ST tuning
  uint8_t hist_merge_max_size_{8};    // Default per ST tuning (implementation defined)
  GPIOPin *xshut_pin_{nullptr};
  GPIOPin *interrupt_pin_{nullptr};

  // Runtime state
  bool device_initialized_{false};
  bool first_measurement_discarded_{false};  // Track Range1 discard
  uint16_t last_distance_mm_{0};
  uint32_t last_measurement_time_{0};
  uint8_t last_stream_count_{0};  // Track StreamCount for missed measurement detection
  uint32_t missed_measurements_{0};  // Count of missed measurements
  uint32_t consecutive_errors_{0};  // Track consecutive errors for recovery
  uint32_t total_measurements_{0};  // Track total measurement attempts
  uint32_t valid_measurements_{0};  // Track successful measurements
  uint32_t last_performance_check_{0};  // Last time we checked performance
  bool performance_degraded_{false};  // Flag for degraded performance
  bool inter_measurement_period_set_{false};

  // Registered sensors (indexed by target number) - using base classes
  std::array<VL53L3CXSensorBase *, 4> distance_sensors_{nullptr, nullptr, nullptr, nullptr};
  VL53L3CXBinarySensorBase *binary_sensor_{nullptr};
  
  // Registered calibration buttons
  RefSpadCalibrationButton *refspad_calibration_button_{nullptr};
  CrosstalkCalibrationButton *crosstalk_calibration_button_{nullptr};
  OffsetCalibrationButton *offset_calibration_button_{nullptr};
  ZeroDistanceCalibrationButton *zero_distance_calibration_button_{nullptr};
  
  // Calibration data storage
  ESPPreferenceObject calibration_pref_;
  bool calibration_loaded_{false};
  VL53LX_CalibrationData_t stored_calibration_data_{};

  // Internal methods
  bool initialize_device_();
  bool read_measurement_();
  void setup_gpio_pins_();
  void reset_device_();
  
  // Calibration data persistence
  bool save_calibration_data_();
  bool load_calibration_data_();
};

}  // namespace vl53l3cx
}  // namespace esphome