# VisionGuard User Manual
## Eye Protection Device for Safe TV Viewing

**Model:** VisionGuard-V1  
**Version:** 1.0  
**Date:** July 2025

---

## Table of Contents

1. [Welcome & Safety Information](#welcome--safety-information)
2. [What's in the Box](#whats-in-the-box)
3. [Product Overview](#product-overview)
4. [Getting Started](#getting-started)
5. [Installation Guide](#installation-guide)
6. [Initial Setup](#initial-setup)
7. [Web Interface Guide](#web-interface-guide)
8. [Configuration Settings](#configuration-settings)
9. [Daily Operation](#daily-operation)
10. [Troubleshooting](#troubleshooting)
11. [Maintenance & Care](#maintenance--care)
12. [Technical Specifications](#technical-specifications)
13. [Warranty Information](#warranty-information)
14. [Support & Contact](#support--contact)

---

## Welcome & Safety Information

### Welcome to VisionGuard

Thank you for choosing VisionGuard, the smart eye protection device designed to promote healthy TV viewing habits for your family. VisionGuard uses advanced sensor technology to automatically monitor viewing distance and help maintain safe screen time practices.

### Important Safety Instructions

**ELECTRICAL SAFETY:**
- Use only the provided USB-C cable and a certified 5V power adapter
- Do not expose the device to water or excessive moisture
- Unplug the device during thunderstorms or when not in use for extended periods
- Do not attempt to repair or modify the device - this will void the warranty

**INSTALLATION SAFETY:**
- Ensure secure mounting to prevent the device from falling
- Keep mounting area clean and dry before installation

**OPERATIONAL SAFETY:**
- The device is designed for indoor use only
- Operating temperature: -10°C to +60°C (14°F to 140°F)
- Do not cover the sensor area - this may cause malfunction
- Keep the device away from direct sunlight and heat sources

**HEALTH CONSIDERATIONS:**
- VisionGuard is a wellness device, not a medical device
- Consult an eye care professional for vision-related concerns
- The device supports healthy habits but does not replace parental supervision
- Children should take regular breaks from screen viewing regardless of distance

---

## What's in the Box

Please verify that all items are included:

### Package Contents
- **1x VisionGuard Device**
- **1x USB-C Power Cable** (1.5m/5ft)
- **1x Mounting Kit** including:
  - 3x Adhesive mounting strips (removable)
  - 2x Magnetic mounts (for metal TV bezels)
  - 1x Cable management clips
- **1x Quick Start Guide**
- **1x User Manual** (this document)

### Additional Requirements (Not Included)
- **USB Power Adapter** (5V, minimum 1A) - standard phone charger
- **WiFi Network** (2.4GHz required)
- **Compatible TV** with infrared remote receiver
- **Web Browser** (Chrome, Firefox, Safari, or Edge)

---

## Product Overview

### What is VisionGuard?

VisionGuard is an intelligent eye protection device that promotes safe TV viewing habits by:

- **Monitoring viewing distance** using advanced sensor technology
- **Automatically controlling your TV** when viewers are too close
- **Providing gentle reminders** to maintain healthy viewing distances
- **Learning your family's patterns** to provide personalized protection
- **Integrating with smart homes** through web-based controls

### How It Works

**The Science Behind VisionGuard:**

1. **Advanced Sensing**: Uses Time-of-Flight (ToF) technology to create a detailed 8x8 zone map of the viewing area
2. **Intelligent Analysis**: Processes distance data in real-time to detect when viewers are too close to the screen
3. **Gentle Intervention**: Provides audio-visual warnings before taking action
4. **Smart TV Control**: Temporarily changes TV input or volume when safe distance isn't maintained
5. **Adaptive Learning**: Adjusts behavior based on your family's viewing patterns and preferences

### Key Features

**🎯 Multi-Zone Distance Monitoring**
- 64-zone sensor array provides precise distance measurement
- Detects multiple viewers simultaneously
- Range: 10cm to 4 meters with ±3% accuracy

**📺 Universal TV Compatibility**
- Works with virtually all TV brands and models
- Supports major IR protocols (NEC, Sony, Samsung, LG, etc.)
- No additional hardware required for most TVs

**🌐 Web-Based Control**
- No app installation required - works in any web browser
- Real-time monitoring and configuration
- Responsive design works on phones, tablets, and computers

**🏠 Smart Home Ready**
- Built on ESPHome platform for easy integration
- Compatible with Home Assistant and other automation systems
- Local processing - no cloud dependency

**⚡ Easy Installation**
- Mounts to TV bezel in minutes
- Single USB-C power connection
- Automatic WiFi setup process

**👨‍👩‍👧‍👦 Family-Friendly Design**
- Age-appropriate distance settings
- Customizable warning times and actions
- Parental control access protection

---

## Getting Started

### Before You Begin

**Check Your TV Compatibility:**
VisionGuard works with most modern TVs. Your TV is compatible if it:
- Responds to an infrared remote control
- Has space on the bezel for a 50mm x 16mm device
- Is not behind glass or in an enclosed cabinet

**Prepare Your Network:**
- Ensure you have a 2.4GHz WiFi network available
- Have your WiFi password ready
- Verify internet connectivity for initial setup

**Choose Installation Location:**
- Find a spot on your TV bezel where the sensor has a clear view of the seating area
- Ensure the location allows easy access to the USB-C port for power
- Avoid locations that might obstruct TV ventilation

### System Requirements

**WiFi Network:**
- 2.4GHz WiFi (802.11b/g/n/ax)
- WPA2 or WPA3 security (recommended)
- Internet connection for initial setup and updates

**Power Requirements:**
- USB-C connection
- Standard phone charger or TV USB port

**Compatible Devices for Configuration:**
- Smartphone, tablet, or computer with web browser
- Modern browsers: Chrome 90+, Firefox 88+, Safari 14+, Edge 90+
- JavaScript enabled

---

## Installation Guide

### Step 1: Choose Mounting Location

**Optimal Placement:**
- **Top center** of TV bezel (most common and effective)
- **Side edges** if top mounting isn't possible
- **Bottom center** as last resort (may have viewing angle limitations)

**Mounting Requirements:**
- Clear line of sight to seating area
- At least 5cm clearance from TV vents
- Accessible for USB-C cable connection
- Stable surface that won't flex

### Step 2: Prepare the Surface

**For Adhesive Mounting:**
1. Clean the mounting area with isopropyl alcohol
2. Allow surface to dry completely (5-10 minutes)
3. Ensure surface temperature is between 15-40°C (59-104°F)

**For Magnetic Mounting:**
1. Verify TV bezel is magnetic (test with included magnet)
2. Clean the area with a dry cloth
3. Remove any protective films or stickers

### Step 3: Mount the Device

**Adhesive Installation:**
1. Remove backing from adhesive strip
2. Position VisionGuard with sensor facing the seating area
3. Press firmly for 30 seconds
4. Wait 24 hours before handling for maximum adhesion

**Important:** The sensor (round window) must face toward the seating area, not the TV screen.

### Step 4: Connect Power

**USB-C Connection:**
1. Connect the provided USB-C cable to VisionGuard
2. Connect the other end to:
   - **TV USB port** (if available and powered when TV is on)
   - **Wall adapter** using a certified 5V charger
   - **Power strip** with USB charging ports

**Cable Management:**
1. Use provided cable clips to secure cable along TV bezel
2. Route cable to avoid interference with TV controls
3. Ensure cable doesn't create tension on the VisionGuard device

### Step 5: Verify Installation

**Power Verification:**
- Connect power and verify LED illumination
- Blue LED indicates setup mode
- Green LED indicates normal operation

---

## Initial Setup

### Step 1: Power On and WiFi Setup

**First Boot:**
1. Connect power to VisionGuard
2. Wait 30-60 seconds for startup
3. Blue LED will indicate setup mode
4. Device will create a WiFi network named "VisionGuard-Setup-XXXX"

**Connect to Setup Network:**
1. On your phone/computer, go to WiFi settings
2. Connect to "VisionGuard-Setup-XXXX" network
3. Password: "visionguard" (if prompted)
4. Your browser should automatically open the setup page
5. If not, navigate to http://192.168.4.1

### Step 2: WiFi Configuration

**Network Setup:**
1. Select your home WiFi network from the list
2. Enter your WiFi password
3. Click "Connect" and wait 30-60 seconds
4. Device will restart and connect to your network
5. LED will turn green when connected

**Find Your Device:**
1. VisionGuard will announce its IP address on your network
2. Note the IP address displayed on the setup page
3. You can also find it in your router's connected devices list
4. Bookmark this IP address for easy access

### Step 3: Basic Configuration

**Access Main Interface:**
1. Open web browser and go to your device's IP address
2. You'll see the VisionGuard dashboard
3. Click "Initial Setup" to begin configuration

**Quick Setup Wizard:**
1. **Distance Calibration:**
   - Sit at your normal viewing distance
   - Click "Calibrate Safe Distance"
   - Follow on-screen instructions

2. **TV Configuration:**
   - Select your TV brand from the list
   - Test IR codes until your TV responds
   - Save working configuration

3. **Family Settings:**
   - Set different distance rules for adults/children
   - Configure warning times and actions
   - Set up parental access protection

### Step 4: Testing

**Distance Testing:**
1. Sit at various distances from the TV
2. Observe real-time distance readings on the dashboard
3. Verify warnings activate at configured distances
4. Test TV control functionality

**Full System Test:**
1. Sit closer than the safe distance
2. Wait for warning period to expire
3. Verify TV input changes or volume mutes
4. Move to safe distance and verify TV returns to normal

**Troubleshooting Setup Issues:**
- If WiFi connection fails, restart setup mode by holding the reset button
- If IR commands don't work, try different codes or manual learning
- Check sensor readings are realistic (should show actual distance)

---

## Web Interface Guide

### Dashboard Overview

**Main Dashboard Elements:**

**📊 Status Panel (Top)**
- Current operational status (Active, Standby, Warning)
- WiFi connection strength and IP address
- Device uptime and firmware version
- Battery status (if applicable)

**📏 Distance Monitor (Center Left)**
- Real-time distance readings from all viewers
- Color-coded zones: Green (safe), Yellow (caution), Red (too close)
- Historical distance data and trends
- Current active detection zones

**📺 TV Control Panel (Center Right)**
- Current TV status and last command sent
- Manual TV control buttons for testing
- IR signal strength indicator
- Protocol and code information

**⚙️ Quick Settings (Bottom)**
- Toggle device on/off
- Adjust sensitivity settings
- Emergency override controls
- Access full configuration menu

### Navigation Menu

**🏠 Home** - Main dashboard and real-time monitoring

**⚙️ Settings** - Full configuration options including:
- Distance thresholds and timing
- TV and IR remote settings
- WiFi and network configuration
- User profiles and parental controls

**📊 Analytics** - Usage patterns and statistics:
- Daily/weekly viewing distance charts
- Intervention frequency reports
- Family member usage patterns
- Health improvement tracking

**🔧 Tools** - Device management and diagnostics:
- Firmware update management
- Backup/restore configurations
- Network diagnostics
- Sensor calibration tools

**❓ Help** - Built-in documentation and support:
- Quick start guides
- Troubleshooting wizards
- FAQ and common issues
- Contact support options

### Mobile Responsive Features

**Smartphone Interface:**
- Simplified dashboard optimized for small screens
- Swipe navigation between sections
- Touch-friendly control buttons
- Auto-rotating charts and graphs

**Tablet Interface:**
- Split-screen layout showing multiple panels
- Enhanced touch controls
- Optimized for both portrait and landscape modes
- Full desktop functionality in compact form

---

## Configuration Settings

### Distance Settings

**Safe Viewing Distances:**

**Children (Default Settings):**
- **Minimum Safe Distance:** 2.0 meters (6.5 feet)
- **Warning Distance:** 1.5 meters (5 feet)
- **Maximum Detection:** 4.0 meters (13 feet)

**Adults (Recommended):**
- **Minimum Safe Distance:** 1.5 meters (5 feet)
- **Warning Distance:** 1.2 meters (4 feet)
- **Maximum Detection:** 4.0 meters (13 feet)

**Custom Profiles:**
- Create up to 5 custom distance profiles
- Schedule different profiles for different times
- Quick-switch between family members
- Guest mode with relaxed settings

### Timing Configuration

**Warning Sequence:**
- **Initial Warning:** 5-60 seconds (default: 15 seconds)
- **Final Warning:** 3-30 seconds (default: 10 seconds)
- **Action Delay:** 1-10 seconds (default: 3 seconds)

**Compliance Timing:**
- **Return Distance Time:** How long to maintain safe distance (30-300 seconds)
- **Reset Timer:** Time before warnings restart (60-600 seconds)
- **Snooze Period:** Temporary disable duration (5-30 minutes)

### TV Control Settings

**Action Types:**
1. **Input Switch** - Change TV input to different source
2. **Volume Control** - Mute or reduce volume
3. **Power Control** - Turn TV off (use with caution)
4. **Custom Command** - Send specific IR codes

**TV Brand Configuration:**
- **Auto-Detection:** Try multiple codes automatically
- **Brand Selection:** Choose from 200+ supported brands
- **Manual Learning:** Learn codes from your existing remote
- **Custom Codes:** Enter specific IR codes manually

**Advanced IR Settings:**
- **Protocol Selection:** NEC, Sony, RC5, RC6, Samsung
- **Carrier Frequency:** Usually 38kHz (adjustable)
- **Repeat Settings:** Number of command repetitions
- **Timing Adjustments:** Fine-tune for difficult TVs

### Network and Security

**WiFi Configuration:**
- **Network Selection:** Connect to different WiFi networks
- **Password Management:** Update saved WiFi credentials
- **Connection Retry:** Automatic reconnection settings
- **Backup Networks:** Configure fallback WiFi options

**Security Settings:**
- **Device Password:** Protect configuration access
- **Parental Lock:** Restrict settings changes to adults
- **Guest Access:** Limited functionality for visitors
- **Auto-Lock:** Automatically lock settings after inactivity
---

## Daily Operation

### Normal Operation Modes

**Active Monitoring Mode:**
- Continuous distance monitoring when TV is on
- Real-time processing of viewer positions
- Automatic intervention when needed
- Status LED shows steady green

**Standby Mode:**
- TV is off or no viewers detected
- Reduced power consumption
- Periodic sensor checks every 30 seconds
- Status LED shows slow green pulse

**Warning Mode:**
- Viewer detected too close to screen
- Audio and visual warnings activated
- Countdown timer until intervention
- Status LED shows yellow blinking

**Intervention Mode:**
- TV control action executed
- Waiting for compliance with safe distance
- Temporary override available
- Status LED shows red

### Understanding Status Indicators

**LED Status Meanings:**

**🟢 Green Solid:** Normal operation, safe viewing distance
**🟢 Green Pulsing:** Standby mode, no viewers detected
**🟡 Yellow Blinking:** Warning - viewers too close
**🔴 Red Solid:** Intervention active - TV controlled
**🔵 Blue Blinking:** Setup mode or WiFi connecting
**🟣 Purple:** Firmware update in progress
**⚪ White Rapid Blinking:** Error - check web interface

**Audio Indicators (if enabled):**
- **Single Beep:** Warning activated
- **Double Beep:** Final warning before intervention
- **Triple Beep:** Intervention activated
- **Long Beep:** Safe distance restored

### Family Usage Patterns

**Children's TV Time:**
1. **Automatic Detection:** Device recognizes when children are watching
2. **Stricter Enforcement:** Shorter warning times, stricter distances
3. **Usage Tracking:** Monitor daily screen time and distance compliance
4. **Bedtime Mode:** Automatic stricter settings during evening hours

**Family Movie Time:**
1. **Group Detection:** Recognize multiple viewers
2. **Flexible Settings:** Adjusted rules for family activities
3. **Manual Override:** Temporary suspension for special occasions
4. **Activity Logging:** Track family viewing sessions

### Handling Interventions

**When VisionGuard Takes Action:**

**Step 1 - Warning Phase:**
- Visual warning appears on TV (if supported)
- VisionGuard LED turns yellow
- Web dashboard shows countdown timer
- Optional audio alert sounds

**Step 2 - Final Warning:**
- More urgent visual and audio alerts
- Countdown accelerates
- Web interface shows "Move Back Now"
- LED blinks more rapidly

**Step 3 - Intervention:**
- TV input changes or volume mutes
- Red LED indicates intervention active
- Web dashboard shows "Too Close - Move Back"
- Timer starts for compliance period

**Step 4 - Resolution:**
- Move to safe distance and remain there
- Wait for compliance timer to complete
- TV automatically returns to normal
- Green LED indicates safe distance restored

**Manual Override Options:**
- **5-Minute Override:** Temporary disable via web interface
- **Emergency Disable:** Physical button press (if configured)
- **Parental Override:** Password-protected full disable
- **Guest Mode:** Relaxed enforcement for visitors

---

## Troubleshooting

### Common Issues and Solutions

**🔴 Device Won't Power On**

*Symptoms:* No LED lights, no response, device appears dead

*Solutions:*
1. **Check Power Connection:**
   - Verify USB-C cable is securely connected
   - Try a different USB power adapter (minimum 5V, 1A)
   - Test with phone charger to confirm cable works

2. **Verify Power Source:**
   - If using TV USB port, ensure it's powered when TV is on
   - Try wall adapter instead of TV USB port
   - Check power strip switches and circuit breakers

3. **Reset Device:**
   - Hold reset button for 10 seconds while powered
   - Wait 30 seconds then reconnect power
   - Look for blue LED indicating setup mode

**📏 Inaccurate Distance Readings**

*Symptoms:* Distance shows wrong measurements, doesn't detect people, random false alarms

*Solutions:*
1. **Sensor Calibration:**
   - Clean sensor window with soft, dry cloth
   - Ensure nothing is blocking the sensor
   - Run calibration wizard in web interface

2. **Environmental Factors:**
   - Check for bright sunlight or heat sources near sensor
   - Remove reflective objects from detection area
   - Adjust sensitivity settings for your room

3. **Positioning Issues:**
   - Verify sensor faces seating area (not TV screen)
   - Check mounting is secure and not vibrating
   - Ensure device is level and properly oriented

**📺 TV Control Not Working**

*Symptoms:* VisionGuard detects distance correctly but can't control TV

*Solutions:*
1. **IR Code Configuration:**
   - Try different TV brand codes in settings
   - Use manual learning to copy your remote
   - Test IR transmission range (should work within 8 meters)

2. **Physical Obstacles:**
   - Ensure clear line of sight between device and TV
   - Remove objects blocking IR sensor on TV
   - Check TV IR receiver isn't covered or damaged

3. **Protocol Settings:**
   - Try different IR protocols (NEC, Sony, Samsung)
   - Adjust repeat settings for stubborn TVs
   - Verify TV responds to original remote in same position

**🌐 Web Interface Won't Load**

*Symptoms:* Can't access device web page, browser shows error, page loads slowly

*Solutions:*
1. **Network Connection:**
   - Verify device and computer are on same network
   - Check device IP address hasn't changed
   - Try different web browser

2. **Browser Issues:**
   - Clear browser cache and cookies
   - Disable ad blockers temporarily
   - Enable JavaScript if disabled

3. **Device Issues:**
   - Restart VisionGuard device
   - Check device status LED is green
   - Try accessing from different device

### Error Messages and Meanings

**"Sensor Error - Check Connection"**
- Internal sensor communication failure
- Try power cycling the device
- Contact support if error persists

**"WiFi Disconnected"**
- Lost connection to home network
- Device will attempt automatic reconnection
- Check router and network stability

**"IR Transmitter Fault"**
- Hardware issue with infrared LED
- TV control functions will be disabled
- Device can still monitor distance

**"Calibration Required"**
- Distance measurements may be inaccurate
- Run calibration wizard in settings
- Ensure sensor is clean and unobstructed

**"Firmware Update Available"**
- New features and improvements available
- Update through web interface settings
- Ensure stable power during update

### Storage and Transportation

**Long-Term Storage:**
- Clean device thoroughly before storage
- Store in original packaging if available
- Keep in dry, room temperature environment
- Avoid extreme temperatures (-40°C to +85°C storage range)

**Transportation:**
- Power down and disconnect cables
- Use protective padding to prevent impact damage
- Avoid exposing to extreme temperatures
- Handle sensor area carefully

---

## Technical Specifications

### Hardware Specifications

**Main Processor:**
- **Model:** ESP32-C6-MINI-1-N4
- **Architecture:** 32-bit RISC-V single-core
- **Clock Speed:** Up to 160MHz
- **Memory:** 4MB Flash, 512KB RAM
- **Connectivity:** WiFi 6 (802.11ax), Bluetooth 5.0 LE

**Distance Sensor:**
- **Model:** VL53L5CX Time-of-Flight Sensor
- **Technology:** SPAD (Single Photon Avalanche Diode) array
- **Resolution:** 8x8 zones (64 individual measurements)
- **Range:** 4mm to 4000mm (0.16" to 13.1 feet)
- **Accuracy:** ±3% or ±10mm, whichever is greater
- **Field of View:** Up to 63° diagonal
- **Update Rate:** Up to 60Hz (configurable)

**Infrared Transmitter:**
- **LED Wavelength:** 940nm
- **Transmission Range:** Up to 8 meters (26 feet)
- **Supported Protocols:** NEC, RC5, RC6, Sony SIRC, Samsung
- **Modulation:** 38kHz carrier frequency (adjustable)
- **Power Output:** High-power LED with adjustable intensity

**Power System:**
- **Input:** USB-C connector, 5V DC
- **Consumption:** 150mA active, 25mA standby, 5mA deep sleep
- **Power Management:** Low-dropout regulator with multiple rail voltages
- **Protection:** Overvoltage, overcurrent, and ESD protection

**Physical Specifications:**
- **Dimensions:** 50mm x 16mm x 8mm (2.0" x 0.63" x 0.31")
- **Weight:** 12 grams (0.42 oz)
- **Mounting:** Adhesive or magnetic
- **Cable Length:** 1.5 meters (5 feet)
- **Enclosure:** ABS plastic with UV resistance

### Environmental Specifications

**Operating Conditions:**
- **Temperature:** -10°C to +60°C (14°F to 140°F)
- **Humidity:** 10% to 90% RH, non-condensing
- **Altitude:** Up to 2000 meters (6500 feet)
- **Vibration:** Residential/office environment suitable

**Storage Conditions:**
- **Temperature:** -40°C to +85°C (-40°F to 185°F)
- **Humidity:** 5% to 95% RH, non-condensing
- **Shock:** Drop test: 1 meter onto concrete surface

### Network and Communication

**WiFi Specifications:**
- **Standards:** 802.11b/g/n/ax (WiFi 6)
- **Frequency:** 2.4GHz only
- **Security:** WPA2, WPA3 support
- **Range:** Up to 50 meters indoor, line of sight
- **Antenna:** Integrated PCB antenna

**Bluetooth (Setup Only):**
- **Standard:** Bluetooth 5.0 Low Energy
- **Range:** Up to 10 meters (33 feet)
- **Purpose:** Initial setup and diagnostics only

**Web Interface:**
- **Server:** Embedded HTTP/WebSocket server
- **Port:** 80 (HTTP), configurable
- **API:** RESTful interface for automation
- **Update Rate:** Real-time via WebSocket
- **Bandwidth:** <1Mbps typical usage

### Sensor Performance

**Detection Capabilities:**
- **Minimum Distance:** 10cm (4 inches) reliable detection
- **Maximum Distance:** 4 meters (13.1 feet)
- **Resolution:** 1mm distance resolution
- **Multiple Targets:** Simultaneous tracking of multiple people
- **Response Time:** <100ms for distance changes

**Environmental Adaptation:**
- **Ambient Light:** Operates in 0 to 100,000 lux
- **Temperature Compensation:** Automatic calibration adjustment
- **Interference Rejection:** Immune to other IR sources
- **Surface Types:** Works on all common surfaces and materials

### Safety and Compliance

**Electrical Safety:**
- **Safety Standard:** IEC 62368-1 compliant
- **Input Protection:** Overvoltage and overcurrent protection
- **Isolation:** Low voltage DC operation only
- **ESD Protection:** 8kV contact, 15kV air discharge

---

**Document Version**: 1.0  
**Last Updated**: July 21, 2025  
**Compatible Firmware**: v4.0 and later

---

*Thank you for choosing the Eye Protection Device. We're committed to helping your family develop healthy viewing habits while enjoying their favorite content safely.*
