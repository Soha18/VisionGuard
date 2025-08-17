# VisionGuard 👁️📺

An intelligent IoT child safety system that promotes healthy TV viewing habits by automatically monitoring viewing distance and controlling television access through advanced Time-of-Flight sensing technology.

## 🚀 Features

- **Multi-Zone Distance Monitoring**: VL53L5CX 8×8 zone ToF sensor with 64 independent detection areas
- **Intelligent TV Control**: Configurable IR transmission with LG TV optimization and multi-brand support
- **Dual-Threshold Hysteresis**: Prevents false triggers with smart grace period management
- **Grove Ecosystem Integration**: I2C expansion port supporting 300+ sensor modules (Rev 2)
- **ESPHome Framework**: Web interface with real-time configuration and OTA updates
- **WiFi 6 Ready**: ESP32-C6 with native USB-JTAG and Bluetooth 5.3 LE
- **Professional Grade**: Dual power input, ESD protection, and enhanced reliability

## 🛠️ Hardware Specifications

### Core Components
- **MCU**: ESP32-C6-MINI-1-N4 (RISC-V dual-core, 4MB flash)
- **Sensor**: VL53L5CX Time-of-Flight (50mm-4000mm range, 45°×45° FOV)
- **Power**: USB-C 5V with ST1L05APU33R 3.3V LDO (1.5A capacity)
- **IR**: 940nm LED with 100mA drive capability, 3m+ range
- **Expansion**: Grove I2C connector (Rev 2) with 5V/3.3V power delivery
- **Protection**: Comprehensive ESD, surge, and overcurrent protection

### Performance
- **Detection Range**: 0.3m - 4.0m with ±30mm accuracy
- **Response Time**: <300ms from detection to IR output
- **Update Rate**: 1-60Hz configurable, 5Hz default
- **Power Consumption**: 65-85mA normal, 180-200mA peak
- **Operating Temp**: -10°C to +60°C

## 🔧 Quick Start

### Hardware Setup
1. **Programming**: Connect USB-C cable for native JTAG programming
2. **Installation**: Mount device near TV with clear sensor view of seating area
3. **Power**: Use included USB-C cable or Grove power input (Rev 2)

### Firmware Development
For detailed firmware setup and development environment:
- **ESPHome Environment**: See [FIRMWARE/ESPHome/README.md](FIRMWARE/ESPHome/README.md)
- **Docker Setup**: Complete containerized development environment
- **Configuration Guide**: [FIRMWARE/ESPHome/User_Guide_Rev_1.md](FIRMWARE/ESPHome/User_Guide_Rev_1.md)

### End User Setup
For complete installation and usage instructions:
- **User Manual**: [docs/USER_MANUAL.md](DOCS/USER_MANUAL.md)
- **Web Interface**: Access device IP after WiFi configuration
- **Troubleshooting**: Common issues and solutions included in user manual

## 🔬 Technical Overview

### Unique Architecture Features
- **STM API Integration**: Custom ESPHome component wrapping STMicroelectronics VL53L3CX/VL53L5CX drivers
- **Dual-Threshold Hysteresis**: Intelligent state management prevents false triggers
- **Multi-Target Processing**: Advanced histogram analysis and spatial filtering
- **Hardware Interrupt Integration**: Low-latency sensor response with ESPHome framework

### Development Stack
- **Framework**: ESPHome with ESP-IDF backend
- **Programming**: Native USB-JTAG via ESP32-C6
- **API**: Custom sensor components and IR protocol libraries
- **Power Management**: Intelligent dual-input switching (Rev 2)

**For complete technical specifications**: See [Technical Documentation](DOCS/technical_doc_rev2.md)

## 🏗️ Hardware Revisions

### Revision 1
- Single-zone VL53L3CX sensor
- USB-C power and programming
- Compact 1.5×5cm form factor
- Production ready design

### Revision 2
- Multi-zone VL53L5CX sensor (8×8 array)
- Grove I2C expansion connector
- Enhanced power management
- Extended environmental specifications

## 📊 Applications

- **Child Safety**: Primary use case for healthy viewing habits
- **Robotics**: Navigation and obstacle avoidance systems  
- **Industrial**: Safety curtains and access control
- **IoT/Smart Home**: Occupancy detection and automation
- **Security**: Spatial intrusion detection systems

### Getting Started
- **Development Environment**: [FIRMWARE/ESPHome/README.md](FIRMWARE/ESPHome/README.md)
- **Technical Architecture**: [docs/TECHNICAL.md](DOCS/technical_doc_rev2.md)
- **Hardware Design**: See [hardware/](hardware/) directory

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---
**Version**: 2.0 | **Hardware**: Rev 1 Production, Rev 2 Development | **Status**: Active Development | **Updated**: August 2025
