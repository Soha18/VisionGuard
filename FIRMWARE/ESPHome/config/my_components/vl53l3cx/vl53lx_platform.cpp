// Platform implementation for VL53LX library that uses ESPHome's I2C
// This replaces the ST library's platform layer

#include "vl53l3cx.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include <algorithm>
#include <vector>
#include <cstring>

// Include ST library headers
extern "C" {
#include "vl53lx_api.h"
#include "vl53lx_platform.h"
}

static const char *const TAG = "vl53l3cx.platform";

// Global pointer to current component for platform functions
static esphome::vl53l3cx::VL53L3CXComponent *g_component = nullptr;

void vl53lx_set_i2c_component(esphome::vl53l3cx::VL53L3CXComponent *component) {
  g_component = component;
}

extern "C" {

// I2C Read implementation using ESPHome's I2C
VL53LX_Error VL53LX_ReadMulti(VL53LX_DEV Dev, uint16_t index, uint8_t *pdata, uint32_t count) {
  if (g_component == nullptr) {
    ESP_LOGE(TAG, "Platform component not set!");
    return VL53LX_ERROR_CONTROL_INTERFACE;
  }

  const uint32_t MAX_CHUNK_SIZE = 32;  // Conservative chunk size
  uint32_t remaining = count;
  uint32_t offset = 0;
  while (remaining > 0) {
    uint32_t chunk = std::min(MAX_CHUNK_SIZE, remaining);

    // Compute current address
    uint16_t cur_index = index + offset;
    uint8_t addr_bytes[2];
    addr_bytes[0] = (cur_index >> 8) & 0xFF;
    addr_bytes[1] = cur_index & 0xFF;

    // Write register address for this chunk
    if (g_component->write(addr_bytes, 2) != esphome::i2c::ERROR_OK) {
      ESP_LOGD(TAG, "Failed to write register address 0x%04X", cur_index);
      return VL53LX_ERROR_CONTROL_INTERFACE;
    }

    // Read chunk
    if (g_component->read(pdata + offset, chunk) != esphome::i2c::ERROR_OK) {
      ESP_LOGD(TAG, "Failed to read %u bytes from 0x%04X", chunk, cur_index);
      return VL53LX_ERROR_CONTROL_INTERFACE;
    }

    remaining -= chunk;
    offset += chunk;

    // Small delay between chunks
    esphome::delay(1);
  }

  ESP_LOGVV(TAG, "Read %u bytes from 0x%04X", count, index);
  return VL53LX_ERROR_NONE;
}

// I2C Write implementation using ESPHome's I2C
VL53LX_Error VL53LX_WriteMulti(VL53LX_DEV Dev, uint16_t index, uint8_t *pdata, uint32_t count) {
  if (g_component == nullptr) {
    ESP_LOGE(TAG, "Platform component not set!");
    return VL53LX_ERROR_CONTROL_INTERFACE;
  }

  // ESPHome I2C may have transaction size limits, so chunk large writes
  const uint32_t MAX_CHUNK_SIZE = 32;  // Conservative chunk size
  
  if (count <= MAX_CHUNK_SIZE) {
    // Single write for small data
    std::vector<uint8_t> buffer(2 + count);
    buffer[0] = (index >> 8) & 0xFF;
    buffer[1] = index & 0xFF;
    std::memcpy(&buffer[2], pdata, count);

    if (g_component->write(buffer.data(), buffer.size()) != esphome::i2c::ERROR_OK) {
      ESP_LOGD(TAG, "Failed to write %u bytes to 0x%04X", count, index);
      return VL53LX_ERROR_CONTROL_INTERFACE;
    }
  } else {
    // Chunked writes for large data
    ESP_LOGD(TAG, "Chunking large write: %u bytes to 0x%04X", count, index);
    
    for (uint32_t offset = 0; offset < count; offset += MAX_CHUNK_SIZE) {
      uint32_t chunk_size = std::min(MAX_CHUNK_SIZE, count - offset);
      uint16_t chunk_addr = index + offset;
      
      std::vector<uint8_t> buffer(2 + chunk_size);
      buffer[0] = (chunk_addr >> 8) & 0xFF;
      buffer[1] = chunk_addr & 0xFF;
      std::memcpy(&buffer[2], pdata + offset, chunk_size);

      if (g_component->write(buffer.data(), buffer.size()) != esphome::i2c::ERROR_OK) {
        ESP_LOGD(TAG, "Failed to write chunk %u bytes to 0x%04X (offset %u)", 
                 chunk_size, chunk_addr, offset);
        return VL53LX_ERROR_CONTROL_INTERFACE;
      }
      
      // Small delay between chunks
      esphome::delay(1);
    }
  }

  ESP_LOGVV(TAG, "Wrote %u bytes to 0x%04X", count, index);
  return VL53LX_ERROR_NONE;
}

// Byte read/write helpers
VL53LX_Error VL53LX_RdByte(VL53LX_DEV Dev, uint16_t index, uint8_t *pdata) {
  return VL53LX_ReadMulti(Dev, index, pdata, 1);
}

VL53LX_Error VL53LX_WrByte(VL53LX_DEV Dev, uint16_t index, uint8_t data) {
  return VL53LX_WriteMulti(Dev, index, &data, 1);
}

// Word read/write helpers (16-bit, big-endian)
VL53LX_Error VL53LX_RdWord(VL53LX_DEV Dev, uint16_t index, uint16_t *pdata) {
  uint8_t buffer[2];
  VL53LX_Error status = VL53LX_ReadMulti(Dev, index, buffer, 2);
  if (status == VL53LX_ERROR_NONE) {
    *pdata = ((uint16_t)buffer[0] << 8) | buffer[1];
  }
  return status;
}

VL53LX_Error VL53LX_WrWord(VL53LX_DEV Dev, uint16_t index, uint16_t data) {
  uint8_t buffer[2];
  buffer[0] = (data >> 8) & 0xFF;
  buffer[1] = data & 0xFF;
  return VL53LX_WriteMulti(Dev, index, buffer, 2);
}

// DWord read/write helpers (32-bit, big-endian)
VL53LX_Error VL53LX_RdDWord(VL53LX_DEV Dev, uint16_t index, uint32_t *pdata) {
  uint8_t buffer[4];
  VL53LX_Error status = VL53LX_ReadMulti(Dev, index, buffer, 4);
  if (status == VL53LX_ERROR_NONE) {
    *pdata = ((uint32_t)buffer[0] << 24) | ((uint32_t)buffer[1] << 16) |
             ((uint32_t)buffer[2] << 8) | buffer[3];
  }
  return status;
}

VL53LX_Error VL53LX_WrDWord(VL53LX_DEV Dev, uint16_t index, uint32_t data) {
  uint8_t buffer[4];
  buffer[0] = (data >> 24) & 0xFF;
  buffer[1] = (data >> 16) & 0xFF;
  buffer[2] = (data >> 8) & 0xFF;
  buffer[3] = data & 0xFF;
  return VL53LX_WriteMulti(Dev, index, buffer, 4);
}

// Timing functions
VL53LX_Error VL53LX_WaitMs(VL53LX_DEV Dev, int32_t wait_ms) {
  if (wait_ms > 0) {
    esphome::delay(wait_ms);
  }
  return VL53LX_ERROR_NONE;
}

VL53LX_Error VL53LX_WaitUs(VL53LX_DEV Dev, int32_t wait_us) {
  if (wait_us > 0) {
    esphome::delayMicroseconds(wait_us);
  }
  return VL53LX_ERROR_NONE;
}

VL53LX_Error VL53LX_GetTickCount(VL53LX_DEV Dev, uint32_t *ptick_count_ms) {
  *ptick_count_ms = esphome::millis();
  return VL53LX_ERROR_NONE;
}

// Timer functions (not used by core, but required by API)
VL53LX_Error VL53LX_GetTimerFrequency(int32_t *ptimer_freq_hz) {
  *ptimer_freq_hz = 1000;  // 1kHz (millisecond timer)
  return VL53LX_ERROR_NONE;
}

VL53LX_Error VL53LX_GetTimerValue(int32_t *ptimer_count) {
  *ptimer_count = esphome::millis();
  return VL53LX_ERROR_NONE;
}

// GPIO functions - mostly no-ops as ESPHome handles GPIO
VL53LX_Error VL53LX_GpioSetMode(uint8_t pin, uint8_t mode) {
  return VL53LX_ERROR_NONE;
}

VL53LX_Error VL53LX_GpioSetValue(uint8_t pin, uint8_t value) {
  return VL53LX_ERROR_NONE;
}

VL53LX_Error VL53LX_GpioGetValue(uint8_t pin, uint8_t *pvalue) {
  *pvalue = 1;
  return VL53LX_ERROR_NONE;
}

VL53LX_Error VL53LX_GpioXshutdown(uint8_t value) {
  return VL53LX_ERROR_NONE;
}

VL53LX_Error VL53LX_GpioCommsSelect(uint8_t value) {
  return VL53LX_ERROR_NONE;
}

VL53LX_Error VL53LX_GpioPowerEnable(uint8_t value) {
  return VL53LX_ERROR_NONE;
}

VL53LX_Error VL53LX_GpioInterruptEnable(void (*function)(void), uint8_t edge_type) {
  return VL53LX_ERROR_NONE;
}

VL53LX_Error VL53LX_GpioInterruptDisable(void) {
  return VL53LX_ERROR_NONE;
}

// I2C initialization - ESPHome handles this
VL53LX_Error VL53LX_CommsInitialise(VL53LX_Dev_t *pdev, uint8_t comms_type, uint16_t comms_speed_khz) {
  // ESPHome already initialized I2C, just return success
  return VL53LX_ERROR_NONE;
}

VL53LX_Error VL53LX_CommsClose(VL53LX_Dev_t *pdev) {
  // ESPHome handles I2C cleanup
  return VL53LX_ERROR_NONE;
}

// Wait for value with mask (polling)
VL53LX_Error VL53LX_WaitValueMaskEx(
    VL53LX_DEV Dev,
    uint32_t timeout_ms,
    uint16_t index,
    uint8_t value,
    uint8_t mask,
    uint32_t poll_delay_ms) {
  
  uint32_t start_time_ms = esphome::millis();
  uint8_t byte_value;
  
  while ((esphome::millis() - start_time_ms) < timeout_ms) {
    VL53LX_Error status = VL53LX_RdByte(Dev, index, &byte_value);
    if (status != VL53LX_ERROR_NONE) {
      return status;
    }
    
    if ((byte_value & mask) == value) {
      return VL53LX_ERROR_NONE;
    }
    
    esphome::delay(poll_delay_ms);
  }
  
  return VL53LX_ERROR_TIME_OUT;
}

} // extern "C"