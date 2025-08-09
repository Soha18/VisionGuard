#include "vl53l3cx.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include <cstring>

// Include ST library headers
extern "C" {
#include "vl53lx_api.h"
#include "vl53lx_api_core.h"
}

// External function to set component for platform layer
extern void vl53lx_set_i2c_component(esphome::vl53l3cx::VL53L3CXComponent *component);

namespace esphome {
namespace vl53l3cx {

static const char *const TAG = "vl53l3cx";

static const char* get_error_string(VL53LX_Error error) {
  switch(error) {
    case 0: return "NONE";
    case -1: return "CALIBRATION_WARNING";
    case -2: return "MIN_CLIPPED";
    case -3: return "UNDEFINED";
    case -4: return "INVALID_PARAMS";
    case -5: return "NOT_SUPPORTED";
    case -6: return "RANGE_ERROR";
    case -7: return "TIME_OUT";
    case -8: return "MODE_NOT_SUPPORTED";
    case -9: return "BUFFER_TOO_SMALL";
    case -10: return "COMMS_BUFFER_TOO_SMALL";
    case -11: return "GPIO_NOT_EXISTING";
    case -12: return "GPIO_FUNCTIONALITY_NOT_SUPPORTED";
    case -13: return "CONTROL_INTERFACE";
    case -14: return "INVALID_COMMAND";
    case -15: return "DIVISION_BY_ZERO";
    case -16: return "REF_SPAD_INIT";
    case -17: return "GPH_SYNC_CHECK_FAIL";
    case -18: return "STREAM_COUNT_CHECK_FAIL";
    case -19: return "GPH_ID_CHECK_FAIL";
    case -20: return "ZONE_STREAM_COUNT_CHECK_FAIL";
    case -21: return "ZONE_GPH_ID_CHECK_FAIL";
    case -22: return "XTALK_EXTRACTION_NO_SAMPLE_FAIL";
    case -23: return "XTALK_EXTRACTION_SIGMA_LIMIT_FAIL";
    case -24: return "OFFSET_CAL_NO_SAMPLE_FAIL";
    case -25: return "OFFSET_CAL_NO_SPADS_ENABLED_FAIL";
    case -26: return "ZONE_CAL_NO_SAMPLE_FAIL";
    case -27: return "TUNING_PARM_KEY_MISMATCH";
    case -28: return "WARNING_REF_SPAD_CHAR_NOT_ENOUGH_SPADS";
    case -29: return "WARNING_REF_SPAD_CHAR_RATE_TOO_HIGH";
    case -30: return "WARNING_REF_SPAD_CHAR_RATE_TOO_LOW";
    case -31: return "WARNING_OFFSET_CAL_MISSING_SAMPLES";
    case -32: return "WARNING_OFFSET_CAL_SIGMA_TOO_HIGH";
    case -33: return "WARNING_OFFSET_CAL_RATE_TOO_HIGH";
    case -34: return "WARNING_OFFSET_CAL_SPAD_COUNT_TOO_LOW";
    case -35: return "WARNING_ZONE_CAL_MISSING_SAMPLES";
    case -36: return "WARNING_ZONE_CAL_SIGMA_TOO_HIGH";
    case -37: return "WARNING_ZONE_CAL_RATE_TOO_HIGH";
    case -38: return "WARNING_XTALK_NO_SAMPLES_FOR_GRADIENT";
    case -39: return "WARNING_XTALK_SIGMA_LIMIT_FOR_GRADIENT";
    case -40: return "WARNING_XTALK_MISSING_SAMPLES";
    case -41: return "NOT_IMPLEMENTED";
    case -42: return "PLATFORM_SPECIFIC_START";
    default: return "UNKNOWN";
  }
}

void VL53L3CXComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up VL53L3CX...");
  
  // Initialize calibration preference object (persist in flash)
  // Use a stable type key based on I2C address to support multiple sensors
  if (global_preferences != nullptr) {
    uint32_t pref_type = 0x564C5300u | (uint32_t)(this->address_ & 0x7Fu); // 'VLS' + addr
    this->calibration_pref_ = global_preferences->make_preference<VL53LX_CalibrationData_t>(pref_type, true);
  }
  
  // Set this component for platform functions to use
  vl53lx_set_i2c_component(this);
  ESP_LOGD(TAG, "Platform component set");
  
  // Setup GPIO pins
  this->setup_gpio_pins_();
  
  // Reset device if XSHUT pin available
  this->reset_device_();
  
  // Allocate device structure
  ESP_LOGD(TAG, "Allocating device structure (size: %u bytes)", sizeof(VL53LX_Dev_t));
  this->device_ = new VL53LX_Dev_t();
  if (!this->device_) {
    ESP_LOGE(TAG, "Failed to allocate device structure");
    this->mark_failed();
    return;
  }
  memset(this->device_, 0, sizeof(VL53LX_Dev_t));
  this->device_->i2c_slave_address = this->address_ << 1;  // Convert 7-bit to 8-bit
  ESP_LOGD(TAG, "Device structure allocated, I2C address: 0x%02X", this->device_->i2c_slave_address);
  
  // Load calibration data (if available)
  bool has_calibration = this->load_calibration_data_();
  
  // Initialize the device
  if (!this->initialize_device_()) {
    ESP_LOGE(TAG, "Failed to initialize VL53L3CX");
    this->mark_failed();
    return;
  }
  
