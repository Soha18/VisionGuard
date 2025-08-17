# VisionGuard Technical Documentation - Revision 2

## 1. Project Overview

### 1.1 System Description
VisionGuard Revision 2 represents a significant evolution of the intelligent child safety monitoring platform, incorporating advanced multi-zone Time-of-Flight sensing technology and expandable I/O capabilities. The system maintains its core functionality of automatic television access control while adding enhanced detection precision and modular expansion capabilities for diverse applications.

### 1.2 Design Objectives
- **Enhanced Sensing Platform**: Integration of VL53L5CX multi-zone ToF sensor for superior spatial awareness
- **Modular Expansion**: Grove I2C connector enabling integration with external sensors and actuators
- **Robust Power Management**: Dual power input system with intelligent switching and protection
- **Advanced Framework Integration**: Continued development of STMicroelectronics API adaptation with ESPHome
- **Multi-Application Platform**: Optimized for child safety, robotics navigation, industrial automation, and IoT applications
- **Professional Grade**: Enhanced reliability and diagnostic capabilities for commercial deployment

### 1.3 Key Improvements over Revision 1
- **Sensor Upgrade**: VL53L3CX → VL53L5CX (single zone → 8×8 multi-zone detection)
- **Expandability**: Addition of Grove I2C connector for modular sensor integration
- **Power Management**: Intelligent dual-input power switching with surge protection
- **Enhanced Protection**: Comprehensive ESD and overcurrent protection circuits
- **Diagnostic Features**: Hardware status indicators and advanced fault detection

## 2. System Architecture

### 2.1 Core Components Evolution

**Main Control Unit (MCU)**: ESP32-C6-MINI-1-N4 (U1) - Unchanged
- **RISC-V Architecture**: Proven 32-bit dual-core processor
- **Integrated Connectivity**: Native WiFi 6 and Bluetooth 5.3 LE
- **JTAG Programming**: Native USB-JTAG interface with precision USB design
- **Memory**: 4MB embedded flash with expanded configuration storage requirements
- **GPIO Allocation**: Optimized pin assignment for enhanced I/O requirements

**Advanced Distance Sensing System**: VL53L5CXV0GC/1 (U3) - Major Upgrade
- **Multi-Zone Detection**: 8×8 zone array (64 independent measurements) vs. single zone
- **Enhanced Field of View**: 45° × 45° with spatial resolution capabilities
- **Measurement Range**: 50mm to 4000mm with per-zone accuracy
- **Advanced Features**: Gesture recognition, motion detection, multiple target tracking
- **STM API Integration**: Extended driver supporting full multi-zone capabilities
- **Interrupt System**: Hardware interrupt with zone-specific event generation
- **Power Efficiency**: Programmable zone activation for optimized power consumption

**Intelligent Power Architecture**: Dual-Input System
- **Primary Path**: USB-C with standard 5V delivery
- **Auxiliary Path**: Grove connector enabling system-level power integration
- **Automatic Switching**: TPS2121RUXR (U5) seamless transition between sources
- **Protection Stack**: MAX40200AUK+T (U6) + IP4220CZ6,125 (D5) comprehensive fault protection

**Modular Expansion Interface**: Grove I2C Ecosystem (CN1)
- **Standard Compliance**: Seeed Grove 4-pin connector with I2C protocol
- **Power Delivery**: 5V/1A + 3.3V/500mA for diverse sensor requirements
- **Signal Integrity**: Dedicated I2C bus with proper termination and ESD protection
- **Ecosystem Access**: 300+ Grove modules including sensors, actuators, displays, communication

### 2.2 Design Philosophy and Implementation Strategy

**Spatial Intelligence Implementation**:
The transition to multi-zone sensing required fundamental firmware architecture changes:
- **Memory Architecture**: Efficient handling of 64 simultaneous distance measurements
- **Processing Pipeline**: Real-time spatial analysis with sub-100ms response time
- **Algorithm Development**: Custom zone filtering, object classification, and gesture recognition
- **API Integration**: Extended STM VL53L5CX API adaptation maintaining ESPHome compatibility

**Modular Expansion Strategy**:
Grove integration enables transformation from fixed-function device to adaptable platform:
- **Hardware Abstraction**: Standardized I2C interface supporting diverse modules
- **Software Framework**: Dynamic device discovery and configuration management
- **Power Management**: Intelligent power budgeting for multiple concurrent modules
- **Ecosystem Leverage**: Access to established Grove sensor and actuator library

**Reliability Engineering**:
Enhanced protection and fault tolerance for professional applications:
- **Multi-Stage Protection**: ESD, overcurrent, and surge protection on all external interfaces
- **Fault Detection**: Real-time monitoring with diagnostic reporting capabilities
- **Graceful Degradation**: System maintains core functionality during component failures
- **Power Redundancy**: Automatic failover between power sources without interruption

### 2.3 Hardware Interface Specifications

**VL53L5CX Control Interface**:
- **I2C Address**: 0x29 (7-bit addressing)
- **Control Signals**: 
  - LPN (Low Power): GPIO-controlled power management
  - INT: Interrupt output for measurement completion
  - RST: Hardware reset capability
