# VisionGuard Technical Documentation - Revision 1

## 1. Project Overview

### 1.1 System Description
VisionGuard is an intelligent child safety monitoring system designed to automatically control television access based on viewing distance. The device employs Time-of-Flight (ToF) sensing technology to continuously monitor viewer proximity and executes predefined actions via infrared transmission when unsafe viewing distances are detected.

### 1.2 Design Objectives
- **Primary Function**: Proof of concept for integrating VL53L3CX ToF sensor with ESP32-C6 architecture and implementation of STMicroelectronics API adaptation with ESPHome/ESP-IDF framework
- **Child Safety Application**: Automatic enforcement of safe TV viewing distances as primary use case
- **Versatile Platform**: Expandable architecture for robotics applications (exploration robots, localization systems), gate control systems, and industrial proximity monitoring
- **Smart Integration**: WiFi connectivity and web interface for IoT applications
- **Configurable Response**: User-defined thresholds and behavioral actions

## 2. System Architecture

### 2.1 Core Components Selection Strategy

**Main Control Unit (MCU)**: ESP32-C6-MINI-1-N4 (U2)
- **RISC-V Architecture**: Modern 32-bit processor with dual-core capability
- **Integrated Connectivity**: Native WiFi 6 (802.11ax) and Bluetooth 5.3 LE
- **JTAG Programming**: Native USB-JTAG interface requiring precise USB differential pair design
- **Memory**: 4MB embedded flash for application code and configuration storage

**Distance Sensing System**: VL53L3CXV0DH/1 (U1) - STMicroelectronics Time-of-Flight Sensor
- **Measurement Range**: 50mm to 4000mm operational range covers typical applications
- **Accuracy**: ±3% precision at 1-2m distances (optimal for most use cases)
- **I2C Interface**: Standard communication protocol with configurable address (0x29)
- **STM API Adaptation**: Custom driver integration with ESPHome framework - unique implementation
- **Interrupt Capability**: Hardware interrupt on measurement completion reduces polling overhead

**Power Management Architecture**: ST1L05APU33R (U3) Low-Dropout Regulator
- **Input Range**: 5V USB-C input with comprehensive surge protection
- **Output**: Stable 3.3V supply rated at 1.5A maximum current
- **Voltage Drop Calculation**: 200mV typical dropout ensures 3.1V minimum at 300mA load
- **Current Budget**: MCU (80mA) + Sensor (20mA) + IR LED (100mA peak) = 200mA minimum
- **Thermal Protection**: Integrated over-temperature shutdown with thermal pad
- **Package**: SOT-323 surface mount optimized for 1.5×5cm PCB constraint

**Infrared Transmission System**: DY-IRS302320C/6.5T (U5) with SS8050 Driver (Q1)
- **Wavelength**: 940nm optimized for consumer electronics IR receivers
- **Drive Current Calculation**: 100mA peak through 220Ω base resistor with β=200 transistor gain
- **Power Dissipation**: 330mW peak (100mA × 3.3V) during transmission bursts
- **Protocol Flexibility**: Configurable IR protocols with LG TV as default tested implementation
- **Range**: >3m transmission distance for typical applications

### 2.2 Critical Design Considerations

**USB-C Interface Design**: TYPE-C 16PLT-H10.0 (U4)
The ESP32-C6 uses native USB-JTAG (not UART bridge) requiring precise differential pair design:
- **Impedance Control**: 90Ω ±10% differential impedance maintained across PCB traces
- **Trace Length**: D+/D- matched within ±0.1mm, total length <50mm for signal integrity
- **Capacitive Loading**: 4.7µF bulk capacitors positioned <5mm from connector
- **CC Resistors**: 5.1kΩ precision resistors for USB-C power negotiation
- **Current Limiting**: MF-MSMF110/16-2 resettable fuse rated for inrush protection

**PCB Layout Architecture**: 1.5×5cm Form Factor
- **Orientation Strategy**: USB-C connector on back side, sensor and IR LED on front side
- **Installation Advantage**: Front-facing sensors allow direct TV screen mounting
- **Component Density**: High-density layout with 0402 passive components
- **Ground Plane**: Continuous ground pour with strategic via stitching
- **Power Distribution**: Star topology from LDO to minimize voltage drops
- **Bulk Capacitance**: 22µF tantalum + 4.7µF ceramic near USB connector for inrush current limiting
- **Thermal Management**: Thermal vias under power components for heat dissipation