  // Log calibration status
  if (has_calibration) {
    ESP_LOGI(TAG, "Using stored calibration data");
  } else {
    ESP_LOGW(TAG, "No calibration data found - using factory defaults. Consider running calibration.");
  }
  
  // CRITICAL: Enable crosstalk compensation (disabled by default per ST guide)
  // This is mandatory even without calibration data for proper operation
  ESP_LOGD(TAG, "Enabling crosstalk compensation (CRITICAL for accuracy)");
  VL53LX_Error status = VL53LX_SetXTalkCompensationEnable(this->device_, 1);
  if (status != VL53LX_ERROR_NONE) {
    ESP_LOGW(TAG, "Failed to enable crosstalk compensation: %d (%s)", status, get_error_string(status));
    // Continue anyway but log warning
  }
  
  ESP_LOGCONFIG(TAG, "VL53L3CX setup complete");
}

void VL53L3CXComponent::update() {
  if (!this->device_initialized_) {
    return;
  }
  
  // Check if measurement is ready
  uint8_t data_ready = 0;
  VL53LX_Error status = VL53LX_GetMeasurementDataReady(this->device_, &data_ready);
  
  if (status != VL53LX_ERROR_NONE) {
    ESP_LOGW(TAG, "Failed to check data ready: %d (%s)", status, get_error_string(status));
    return;
  }
  
  if (data_ready) {
    // Update binary sensor
    if (this->binary_sensor_ != nullptr) {
      this->binary_sensor_->publish_state(true);
    }
    
    this->read_measurement_();
    
    // Clear binary sensor after reading
    if (this->binary_sensor_ != nullptr) {
      this->binary_sensor_->publish_state(false);
    }
  }
}

void VL53L3CXComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "VL53L3CX:");
  LOG_I2C_DEVICE(this);
  
  const char *mode_str;
  switch (this->distance_mode_) {
    case 1: mode_str = "SHORT"; break;
    case 2: mode_str = "MEDIUM"; break;
    case 3: mode_str = "LONG"; break;
    default: mode_str = "UNKNOWN";
  }
  
  ESP_LOGCONFIG(TAG, "  Distance Mode: %s", mode_str);
  ESP_LOGCONFIG(TAG, "  Timing Budget: %u µs", this->timing_budget_us_);
  ESP_LOGCONFIG(TAG, "  Signal Rate Limit: %.2f MCPS", this->signal_rate_limit_mcps_);
  ESP_LOGCONFIG(TAG, "  Sigma Threshold: %.1f mm", this->sigma_threshold_mm_);
  const char *smudge_mode_str = "UNKNOWN";
  switch (this->smudge_correction_mode_) {
    case 0: smudge_mode_str = "NONE"; break;
    case 1: smudge_mode_str = "CONTINUOUS"; break;
    case 2: smudge_mode_str = "SINGLE"; break;
    case 3: smudge_mode_str = "DEBUG"; break;
  }
  ESP_LOGCONFIG(TAG, "  Smudge Correction Mode: %s", smudge_mode_str);
  ESP_LOGCONFIG(TAG, "  Target Order: %s", this->target_order_ == 1 ? "DISTANCE (closest first)" : "SIGNAL_STRENGTH (strongest first)");
  // Attempt to read back inter-measurement period
  if (this->device_initialized_) {
    uint32_t imp_ms = 0;
    if (VL53LX_get_inter_measurement_period_ms(this->device_, &imp_ms) == VL53LX_ERROR_NONE) {
      ESP_LOGCONFIG(TAG, "  Inter-Measurement Period: %u ms", imp_ms);
    }
  }
  ESP_LOGCONFIG(TAG, "  Merge Threshold: %u", this->merge_threshold_);
  ESP_LOGCONFIG(TAG, "  Hist Merge: %s", this->hist_merge_enabled_ ? "ENABLED" : "DISABLED");
  ESP_LOGCONFIG(TAG, "  Hist Noise Threshold: %u", this->hist_noise_threshold_);
  ESP_LOGCONFIG(TAG, "  Hist Merge Max Size: %u", this->hist_merge_max_size_);
  
  if (this->roi_configured_) {
    ESP_LOGCONFIG(TAG, "  ROI: (%u,%u) to (%u,%u)", 
                 this->roi_top_left_x_, this->roi_top_left_y_,
                 this->roi_bottom_right_x_, this->roi_bottom_right_y_);
  }
  
  if (this->missed_measurements_ > 0) {
    ESP_LOGCONFIG(TAG, "  Missed Measurements: %u", this->missed_measurements_);
  }
  
  LOG_UPDATE_INTERVAL(this);  // Use ESPHome's standard logging macro
  
  if (this->xshut_pin_) {
    ESP_LOGCONFIG(TAG, "  XSHUT Pin: %s", this->xshut_pin_->dump_summary().c_str());
  }
  if (this->interrupt_pin_) {
    ESP_LOGCONFIG(TAG, "  Interrupt Pin: %s", this->interrupt_pin_->dump_summary().c_str());
  }
  
  for (size_t i = 0; i < this->distance_sensors_.size(); i++) {
    if (this->distance_sensors_[i] != nullptr) {
      ESP_LOGCONFIG(TAG, "  Target %u: Distance sensor registered", i);
    }
  }
  
  if (this->binary_sensor_ != nullptr) {
    ESP_LOGCONFIG(TAG, "  Binary sensor: Data Ready sensor registered");
  }
}