- **Supply Requirements**: 2.8V I/O, 1.8V analog (internally regulated)
- **Signal Processing**: On-chip histogram analysis and multi-target separation

**Grove Connector Pinout** (CN1):
```
Pin 1: VCC (5V, 1A max, fused)
Pin 2: GND (common ground)
Pin 3: I2C_SDA_EXT (3.3V logic, 2.2kΩ pull-up)
Pin 4: I2C_SCL_EXT (3.3V logic, 2.2kΩ pull-up)
```

**Power Input Specifications**:
- **USB-C**: 5V ±5%, 1.5A maximum with PD negotiation
- **Grove**: 5V ±10%, 1A maximum auxiliary input
- **Switching**: <10ms transition time between sources
- **Efficiency**: >90% power conversion efficiency

## 3. Firmware Architecture and Multi-Zone Integration

**Enhanced Software Framework**: ESPHome with expanded VL53L5CX integration
**Multi-Zone Processing**: Advanced spatial algorithms for 64-zone analysis
**Grove Module Support**: Dynamic I2C device discovery and configuration

### 3.1 VL53L5CX Multi-Zone Implementation

**Unique STM API Integration Enhancement**:
- **64-Zone Data Handling**: Real-time processing of 8×8 distance matrix
- **Spatial Algorithms**: Custom zone filtering and object recognition
- **Memory Optimization**: Efficient handling of multi-zone data structures
- **Interrupt Processing**: Zone-specific event handling and prioritization
- **Configuration Management**: Per-zone parameter adjustment capabilities

**Multi-Zone Configuration Parameters**:
```yaml
VL53L5CX Advanced Settings:
- Resolution Mode: 8×8 (64 zones) or 4×4 (16 zones) selectable
- Detection Range: 50mm to 4000mm per zone
- Update Rate: 1-60Hz with zone-dependent timing
- Target Order: Distance or signal strength prioritization
- Zone Filtering: Configurable ignore masks for specific areas
- Gesture Detection: Motion pattern recognition algorithms
```

### 3.2 Enhanced Control Logic

**Multi-Zone State Machine**:
1. **SPATIAL_MONITORING**: Continuous 64-zone distance measurement
2. **ZONE_ANALYSIS**: Object detection and classification per zone  
3. **GESTURE_RECOGNITION**: Movement pattern analysis
4. **MULTI_TARGET_TRACKING**: Simultaneous object tracking
5. **INTELLIGENT_ACTION**: Context-aware response execution

**Advanced Threshold Management**:
- **Zone-Specific Thresholds**: Individual distance limits per detection zone
- **Spatial Hysteresis**: 2D hysteresis preventing edge oscillations  
- **Gesture Triggers**: Hand wave detection for user interaction
- **Multi-Object Logic**: Handling multiple simultaneous detections
- **Adaptive Sensitivity**: Learning algorithms for environment adaptation

**Grove Module Integration**:
- **Device Auto-Discovery**: I2C scanning and device identification
- **Dynamic Configuration**: Runtime module parameter adjustment
- **Data Fusion**: Combining VL53L5CX data with external sensor inputs
- **Modular Actions**: Triggering external devices via Grove modules

### 3.3 IR Protocol and External Control

**Enhanced IR Protocol Support**:
- **Multi-Device Control**: Simultaneous control of multiple IR devices
- **Zone-Based Actions**: Different actions for different spatial zones
- **Grove IR Modules**: Support for external IR transmitters via Grove
- **Protocol Library**: Expanded support for TV, projector, and AV equipment

## 4. Hardware Design Specifications - Revision 2

### 4.1 Advanced Electrical Design

**Power Consumption Analysis** (Multi-Zone Operation):
| Operating Mode | Current (mA) | Power (mW) | Notes |
|----------------|--------------|------------|-------|
| 64-Zone Active Sensing | 95-120 | 315-400 | VL53L5CX active |
| 16-Zone Reduced Power | 70-85 | 230-280 | 4×4 mode |
| WiFi + Multi-Zone | 150-180 | 500-600 | Peak operation |
| Grove Module Active | +20-50 | +65-165 | Variable per module |
| Deep Sleep | 0.08-0.15 | 0.25-0.5 | Enhanced sleep mode |

**Power Distribution Design**:
- **5V Rail**: Dual-input with TPS2121 automatic switching
- **3.3V Main**: ST1L05APU33R providing 1.5A capability
- **Current Budget**: MCU (80mA) + VL53L5CX (40mA) + Grove (50mA) = 170mA typical
- **Surge Protection**: MAX40200 provides 4A peak current limiting
- **ESD Protection**: IP4220CZ6,125 on all external interfaces

**Signal Integrity Enhancements**:
- **Multi-Zone Data**: High-speed I2C with 64-zone data bandwidth
- **USB Differential Pairs**: Maintained 90Ω ±10% impedance
- **Grove I2C**: Dedicated 33Ω series termination for signal integrity
- **Power Distribution**: Enhanced decoupling with zone-specific filtering
- **Ground Plane**: Improved ground distribution for multi-zone sensor

### 4.2 PCB Layout Evolution

