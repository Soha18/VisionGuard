# ESPHome Firmware Environment for VisionGuard

This directory contains the ESPHome firmware environment for the VisionGuard project. It provides a Docker-based setup for building, flashing, and managing ESPHome devices, including the VisionGuard Rev.1 hardware.

## Requirements

- **Docker**: Ensure Docker is installed on your system. [Install Docker](https://docs.docker.com/get-docker/)
- **Docker Compose**: Required to orchestrate the ESPHome container. [Install Docker Compose](https://docs.docker.com/compose/install/)
- **Supported OS**: Linux (recommended), macOS, or Windows with Docker support.
- **ESP32 Device**: The stack is configured for ESP32-C6-DevKitM-1 hardware.

## Directory Structure

- `docker-compose.yaml` — Docker Compose file to launch the ESPHome service.
- `config/` — Contains all ESPHome YAML configuration files, secrets, and custom components:
  - `vision-guard-rev-1.yml` — Main ESPHome config for VisionGuard Rev.1.
  - `secrets.yaml` — WiFi and sensitive credentials (not included in version control).
  - `my_components/` — Custom ESPHome components (e.g., VL53L3CX sensor driver).

## Getting Started

1. **Clone the Repository**

   ```bash
   git clone <this-repo-url>
   cd FIRMWARE/ESPHome
   ```

2. **Configure Secrets**

   - Copy `config/secrets.yaml.example` to `config/secrets.yaml` (if provided), and fill in your WiFi credentials and other secrets.

3. **Connect Your ESP32 Device**

   - Plug your ESP32 device into your computer. The container is pre-configured to access `/dev/ttyACM0` through `/dev/ttyACM6`.

4. **Start ESPHome**

   Launch the ESPHome dashboard using Docker Compose:

   ```bash
   docker compose up -d
   ```

   This will start the ESPHome web dashboard on port 6052 (default ESPHome port) or as configured.

5. **Access the ESPHome Dashboard**

   Open your browser and go to:

   ```
   http://localhost:6052
   ```
   (Or the port mapped by Docker, if different.)

6. **Build, Upload, and Monitor Firmware**

   - Use the ESPHome dashboard to build and upload firmware to your device.
   - The `config/vision-guard-rev-1.yml` file contains the main configuration for the VisionGuard Rev.1 device, including sensor, IR, and automation logic.

## Notes

- The container runs with `privileged: true` and `network_mode: host` to allow direct access to USB devices and network discovery.
- The `config/` folder is mounted into the container, so any changes to YAML files or custom components are immediately available in ESPHome.
- For advanced configuration, refer to the [ESPHome documentation](https://esphome.io/).

## Stopping the Stack

To stop the ESPHome service:

```bash
docker compose down
```

---

For more details on the VisionGuard firmware logic, see `config/vision-guard-rev-1.yml` and the technical documentation in the main project `DOCS/` folder.