float VL53L3CXComponent::get_setup_priority() const {
  return setup_priority::DATA;
}

void VL53L3CXComponent::register_distance_sensor(VL53L3CXSensorBase *sensor, uint8_t target_number) {
  if (target_number < 4) {
    this->distance_sensors_[target_number] = sensor;
    sensor->set_target_number(target_number);
  }
}

void VL53L3CXComponent::register_binary_sensor(VL53L3CXBinarySensorBase *sensor) {
  this->binary_sensor_ = sensor;
}

uint16_t VL53L3CXComponent::get_distance_mm() {
  return this->last_distance_mm_;
}

bool VL53L3CXComponent::is_data_ready() {
  if (!this->device_initialized_) {
    return false;
  }
  
  uint8_t data_ready = 0;
  VL53LX_GetMeasurementDataReady(this->device_, &data_ready);
  return data_ready != 0;
}

bool VL53L3CXComponent::initialize_device_() {
  ESP_LOGD(TAG, "Initializing VL53L3CX device...");
  
  // Wait for device boot
  ESP_LOGD(TAG, "Waiting for device boot...");
  VL53LX_Error status = VL53LX_WaitDeviceBooted(this->device_);
  if (status != VL53LX_ERROR_NONE) {
    ESP_LOGE(TAG, "Device boot timeout: %d (%s)", status, get_error_string(status));
    return false;
  }
  ESP_LOGD(TAG, "Device booted successfully");
  
  // Initialize device
  ESP_LOGD(TAG, "Initializing device data...");
  status = VL53LX_DataInit(this->device_);
  if (status != VL53LX_ERROR_NONE) {
    ESP_LOGE(TAG, "Data init failed: %d (%s)", status, get_error_string(status));
    return false;
  }
  ESP_LOGD(TAG, "Data init successful");

  // If we have stored calibration, apply it before other config for correctness
  if (this->calibration_loaded_) {
    ESP_LOGD(TAG, "Applying stored calibration data");
    VL53LX_Error cal_status = VL53LX_SetCalibrationData(this->device_, &this->stored_calibration_data_);
    if (cal_status != VL53LX_ERROR_NONE) {
      ESP_LOGW(TAG, "Failed to apply stored calibration: %d (%s)", cal_status, get_error_string(cal_status));
    }
  }
  
  // Get device info for verification
  VL53LX_DeviceInfo_t device_info;
  status = VL53LX_GetDeviceInfo(this->device_, &device_info);
  if (status == VL53LX_ERROR_NONE) {
    ESP_LOGD(TAG, "VL53L3CX Info: Type=%u, Major=%u, Minor=%u",
             device_info.ProductType, 
             device_info.ProductRevisionMajor,
             device_info.ProductRevisionMinor);
  }
  
  // Set distance mode
  status = VL53LX_SetDistanceMode(this->device_, (VL53LX_DistanceModes)this->distance_mode_);
  if (status != VL53LX_ERROR_NONE) {
    ESP_LOGE(TAG, "Failed to set distance mode: %d", status);
    return false;
  }
  
  // Set timing budget
  status = VL53LX_SetMeasurementTimingBudgetMicroSeconds(this->device_, this->timing_budget_us_);
  if (status != VL53LX_ERROR_NONE) {
    ESP_LOGE(TAG, "Failed to set timing budget: %d (%s)", status, get_error_string(status));
    return false;
  }
  
  // CRITICAL: Set offset correction mode (required after loading calibration data)
  ESP_LOGD(TAG, "Setting offset correction mode to PerVCSEL (recommended)");
  status = VL53LX_SetOffsetCorrectionMode(this->device_, VL53LX_OFFSETCORRECTIONMODE_PERVCSEL);
  if (status != VL53LX_ERROR_NONE) {
    ESP_LOGW(TAG, "Failed to set offset correction mode: %d (%s)", status, get_error_string(status));
    // Continue anyway as this is not fatal
  }
  
  // Configure signal rate limit using distance mode-specific tuning parameters
  ESP_LOGD(TAG, "Setting signal rate limit: %.2f MCPS for distance mode %d", this->signal_rate_limit_mcps_, this->distance_mode_);
  uint32_t signal_rate_tuning_param;
  switch (this->distance_mode_) {
    case 1: // SHORT
      signal_rate_tuning_param = VL53LX_TUNINGPARM_LITE_SHORT_MIN_COUNT_RATE_RTN_MCPS;
      break;
    case 2: // MEDIUM  
      signal_rate_tuning_param = VL53LX_TUNINGPARM_LITE_MED_MIN_COUNT_RATE_RTN_MCPS;
      break;
    case 3: // LONG
      signal_rate_tuning_param = VL53LX_TUNINGPARM_LITE_LONG_MIN_COUNT_RATE_RTN_MCPS;
      break;
    default:
      signal_rate_tuning_param = VL53LX_TUNINGPARM_LITE_MED_MIN_COUNT_RATE_RTN_MCPS;
      break;
  }
  
  // Convert MCPS to fixed-point for tuning parameter (same format as limit check)
  int32_t signal_rate_value = (int32_t)(this->signal_rate_limit_mcps_ * 65536.0f);
  status = VL53LX_SetTuningParameter(this->device_, signal_rate_tuning_param, signal_rate_value);
  if (status != VL53LX_ERROR_NONE) {
    ESP_LOGW(TAG, "Failed to set signal rate limit tuning parameter: %d (%s)", status, get_error_string(status));
  }
  
  // Configure sigma threshold using histogram sigma threshold tuning parameter
  ESP_LOGD(TAG, "Setting sigma threshold: %.1f mm", this->sigma_threshold_mm_);
  // Convert mm to fixed-point (same format as limit check)
  int32_t sigma_threshold_value = (int32_t)(this->sigma_threshold_mm_ * 65536.0f);
  status = VL53LX_SetTuningParameter(this->device_, VL53LX_TUNINGPARM_HIST_SIGMA_THRESH_MM, sigma_threshold_value);
  if (status != VL53LX_ERROR_NONE) {
    ESP_LOGW(TAG, "Failed to set sigma threshold tuning parameter: %d (%s)", status, get_error_string(status));
  }
  
  // Configure ROI if specified
  if (this->roi_configured_) {
    ESP_LOGD(TAG, "Setting ROI: (%u,%u) to (%u,%u)", 
             this->roi_top_left_x_, this->roi_top_left_y_,
             this->roi_bottom_right_x_, this->roi_bottom_right_y_);
    VL53LX_UserRoi_t roi;
    roi.TopLeftX = this->roi_top_left_x_;
    roi.TopLeftY = this->roi_top_left_y_;
    roi.BotRightX = this->roi_bottom_right_x_;
    roi.BotRightY = this->roi_bottom_right_y_;
    status = VL53LX_SetUserROI(this->device_, &roi);
    if (status != VL53LX_ERROR_NONE) {
      ESP_LOGW(TAG, "Failed to set ROI: %d (%s)", status, get_error_string(status));
    }
  }

  // Inter-measurement period: if user set, ensure >= timing budget; else derive deterministic default = timing_budget_ms + 5ms margin
  uint32_t desired_imp_ms;
  if (this->inter_measurement_period_set_) {
    desired_imp_ms = std::max<uint32_t>(this->inter_measurement_period_ms_, this->timing_budget_us_ / 1000u);
  } else {
    desired_imp_ms = (this->timing_budget_us_ / 1000u) + 5; // small guard interval
    this->inter_measurement_period_ms_ = desired_imp_ms;
  }
  ESP_LOGD(TAG, "Setting inter-measurement period: %u ms", desired_imp_ms);
  status = VL53LX_set_inter_measurement_period_ms(this->device_, desired_imp_ms);
  if (status != VL53LX_ERROR_NONE) {
    ESP_LOGW(TAG, "Failed to set inter-measurement period: %d (%s)", status, get_error_string(status));
  }
  
  // Configure smudge correction (recommended by ST guide for cover glass contamination)
  ESP_LOGD(TAG, "Setting smudge correction mode: %u", this->smudge_correction_mode_);
  status = VL53LX_SmudgeCorrectionEnable(this->device_, (VL53LX_SmudgeCorrectionModes) this->smudge_correction_mode_);
  if (status != VL53LX_ERROR_NONE) {
    ESP_LOGW(TAG, "Failed to configure smudge correction mode: %d (%s)", status, get_error_string(status));
  }
  
  // Configure target ordering for multi-target detection
  ESP_LOGD(TAG, "Setting target order: %u (%s)", this->target_order_, 
           this->target_order_ == 1 ? "closest first" : "strongest first");
  status = VL53LX_SetTuningParameter(this->device_, VL53LX_TUNINGPARM_HIST_TARGET_ORDER, this->target_order_);
  if (status != VL53LX_ERROR_NONE) {
    ESP_LOGW(TAG, "Failed to set target order: %d (%s)", status, get_error_string(status));
  }
  
  // Always configure merge threshold deterministically
  ESP_LOGD(TAG, "Applying merge threshold: %u", this->merge_threshold_);
  status = VL53LX_SetTuningParameter(this->device_, VL53LX_TUNINGPARM_RESET_MERGE_THRESHOLD, (int32_t)this->merge_threshold_);
  if (status != VL53LX_ERROR_NONE) {
    ESP_LOGW(TAG, "Failed to set merge threshold: %d (%s)", status, get_error_string(status));
  }

  // Optional: control histogram merge behavior (advanced)
  // Histogram merge enable/disable
  ESP_LOGD(TAG, "Applying HIST_MERGE: %s", this->hist_merge_enabled_ ? "ENABLED" : "DISABLED");
  status = VL53LX_SetTuningParameter(this->device_, VL53LX_TUNINGPARM_HIST_MERGE, this->hist_merge_enabled_ ? 1 : 0);
  if (status != VL53LX_ERROR_NONE) {
    ESP_LOGW(TAG, "Failed to set HIST_MERGE: %d (%s)", status, get_error_string(status));
  }
  // Noise threshold
  ESP_LOGD(TAG, "Applying HIST_NOISE_THRESHOLD: %u", this->hist_noise_threshold_);
  status = VL53LX_SetTuningParameter(this->device_, VL53LX_TUNINGPARM_HIST_NOISE_THRESHOLD, (int32_t)this->hist_noise_threshold_);
  if (status != VL53LX_ERROR_NONE) {
    ESP_LOGW(TAG, "Failed to set HIST_NOISE_THRESHOLD: %d (%s)", status, get_error_string(status));
  }
  // Merge max size
  ESP_LOGD(TAG, "Applying HIST_MERGE_MAX_SIZE: %u", this->hist_merge_max_size_);
  status = VL53LX_SetTuningParameter(this->device_, VL53LX_TUNINGPARM_HIST_MERGE_MAX_SIZE, (int32_t)this->hist_merge_max_size_);
  if (status != VL53LX_ERROR_NONE) {
    ESP_LOGW(TAG, "Failed to set HIST_MERGE_MAX_SIZE: %d (%s)", status, get_error_string(status));
  }
  
  // Start measurements
  status = VL53LX_StartMeasurement(this->device_);
  if (status != VL53LX_ERROR_NONE) {
    ESP_LOGE(TAG, "Failed to start measurement: %d", status);
    return false;
  }
  
  this->device_initialized_ = true;
  ESP_LOGD(TAG, "VL53L3CX initialized successfully");
  return true;
}

