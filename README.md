# VisionGuard 👁️📺

An IoT device that promotes healthy TV viewing habits for children by automatically monitoring viewing distance and controlling TV access.

## 🚀 Features

- **Smart Distance Monitoring**: 8x8 zone ToF sensor detects viewer proximity
- **Automatic TV Control**: IR transmission switches TV input when too close
- **Matter/Thread Compatible**: Seamless smart home integration
- **WiFi/Bluetooth**: Wireless configuration and updates
- **Expandable**: I2C interface for additional sensors
- **USB-C Powered**: Simple plug-and-play setup

## 🛠️ Hardware

- **MCU**: ESP32-C6 (WiFi 6, Bluetooth 5, Matter support)
- **Sensor**: VL53L5CX Time-of-Flight (4m range, 64 zones)
- **Power**: USB-C 5V input with 3.3V LDO regulation
- **Expansion**: I2C level translator for 5V sensor modules
- **Form Factor**: Compact design for TV bezel mounting

## 🔧 Quick Start

1. **Hardware**: Flash firmware via USB-C connection
2. **Configuration**: Use ESPHome dashboard or mobile app
3. **Installation**: Mount device on TV bezel with clear sensor view
4. **Smart Home**: Commission via Matter QR code

## ⚙️ Configuration

- **Safe Distance**: 50-500cm (default: 200cm)
- **Warning Time**: 5-60s before action
- **TV Codes**: Support for major manufacturers
- **Zones**: Configurable 1-64 detection areas

## 🤝 Contributing

Pull requests welcome! Please read [CONTRIBUTING.md](CONTRIBUTING.md) first.

---
**Version**: 1.0 | **Date**: July 2025 | **Status**: Production Ready