**Enhanced Form Factor Considerations**:
- **Sensor Positioning**: VL53L5CX optimally placed for maximum FOV
- **Grove Connector**: Side-mounted for easy external module connection
- **USB-C Orientation**: Maintained back-side positioning for clean installation
- **Component Density**: Increased component count with optimized placement
- **Thermal Management**: Enhanced thermal design for higher power operation

### 4.3 Bill of Materials - Revision 2 Critical Components

| Designator | Component | Part Number | Function | Rev1 Comparison |
|------------|-----------|-------------|----------|-----------------|
| U1 | ESP32-C6 | ESP32-C6-MINI-1-N4 | Main MCU | Unchanged |
| U3 | Multi-Zone ToF | VL53L5CXV0GC/1 | 64-zone distance sensing | Major upgrade from VL53L3CX |
| U2 | LDO Regulator | ST1L05APU33R | 3.3V regulation | Unchanged |
| U5 | Power Switch | TPS2121RUXR | Dual-input switching | New addition |
| U6 | Current Limiter | MAX40200AUK+T | Overcurrent protection | New addition |
| D5 | ESD Protector | IP4220CZ6,125 | USB/Grove protection | New addition |
| CN1 | Grove Connector | ZX-HY2.0-4PZZ | I2C expansion | New addition |
| Q1 | NPN Transistor | SS8050 | IR LED driver | Unchanged |
| LED1 | IR Emitter | DY-IRS302320C/6.5T | IR transmission | Unchanged |

## 5. Performance Specifications - Revision 2

### 5.1 Multi-Zone Detection Performance
| Parameter | Specification | Improvement over Rev1 |
|-----------|---------------|-----------------------|
| Detection Zones | 64 (8×8) or 16 (4×4) | +6300% spatial resolution |
| Zone Resolution | 5.6° per zone | Individual zone targeting |
| Simultaneous Targets | Up to 64 | Multiple object tracking |
| Gesture Recognition | Hand/body gestures | New capability |
| Update Rate | 1-60Hz per zone | Maintained with 64× data |
| Spatial Accuracy | ±30mm per zone | Maintained across all zones |

### 5.2 Grove Integration Performance
| Parameter | Specification |
|-----------|---------------|
| I2C Bus Speed | Up to 400kHz |
| Module Discovery | <1s auto-detection |
| Concurrent Modules | Up to 8 devices |
| Power Delivery | 5V @ 1A, 3.3V @ 500mA |
| Cable Length | Up to 2m Grove cable |

### 5.3 Enhanced Environmental Specifications
| Parameter | Minimum | Maximum | Unit | Notes |
|-----------|---------|---------|------|-------|
| Operating Temperature | -20 | +70 | °C | Extended range |
| Multi-Zone Operation | -10 | +60 | °C | Full 64-zone capability |
| Humidity | 5 | 95 | % RH | Non-condensing |
| Vibration Resistance | - | 10G | - | Enhanced for mobile robotics |

## 6. Applications and Use Cases - Revision 2

### 6.1 Enhanced Child Safety Applications
- **Multi-Child Detection**: Simultaneous monitoring of multiple children
- **Spatial Awareness**: Zone-specific safety enforcement  
- **Gesture Control**: Hand wave commands for interaction
- **Parent Override**: Gesture-based temporary disable

### 6.2 Robotics and Automation
- **Navigation**: 64-zone obstacle detection for mobile robots
- **Localization**: Spatial landmark detection and mapping
- **Human-Robot Interaction**: Gesture-based robot control
- **Industrial Automation**: Multi-zone safety curtains and access control

### 6.3 IoT and Smart Home Integration
- **Occupancy Detection**: Room-level presence sensing with zone resolution
- **Activity Recognition**: Movement pattern analysis for smart lighting
- **Security Systems**: Intrusion detection with spatial discrimination
- **Energy Management**: Zone-based HVAC control optimization

### 6.4 Grove Ecosystem Integration
- **Environmental Monitoring**: Temperature, humidity, air quality sensors
- **Display Systems**: OLED/LCD status displays via Grove
- **Wireless Communication**: LoRa, Zigbee modules for extended connectivity
- **Actuator Control**: Servo motors, relays, and output devices

## 7. Manufacturing and Quality - Revision 2

### 7.1 Advanced Assembly Requirements
- **Multi-Zone Calibration**: Factory calibration of 64-zone sensor array
- **Grove Interface Testing**: Automated testing of I2C expansion capability
- **Dual Power Testing**: Verification of automatic power switching functionality
- **Spatial Alignment**: Precise sensor mounting for optimal FOV coverage

### 7.2 Enhanced Quality Assurance
- **Multi-Zone Verification**: Per-zone distance accuracy testing
- **Grove Compatibility**: Testing with standard Grove sensor modules
- **Power Management**: Dual-input switching and protection verification
- **Long-term Reliability**: Extended burn-in testing for commercial applications

---

*Document Version: 2.0*  
*Hardware Revision: 2*  
*Supersedes: VisionGuard Revision 1 Technical Documentation*  
*Last Updated: August 2025*  
*Classification: Advanced Technical Design Specification*