bool VL53L3CXComponent::read_measurement_() {
  VL53LX_MultiRangingData_t ranging_data;
  const uint32_t MAX_CONSECUTIVE_ERRORS = 10;
  const uint32_t MAX_RETRIES = 3;
  
  this->total_measurements_++;
  
  // Retry logic for robust operation
  for (uint32_t retry = 0; retry < MAX_RETRIES; retry++) {
    VL53LX_Error status = VL53LX_GetMultiRangingData(this->device_, &ranging_data);
    
    if (status == VL53LX_ERROR_NONE) {
      this->consecutive_errors_ = 0;  // Reset error counter on success
      this->valid_measurements_++;  // Track successful measurements
      break;  // Success - exit retry loop
    }
    
    // Error occurred - apply recovery strategy
    this->consecutive_errors_++;
    ESP_LOGW(TAG, "Failed to get ranging data (attempt %u/%u): %d (%s)", 
             retry + 1, MAX_RETRIES, status, get_error_string(status));
    
    // Apply sophisticated recovery strategy based on error type and history
    if (status == VL53LX_ERROR_TIME_OUT) {
      // Timeout - increase wait time progressively
      uint32_t wait_time = 10 + (retry * 5);  // 10ms, 15ms, 20ms
      delay(wait_time);
    }
    else if (status == VL53LX_ERROR_RANGE_ERROR) {
      // Range error - simple retry
      delay(10);
    }
    else if (status == VL53LX_ERROR_GPH_SYNC_CHECK_FAIL || 
             status == VL53LX_ERROR_STREAM_COUNT_CHECK_FAIL ||
             status == VL53LX_ERROR_ZONE_STREAM_COUNT_CHECK_FAIL) {
      // Sync failures - restart measurement with progressive delays
      ESP_LOGD(TAG, "Restarting measurement due to sync error (attempt %u)", retry + 1);
      VL53LX_StopMeasurement(this->device_);
      delay(20 + (retry * 10));  // Progressive delay
      VL53LX_StartMeasurement(this->device_);
      delay(this->timing_budget_us_ / 1000 + 10);  // Wait for timing budget + margin
    }
    else if (status == VL53LX_ERROR_CONTROL_INTERFACE) {
      // I2C communication error - progressive backoff
      uint32_t wait_time = 50 + (retry * 25);  // 50ms, 75ms, 100ms
      ESP_LOGD(TAG, "I2C error, waiting %u ms before retry", wait_time);
      delay(wait_time);
    }
    else {
      // Other errors - moderate delay with progressive increase
      uint32_t wait_time = 15 + (retry * 5);
      delay(wait_time);
    }
    
    // Check for persistent failures
    if (this->consecutive_errors_ > MAX_CONSECUTIVE_ERRORS) {
      ESP_LOGE(TAG, "Too many consecutive errors (%u), sensor may have failed", this->consecutive_errors_);
      this->mark_failed();
      return false;
    }
    
    // On last retry, return failure
    if (retry == MAX_RETRIES - 1) {
      ESP_LOGE(TAG, "All retries exhausted, measurement failed");
      return false;
    }
  }
  
  // Clear interrupt and start next measurement
  VL53LX_Error status = VL53LX_ClearInterruptAndStartMeasurement(this->device_);
  if (status != VL53LX_ERROR_NONE) {
    ESP_LOGW(TAG, "Failed to clear interrupt: %d (%s)", status, get_error_string(status));
  }
  
  // CRITICAL: Discard the very first measurement (Range1) per ST guide
  // Range1 lacks wrap-around validation and is not reliable
  if (!this->first_measurement_discarded_) {
    ESP_LOGD(TAG, "Discarding Range1 (first measurement - no wrap-around check)");
    this->first_measurement_discarded_ = true;
    return true;  // Don't process this data, but consider it successful
  }
  
  // Process measurement data for all targets (Range2 onwards)
  ESP_LOGD(TAG, "=== MEASUREMENT DEBUG: Objects found: %u, StreamCount: %u ===", 
           ranging_data.NumberOfObjectsFound, ranging_data.StreamCount);
  
  // CRITICAL: StreamCount rollover detection per ST guide (0-255, then 128-255)
  // Check for missed measurements using StreamCount rollover behavior
  if (this->last_stream_count_ != 0) {  // Skip check on first valid measurement
    uint8_t expected_count;
    
    // Handle the special rollover behavior: 0-255, then 128-255
    if (this->last_stream_count_ == 255) {
      expected_count = 128;  // After 255, it goes to 128
    } else if (this->last_stream_count_ >= 128 && this->last_stream_count_ < 255) {
      expected_count = this->last_stream_count_ + 1;  // Normal increment in 128-255 range
    } else {
      expected_count = this->last_stream_count_ + 1;  // Normal increment in 0-127 range
    }
    
    if (ranging_data.StreamCount != expected_count) {
      // Calculate how many measurements we missed
      uint8_t missed_count;
      if (ranging_data.StreamCount > expected_count) {
        missed_count = ranging_data.StreamCount - expected_count;
      } else {
        // Handle wrap-around case
        if (expected_count <= 127 && ranging_data.StreamCount >= 128) {
          // We crossed the 127->128 boundary
          missed_count = (128 - expected_count) + (ranging_data.StreamCount - 128);
        } else {
          missed_count = (256 - expected_count) + ranging_data.StreamCount;
        }
      }
      
      this->missed_measurements_ += missed_count;
      ESP_LOGW(TAG, "Missed %u measurement(s). Expected StreamCount: %u, Got: %u (Total missed: %u)",
               missed_count, expected_count, ranging_data.StreamCount, this->missed_measurements_);
    }
  }
  this->last_stream_count_ = ranging_data.StreamCount;
  
  // Update all 4 sensor slots - process detected targets and clear undetected ones
  for (uint8_t i = 0; i < 4; i++) {
    if (this->distance_sensors_[i] == nullptr) {
      continue; // Skip unregistered sensors
    }
    
    if (i < ranging_data.NumberOfObjectsFound) {
      // Process detected target
      const VL53LX_TargetRangeData_t* target = &ranging_data.RangeData[i];
      uint16_t distance_mm = target->RangeMilliMeter;
      uint8_t range_status = target->RangeStatus;
      float signal_rate = (float)target->SignalRateRtnMegaCps / 65536.0f;
      float sigma = (float)target->SigmaMilliMeter / 65536.0f;
      
      ESP_LOGD(TAG, "Target %u: Distance=%u mm, Status=%u, Signal=%.2f Mcps, Sigma=%.2f mm, RangeMax=%u, RangeMin=%u",
               i, distance_mm, range_status, signal_rate, sigma, target->RangeMaxMilliMeter, target->RangeMinMilliMeter);
      
      // Process measurement based on range status
      bool should_publish = false;
      float distance_m = distance_mm / 1000.0f;
      
      switch (range_status) {
        case VL53LX_RANGESTATUS_RANGE_VALID:
          // Valid measurement - always publish
          ESP_LOGI(TAG, "✅ Valid measurement: Target %u = %.3f m", i, distance_m);
          should_publish = true;
          break;
          
        case VL53LX_RANGESTATUS_RANGE_VALID_MERGED_PULSE:
          // Multiple targets merged - still useful data
          ESP_LOGW(TAG, "⚠️ Target %u: Multiple targets merged (status=11), Distance=%.3f m", i, distance_m);
          should_publish = true;
          break;
          
        case VL53LX_RANGESTATUS_NONE:
          // No target detected for this slot (valid condition)
          ESP_LOGD(TAG, "🔍 Target %u: No target detected (status=255)", i);
          // Don't publish - sensor will show unavailable
          break;
          
        case VL53LX_RANGESTATUS_SIGNAL_FAIL:
          ESP_LOGW(TAG, "❌ Target %u: Signal too weak (status=2), Signal=%.2f Mcps", i, signal_rate);
          break;
          
        case VL53LX_RANGESTATUS_SIGMA_FAIL:
          ESP_LOGW(TAG, "❌ Target %u: Signal quality too low (status=1), Sigma=%.2f mm", i, sigma);
          break;
          
        case VL53LX_RANGESTATUS_OUTOFBOUNDS_FAIL:
          ESP_LOGW(TAG, "❌ Target %u: Measurement out of bounds (status=4)", i);
          break;
          
        case VL53LX_RANGESTATUS_HARDWARE_FAIL:
          ESP_LOGE(TAG, "❌ Target %u: Hardware/VCSEL failure (status=5)", i);
          break;
          
        case VL53LX_RANGESTATUS_TARGET_PRESENT_LACK_OF_SIGNAL:
          ESP_LOGW(TAG, "❌ Target %u: Target present but signal too weak (status=12)", i);
          break;
          
        case VL53LX_RANGESTATUS_SYNCRONISATION_INT:
          // Synchronization measurement - ignore
          ESP_LOGD(TAG, "🔧 Target %u: Synchronization measurement - ignoring (status=10)", i);
          break;
          
        case VL53LX_RANGESTATUS_RANGE_VALID_NO_WRAP_CHECK_FAIL:
          // This should have been discarded as Range1
          ESP_LOGW(TAG, "⚠️ Target %u: Range1 measurement detected - should have been discarded (status=6)", i);
          break;
          
        case VL53LX_RANGESTATUS_WRAP_TARGET_FAIL:
          ESP_LOGW(TAG, "❌ Target %u: Wraparound error (status=7)", i);
          break;
          
        default:
          ESP_LOGW(TAG, "❌ Target %u: Unknown measurement error, status=%u", i, range_status);
          break;
      }
      
      // Publish valid measurements
      if (should_publish) {
        this->distance_sensors_[i]->publish_distance_state(distance_m);
      }
    } else {
      // No target detected in this slot
      ESP_LOGV(TAG, "Target %u: No data available (only %u targets detected)", i, ranging_data.NumberOfObjectsFound);
      // Don't publish - let Home Assistant show "unavailable" for unused sensors
    }
  }
  
  // Store primary target distance for backward compatibility
  if (ranging_data.NumberOfObjectsFound > 0 && 
      ranging_data.RangeData[0].RangeStatus == VL53LX_RANGESTATUS_RANGE_VALID) {
    this->last_distance_mm_ = ranging_data.RangeData[0].RangeMilliMeter;
    this->last_measurement_time_ = millis();
  }
  
  // Log crosstalk compensation events
  if (ranging_data.HasXtalkValueChanged) {
    ESP_LOGD(TAG, "Crosstalk compensation applied (smudge correction)");
  }
  
  return true;
}