**I2C Bus Implementation**:
- **Clock Frequency**: 38kHz optimized for VL53L3CX sensor timing requirements
- **Pull-up Resistors**: 2.2kΩ (R4, R5) calculated for 3.3V logic with 400pF bus capacitance
- **GPIO Assignment**: SDA=GPIO6, SCL=GPIO7 with internal weak pull-ups disabled
- **Signal Integrity**: <10pF parasitic capacitance maintained through careful routing

## 3. Firmware Architecture and STM API Integration

**Software Framework**: ESPHome with ESP-IDF backend and custom STM API adaptation
**Real-Time OS**: FreeRTOS kernel for task scheduling
**Development Framework**: YAML configuration with C++ lambda extensions

### 3.1 Unique STM API Adaptation
This project features a novel integration of STMicroelectronics VL53L3CX API with the ESPHome framework:
- **Custom Component**: Native ESPHome component wrapping STM's official API
- **Memory Management**: Adaptation of STM's dynamic allocation to ESP-IDF heap management
- **Interrupt Handling**: Integration of STM interrupt callbacks with ESPHome's GPIO system
- **Configuration Bridge**: YAML parameter mapping to STM's configuration structures
- **Multi-Target Support**: Full exposure of STM's advanced ranging capabilities

### 3.2 Sensor Management System

**VL53L3CX Driver Configuration**:
- Distance Mode: MEDIUM (up to 2.90m range)
- Timing Budget: 80ms (accuracy vs. speed balance)
- Update Interval: 200ms (system responsiveness)
- Signal Processing: Exponential moving average (α=0.1)

**Multi-Target Processing Algorithm**: Custom histogram analysis with STM API integration
- Merge Threshold: 12000 units (target separation)
- Noise Threshold: 50 units (weak signal rejection)
- Signal Rate Limit: 0.1 MCPS (minimum acceptable return)
- Sigma Threshold: 60mm (measurement uncertainty limit)

### 3.3 Control Logic Implementation

**State Machine Architecture**:
1. MONITORING: Continuous distance measurement
2. GRACE_PERIOD: Countdown before action execution  
3. ACTION_ACTIVE: IR command transmission
4. RECOVERY_HOLD: Waiting for distance recovery

**Dual-Threshold Hysteresis System**:
- Close Threshold: 0.8m default (user configurable 0.3-2.0m)
- Far Threshold: 1.0m default (user configurable 0.5-3.0m)
- Dead Band: Prevents oscillation between states
- Grace Period: 2s default delay before action execution

**Action Execution Framework**:
- Near Actions (child too close): Mute, Power Off, Input switching, Navigation commands
- Far Actions (child moved away): Unmute, Power On, Input restoration
- IR repeat count: 3 transmissions with 50ms intervals
- Command verification through state tracking

### 3.4 IR Protocol Implementation

**Configurable Protocol Support**: The system supports multiple IR protocols with LG TV as the default tested configuration:
- **Primary Protocol**: NEC encoding with LG address prefix (0x20DF)
- **Alternative Protocols**: Samsung, Sony, Panasonic (configurable via firmware)
- **Carrier Frequency**: 38kHz with 50% duty cycle
- **Bit Encoding**: PWM with protocol-specific timing requirements

**LG TV Command Set (Default Configuration)**:
```
Verified Commands:
- Power: 0x20DF10EF
- Mute: 0x20DF906F  
- Input: 0x20DFD02F
- HDMI1: 0x20DF738C
- HDMI2: 0x20DF33CC
- HDMI3: 0x20DF9768
```

**Transmission Strategy**:
- Multiple transmission repeats (user configurable 1-5)
- Inter-command delays (50ms) prevent receiver overflow
- Queue management for sequential commands
- Error recovery through state machine reset

## 4. Hardware Design Specifications

### 4.1 Electrical Design and Power Analysis

**Power Consumption Analysis**:
| Operating Mode | Current (mA) | Power (mW) | Duration |
|----------------|--------------|------------|----------|
| Active Sensing | 65-85 | 215-280 | Continuous |
| WiFi Transmission | 120-150 | 400-500 | Burst |
| IR Transmission | 180-200 | 600-660 | 100ms |
| Deep Sleep | 0.05-0.1 | 0.15-0.3 | Standby |

