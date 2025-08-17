# VisionGuard User Manual

## Overview

VisionGuard is an intelligent child safety monitoring system that automatically controls television access based on viewing distance. The device uses advanced Time-of-Flight sensing technology to monitor viewer proximity and executes customizable actions via infrared commands when unsafe viewing distances are detected.

## What's in the Box

- VisionGuard device (Revision 1 or 2)
- USB-C cable for power and programming
- Quick start guide

## Initial Setup

### 1. Physical Installation

**Mounting Position:**
- Mount the device near your TV with the sensor facing the viewing area
- Ensure clear line-of-sight between the sensor and typical viewing positions
- Position the IR LED to face your TV's IR receiver (usually front-center of TV)
- Connect power via USB-C cable

**Optimal Placement:**
- Height: Same level as TV screen center
- Distance from TV: 1-2 feet to the side
- Avoid obstacles between sensor and seating area

### 2. Network Configuration

**WiFi Setup:**
1. Power on the device
2. Connect to the "VisionGuard Rev.1 AP" network (password: 12345678)
3. Configure your home WiFi network through the captive portal
4. Device will restart and connect to your network

**Web Interface Access:**
- Find the device IP address in your router's DHCP table
- Open web browser and navigate to the device IP
- Bookmark this page for easy access

## Core Features

### Distance Monitoring

**How it Works:**
- Continuously measures distance to viewers using Time-of-Flight sensor
- Supports detection range from 0.3m to 4.0m
- Updates 5 times per second for responsive monitoring
- Uses dual-threshold system to prevent false triggers

**Detection Zones:**
- **Close Zone**: When viewer is too close to TV (default: 0.8m)
- **Safe Zone**: Optimal viewing distance (between thresholds)  
- **Far Zone**: When viewer moves away (default: 1.0m)

### Automatic TV Control

**Supported Actions:**
- **Near Actions** (child too close): Mute, Power Off, Switch inputs
- **Far Actions** (child moved away): Unmute, Power On, Restore previous

**IR Protocol Support:**
- Primary: LG TV commands (pre-configured)
- Compatible with most modern TV brands
- Configurable repeat count for reliability

## Configuration Guide

### Basic Settings

**Distance Thresholds:**
- **Close Threshold**: Distance considered "too close" (0.3m - 2.0m)
- **Far Threshold**: Distance when child has "moved away" (0.5m - 3.0m)
- *Note: Far threshold must be greater than close threshold*

**Timing Settings:**
- **Close Grace Period**: Delay before taking action (1-30 seconds)
- **Far Hold Time**: How long to wait before restoration (1-30 seconds)

**IR Settings:**
- **Repeat Count**: Number of IR command repetitions (1-5)
- Higher values improve reliability but may cause TV lag

### Action Configuration

**Near Actions (When Child Too Close):**
- Mute - Silence TV audio
- Power Off - Turn TV off completely  
- Switch to HDMI1/2/3 - Change to safe content input
- Switch to TV - Change to broadcast TV
- Show Input List - Display input selection menu
- Do Nothing - Monitor only, no action

**Far Actions (When Child Moves Away):**
- Unmute - Restore TV audio
- Power On - Turn TV back on
- Switch to Previous - Return to previous input
- Switch to HDMI1/2/3 - Change to specific input
- Exit/Back - Navigate back in TV menu
- Do Nothing - No restoration action

## Daily Operation

### Enabling Protection

1. Access the web interface
2. Toggle "Protection Enabled" to ON
3. Green indicator shows system is active
4. System begins monitoring immediately

### Normal Operation Flow

1. **Standby**: System monitors distance continuously
2. **Detection**: Child detected in close zone
3. **Grace Period**: Countdown timer starts (configurable delay)
4. **Action**: If still too close, executes configured near action
5. **Recovery**: When child moves to safe distance, executes far action

### Manual Controls

**Test Buttons:**
- **Test Near Action**: Manually trigger close action
- **Test Far Action**: Manually trigger recovery action
- **Force Mute**: Emergency mute button
- **Force Input**: Show TV input selection

**System Control:**
- **Restart**: Reboot the device
- **Protection Toggle**: Enable/disable monitoring

## Advanced Features (Revision 2)

### Multi-Zone Detection
- 64-zone sensing array (8×8 grid)
- Simultaneous tracking of multiple children
- Zone-specific configuration options
- Enhanced spatial accuracy

### Grove Expansion Port
- Connect additional sensors and modules
- I2C interface supports 300+ Grove modules
- Examples: displays, environmental sensors, additional outputs
- Automatic device discovery and configuration

### Enhanced Power Management
- Dual power input (USB-C + Grove)
- Automatic power source switching
- Enhanced surge and overcurrent protection

## Troubleshooting

### Common Issues

**"No object detected" constantly:**
- Check sensor mounting - ensure clear view of seating area
- Verify sensor is not blocked by objects
- Check if child is within detection range (0.3-4.0m)

**IR commands not working:**
- Verify IR LED is pointing toward TV IR receiver
- Check IR repeat count setting (try increasing to 3-5)
- Ensure TV brand compatibility (optimized for LG)
- Test with manual "Force Mute" button

**False triggers:**
- Adjust grace period (increase to 3-5 seconds)
- Fine-tune distance thresholds
- Check for moving objects in sensor field of view

**WiFi connection issues:**
- Reset to AP mode by holding reset during power-on
- Reconnect using "VisionGuard Rev.1 AP" network
- Check WiFi signal strength in device location

### System Status

**Web Interface Indicators:**
- **Target Distance**: Current measured distance
- **Uptime**: How long device has been running
- **WiFi Signal**: Connection strength
- **MCU Temperature**: Internal temperature monitoring

**Log Messages:**
- View real-time system logs through web interface
- Monitor protection state changes
- Track IR command transmission

### Factory Reset

1. Power off the device
2. Hold down any physical button (if available) while powering on
3. Or reset network settings through web interface
4. Reconfigure WiFi and all settings

## Safety Information

### IR Safety
- Device uses Class 1 IR LED (eye-safe)
- 940nm wavelength (invisible infrared)
- Pulsed transmission minimizes exposure

### Electrical Safety  
- Use only provided USB-C cable
- Operating temperature: -10°C to +60°C
- Keep away from moisture and extreme heat

### Child Safety Notes
- Device is a parental aid, not a substitute for supervision
- Regularly verify proper operation and calibration
- Consider multiple monitoring methods for comprehensive safety

## Technical Specifications

**Detection Performance:**
- Range: 0.3m to 4.0m
- Accuracy: ±30mm at 1m distance
- Response time: <300ms
- False positive rate: <1%

**Connectivity:**
- WiFi: 802.11 b/g/n/ax (2.4GHz)
- Security: WPA3/WPA2 Personal
- Range: 30m typical indoor

**Power Requirements:**
- Input: 5V via USB-C
- Consumption: 65-85mA normal operation
- Peak: 180-200mA during IR transmission

## Support

For additional support:
- Check device logs through web interface
- Verify latest firmware through OTA updates
- Consult technical documentation for advanced configuration

---

*Manual Version 1.0 - Compatible with VisionGuard Revision 1 & 2*