void VL53L3CXComponent::setup_gpio_pins_() {
  if (this->xshut_pin_) {
    this->xshut_pin_->setup();
    this->xshut_pin_->pin_mode(gpio::FLAG_OUTPUT);
    this->xshut_pin_->digital_write(true);
    ESP_LOGD(TAG, "XSHUT pin configured");
  }
  
  if (this->interrupt_pin_) {
    this->interrupt_pin_->setup();
    this->interrupt_pin_->pin_mode(gpio::FLAG_INPUT | gpio::FLAG_PULLUP);
    ESP_LOGD(TAG, "Interrupt pin configured");
  }
}

void VL53L3CXComponent::reset_device_() {
  if (!this->xshut_pin_) {
    return;
  }
  
  ESP_LOGD(TAG, "Resetting VL53L3CX via XSHUT pin");
  this->xshut_pin_->digital_write(false);
  delay(20);
  this->xshut_pin_->digital_write(true);
  delay(10);
}

void VL53L3CXComponent::perform_refspad_calibration() {
  ESP_LOGI(TAG, "Starting RefSPAD calibration...");
  ESP_LOGI(TAG, "IMPORTANT: Ensure no target is in front of sensor during calibration");
  
  if (!this->device_initialized_) {
    ESP_LOGE(TAG, "Cannot perform calibration: device not initialized");
    return;
  }
  
  // Stop measurements
  VL53LX_StopMeasurement(this->device_);
  
  // Perform RefSPAD calibration
  VL53LX_Error status = VL53LX_PerformRefSpadManagement(this->device_);
  
  if (status == VL53LX_ERROR_NONE) {
    ESP_LOGI(TAG, "RefSPAD calibration completed successfully");
  } else if (status == VL53LX_WARNING_REF_SPAD_CHAR_NOT_ENOUGH_SPADS) {
    ESP_LOGW(TAG, "RefSPAD calibration warning: Less than 5 good SPADs available, output not valid");
  } else if (status == VL53LX_WARNING_REF_SPAD_CHAR_RATE_TOO_HIGH) {
    ESP_LOGW(TAG, "RefSPAD calibration warning: Reference rate > 40.0 Mcps, offset stability may be degraded");
  } else if (status == VL53LX_WARNING_REF_SPAD_CHAR_RATE_TOO_LOW) {
    ESP_LOGW(TAG, "RefSPAD calibration warning: Reference rate < 10.0 Mcps, offset stability may be degraded");
  } else {
    ESP_LOGE(TAG, "RefSPAD calibration failed: %d (%s)", status, get_error_string(status));
  }
  
  // Save calibration after successful run
  if (status == VL53LX_ERROR_NONE || status == VL53LX_WARNING_REF_SPAD_CHAR_NOT_ENOUGH_SPADS ||
      status == VL53LX_WARNING_REF_SPAD_CHAR_RATE_TOO_HIGH || status == VL53LX_WARNING_REF_SPAD_CHAR_RATE_TOO_LOW) {
    this->save_calibration_data_();
  }
  
  // Restart measurements
  VL53LX_StartMeasurement(this->device_);
}