**LDO Design Calculations**:
- Input Voltage: 5.0V (USB-C nominal)
- Output Voltage: 3.3V regulated
- Maximum Load Current: 200mA (continuous), 300mA (peak)
- Voltage Drop: 1.7V (5.0V - 3.3V)
- Power Dissipation: 340mW @ 200mA load
- Thermal Resistance: 120°C/W (SOT-323 package)
- Temperature Rise: 41°C above ambient (acceptable for ≤45°C ambient)

**Signal Integrity Requirements**:
- I2C Bus: ±5% tolerance, <10pF parasitic capacitance
- USB Differential Pairs: 90Ω ±10% impedance, ±0.1mm length matching
- IR Drive Circuit: Fast rise/fall times (<1µs) for 38kHz modulation
- Power Rails: <50mV ripple at full load with bulk capacitor filtering

### 4.3 Bill of Materials - Critical Components

| Designator | Component | Part Number | Function | Selection Criteria |
|------------|-----------|-------------|----------|--------------------|
| U2 | ESP32-C6 | ESP32-C6-MINI-1-N4 | Main MCU | RISC-V architecture, native USB-JTAG, WiFi 6 |
| U1 | ToF Sensor | VL53L3CXV0DH/1 | Distance measurement | 4m range, I2C interface, STM API support |
| U3 | LDO Regulator | ST1L05APU33R | 3.3V regulation | 1.5A rating, low dropout, thermal protection |
| U5 | IR LED | DY-IRS302320C/6.5T | IR transmission | 940nm wavelength, 100mA drive capability |
| Q1 | NPN Transistor | SS8050 | IR LED driver | β=200 gain, fast switching, 100mA collector |
| U4 | USB-C Connector | TYPE-C 16PLT-H10.0 | Power/JTAG interface | USB-C standard, robust mounting |

## 5. Programming and Development Interface

**Firmware Flashing Procedure**:
- **Interface**: USB-C connector with native ESP32-C6 USB-JTAG support
- **Protocol**: JTAG debugging protocol (not UART bridge)
- **Tools**: ESPHome command line, ESP-IDF, or OpenOCD
- **Flash Process**: Direct JTAG programming via USB-C connection
  1. Connect USB-C cable to development host
  2. Auto-detection via USB-JTAG bridge
  3. Execute: `esphome run vision-guard-rev-1.yml`
  4. Monitor via serial console or JTAG debugger

## 6. Performance Specifications

### 6.1 Detection Performance
| Parameter | Specification | Test Condition |
|-----------|---------------|----------------|
| Detection Range | 0.3m - 4.0m | Standard reflective target |
| Accuracy | ±30mm | At 1m distance |
| Response Time | <300ms | From detection to IR output |
| False Positive Rate | <1% | 8-hour continuous operation |
| Update Rate | 5 Hz | Configurable 1-60 Hz |

### 6.2 Environmental Specifications
| Parameter | Minimum | Maximum | Unit |
|-----------|---------|---------|------|
| Operating Temperature | -10 | +60 | °C |
| Storage Temperature | -40 | +85 | °C |
| Relative Humidity | 10 | 90 | % RH |
| Altitude | 0 | 3000 | meters |

### 6.3 Network Performance
| Parameter | Specification |
|-----------|---------------|
| WiFi Standards | 802.11 b/g/n/ax (2.4GHz) |
| Security | WPA3-Personal, WPA2-Personal |
| Range | 30m typical indoor |
| Connection Time | <10s from power-on |
| OTA Update Size | <2MB typical |

## 7. Safety and Compliance

### 7.1 Electrical Safety
- **Input Protection**: TVS diode for surge protection
- **Current Limiting**: Resettable fuse prevents overcurrent
- **Isolation**: No galvanic connection between USB and IR circuits
- **EMC**: Design practices for FCC Part 15 Class B compliance

### 7.2 IR Safety
- **Eye Safety**: IR LED power limited to IEC 62471 Class 1
- **Exposure Duration**: Pulse-based transmission minimizes continuous exposure
- **Wavelength**: 940nm outside visible spectrum

---

*Document Version: 1.0*  
*Hardware Revision: 1*  
*Firmware Version: ESPHome compatible*  
*Last Updated: August 2025*  
*Classification: Technical Design Specification*