void VL53L3CXComponent::perform_crosstalk_calibration() {
  ESP_LOGI(TAG, "Starting Crosstalk calibration...");
  ESP_LOGI(TAG, "IMPORTANT: Place target at exactly 600mm distance in dark environment");
  
  if (!this->device_initialized_) {
    ESP_LOGE(TAG, "Cannot perform calibration: device not initialized");
    return;
  }
  
  // Stop measurements
  VL53LX_StopMeasurement(this->device_);
  
  // Perform Crosstalk calibration
  VL53LX_Error status = VL53LX_PerformXTalkCalibration(this->device_);
  
  if (status == VL53LX_ERROR_NONE) {
    ESP_LOGI(TAG, "Crosstalk calibration completed successfully");
  } else if (status == VL53LX_WARNING_XTALK_MISSING_SAMPLES) {
    ESP_LOGW(TAG, "Crosstalk calibration warning: Missing samples - check setup");
  } else {
    ESP_LOGE(TAG, "Crosstalk calibration failed: %d (%s)", status, get_error_string(status));
  }
  
  if (status == VL53LX_ERROR_NONE || status == VL53LX_WARNING_XTALK_MISSING_SAMPLES) {
    this->save_calibration_data_();
  }
  
  // CRITICAL: Re-enable crosstalk compensation after calibration
  status = VL53LX_SetXTalkCompensationEnable(this->device_, 1);
  if (status != VL53LX_ERROR_NONE) {
    ESP_LOGW(TAG, "Failed to re-enable crosstalk compensation: %d (%s)", status, get_error_string(status));
  }
  
  // Restart measurements
  VL53LX_StartMeasurement(this->device_);
}

void VL53L3CXComponent::perform_offset_calibration() {
  ESP_LOGI(TAG, "Starting Offset calibration...");
  ESP_LOGI(TAG, "IMPORTANT: Place target at known distance with signal rate 2-80 MCPS in dark environment");
  ESP_LOGI(TAG, "Using default target distance of 100mm for zero-distance calibration");
  
  if (!this->device_initialized_) {
    ESP_LOGE(TAG, "Cannot perform calibration: device not initialized");
    return;
  }
  
  // Stop measurements
  VL53LX_StopMeasurement(this->device_);
  
  // Use Per-VCSEL calibration (recommended by ST guide)
  VL53LX_Error status = VL53LX_PerformOffsetPerVcselCalibration(this->device_, 100);
  
  if (status == VL53LX_ERROR_NONE) {
    ESP_LOGI(TAG, "Offset calibration completed successfully");
  } else if (status == VL53LX_WARNING_OFFSET_CAL_SPAD_COUNT_TOO_LOW) {
    ESP_LOGW(TAG, "Offset calibration warning: Signal too low - accuracy may be degraded");
  } else if (status == VL53LX_WARNING_OFFSET_CAL_RATE_TOO_HIGH) {
    ESP_LOGW(TAG, "Offset calibration warning: Signal too high - accuracy may be degraded");
  } else {
    ESP_LOGE(TAG, "Offset calibration failed: %d (%s)", status, get_error_string(status));
  }
  
  // Set offset correction mode to match calibration method
  status = VL53LX_SetOffsetCorrectionMode(this->device_, VL53LX_OFFSETCORRECTIONMODE_PERVCSEL);
  if (status != VL53LX_ERROR_NONE) {
    ESP_LOGW(TAG, "Failed to set offset correction mode: %d (%s)", status, get_error_string(status));
  }
  
  if (status == VL53LX_ERROR_NONE || status == VL53LX_WARNING_OFFSET_CAL_SPAD_COUNT_TOO_LOW ||
      status == VL53LX_WARNING_OFFSET_CAL_RATE_TOO_HIGH) {
    this->save_calibration_data_();
  }
  
  // Restart measurements
  VL53LX_StartMeasurement(this->device_);
}

void VL53L3CXComponent::perform_zero_distance_calibration() {
  ESP_LOGI(TAG, "Starting Zero-Distance calibration (field calibration)...");
  ESP_LOGI(TAG, "IMPORTANT: Place target (e.g. paper) directly touching the cover glass");
  
  if (!this->device_initialized_) {
    ESP_LOGE(TAG, "Cannot perform calibration: device not initialized");
    return;
  }
  
  // Stop measurements
  VL53LX_StopMeasurement(this->device_);
  
  // Perform zero-distance offset calibration (simplified field calibration)
  VL53LX_Error status = VL53LX_PerformOffsetZeroDistanceCalibration(this->device_);
  
  if (status == VL53LX_ERROR_NONE) {
    ESP_LOGI(TAG, "Zero-distance calibration completed successfully");
    
    // Get calibration data for future use
  this->save_calibration_data_();
  } else {
    ESP_LOGE(TAG, "Zero-distance calibration failed: %d (%s)", status, get_error_string(status));
  }
  
  // Restart measurements
  VL53LX_StartMeasurement(this->device_);
}

bool VL53L3CXComponent::load_calibration_data_() {
  if (!global_preferences) {
    return false;
  }
  VL53LX_CalibrationData_t tmp{};
  if (!this->calibration_pref_.load(&tmp)) {
    return false;
  }
  // Basic sanity check: check some fields are non-zero; structure is large, but a simple check helps
  // We'll verify by trying to apply later; here just copy into storage
  this->stored_calibration_data_ = tmp;
  this->calibration_loaded_ = true;
  ESP_LOGI(TAG, "Loaded calibration data from preferences");
  return true;
}

bool VL53L3CXComponent::save_calibration_data_() {
  if (!global_preferences) {
    ESP_LOGW(TAG, "Preferences backend not available; cannot save calibration");
    return false;
  }
  VL53LX_CalibrationData_t tmp{};
  VL53LX_Error st = VL53LX_GetCalibrationData(this->device_, &tmp);
  if (st != VL53LX_ERROR_NONE) {
    ESP_LOGW(TAG, "Failed to read calibration data from device: %d (%s)", st, get_error_string(st));
    return false;
  }
  bool ok = this->calibration_pref_.save(&tmp);
  if (ok) {
    this->stored_calibration_data_ = tmp;
    this->calibration_loaded_ = true;
    ESP_LOGI(TAG, "Calibration data saved to preferences");
    // Best-effort sync
    global_preferences->sync();
  } else {
    ESP_LOGW(TAG, "Failed to save calibration data to preferences");
  }
  return ok;
}

}  // namespace vl53l3cx
}  // namespace esphome