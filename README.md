# 24GHz mmWave Human Presence and Micro Motion Radar Monitor

<div align="center">

[![GitHub Repository](https://img.shields.io/badge/GitHub-Repository-00f0ff?style=for-the-badge&logo=github&logoColor=white)](https://github.com/ALWINTR/24GHz-mmWave-Human-Presence-and-Micro-Motion-Radar-Monitor)
[![Developer](https://img.shields.io/badge/Developer-Alwin_T_R-0284c7?style=for-the-badge&logo=linkedin&logoColor=white)](https://www.linkedin.com/in/alwintr)
[![Portfolio](https://img.shields.io/badge/Portfolio-alwintr.github.io-00f0ff?style=for-the-badge&logo=googlechrome&logoColor=black)](https://alwintr.github.io)
[![License](https://img.shields.io/badge/License-MIT-10b981?style=for-the-badge)](LICENSE)

</div>

> Advanced 24GHz FMCW millimeter-wave radar monitoring station utilizing Hi-Link HLK-LD2410C to detect microscopic chest movements (breathing, sedentary presence) across 8 range gates up to 6 meters.

---

## 📌 Executive Overview

Advanced 24GHz FMCW millimeter-wave radar monitoring station utilizing Hi-Link HLK-LD2410C to detect microscopic chest movements (breathing, sedentary presence) across 8 range gates up to 6 meters. This repository contains production-ready firmware implementations, hardware wiring pinouts, signal conditioning routines, and real-time telemetry pipelines engineered for reliable physical operation.

- **Primary Domain**: Radar & Sensing
- **Microcontroller & Hardware Core**: 24GHz FMCW / ESP32
- **Specialization**: Radar Sensing
- **Live Specs & BOM Viewer**: [alwintr.github.io](https://alwintr.github.io)

---

## 📐 System Architecture & Signal Flow

```
[ 24GHz FMCW Radio Waves ] ===> [ Moving / Breathing Target ]
                                           | Reflected Chirp Spectrum
                                           v
+--------------------------------------------------------------+
|            Hi-Link HLK-LD2410C 24GHz mmWave Sensor           |
|  - Integrated Baseband DSP     - 8 Range Gates (0.75m / gate)|
|  - Motion Energy & Static Micro-Movement Energy Calculation  |
+--------------------------------------------------------------+
        |
        | Hardware UART Stream (115200 Baud Frame Protocol)
        v
+--------------------------------------------------------------+
|                  ESP32 Dual-Core Microcontroller             |
|  - Frame Sync & Byte Parsing   - Per-Gate Energy Thresholding|
|  - Target Distance & Velocity  - Occupancy Verification State|
+--------------------------------------------------------------+
        |
        v I2C / Serial HUD
+--------------------------------------------------------------+
|         OLED Real-Time Radar Target & Distance HUD           |
+--------------------------------------------------------------+
```

---

## 📋 Hardware Bill of Materials (BOM)

| Component Name | Operating Voltage | Interface / Functionality |
| :--- | :--- | :--- |
| **ESP32-WROOM-32D** | `3.3V / 5V` | Dual-Core 240MHz SoC |
| **Hi-Link HLK-LD2410C 24GHz Radar** | `5V DC / 80mA` | 24GHz FMCW Transceiver |
| **0.96" SSD1306 I2C OLED** | `3.3V DC` | Radar range & energy HUD |
| **Status Indicator LEDs** | `5V DC` | Stationary Presence & Motion LEDs |

---

## 🔌 Circuit Pinout & Microcontroller Interconnect

| MCU Pin / Channel | Peripheral Pin | Signal Function / Bus |
| :--- | :--- | :--- |
| `ESP32 GPIO 16 (RX2)` | `HLK-LD2410C TX Out` | Hardware UART2 at 115200 Baud |
| `ESP32 GPIO 17 (TX2)` | `HLK-LD2410C RX In` | Configuration Command Bus |
| `ESP32 GPIO 21 (SDA), GPIO 22 (SCL)` | `SSD1306 OLED` | I2C Display Bus |
| `ESP32 GPIO 4` | `Micro-Motion Detected LED` | Logic High on Stationary Presence |

---

## ⚙️ Firmware Architecture & Control Logic

Binary frame packet decoder parsing proprietary 0xFD 0xFC 0xFB 0xFA headers, extracting stationary and moving energy values across all 8 range gates in real-time.

### Key Firmware Capabilities:
- **Zero-Blocking Architecture**: Real-time non-blocking scheduling eliminating delay() deadlocks.
- **Hardware Interrupt Servicing**: Microsecond-precision edge-triggered pulse detection and safety shutoffs.
- **Diagnostic Telemetry**: Continuous serial datastreaming and state-machine transition logging.

---

## 📁 Repository Structure

```
24GHz-mmWave-Human-Presence-and-Micro-Motion-Radar-Monitor/
├── .gitignore
├── LICENSE
├── platformio.ini
├── src
└── README.md
```

---

## 🚀 Build & Deployment Instructions

### 1. Hardware Assembly
- Verify common system ground across all microcontrollers, drivers, and peripheral sensor boards.
- Ensure dedicated power supply rails match the operating voltage specified in the Hardware Bill of Materials.

### 2. Firmware Compilation & Flashing
```bash
# Clone the repository
git clone https://github.com/ALWINTR/24GHz-mmWave-Human-Presence-and-Micro-Motion-Radar-Monitor.git

# Navigate to the project directory
cd 24GHz-mmWave-Human-Presence-and-Micro-Motion-Radar-Monitor
```

- Open the project in **Arduino IDE**, **PlatformIO**, or **ESP-IDF**.
- Select the target board configuration and COM port.
- Compile and upload firmware directly to the microcontroller.

---

## 👨‍💻 Author

**Alwin T R** — Robotics & Automation Engineer  
- 💼 **LinkedIn**: [linkedin.com/in/alwintr](https://www.linkedin.com/in/alwintr)  
- 🌐 **Interactive Portfolio**: [alwintr.github.io](https://alwintr.github.io)  
- 🐙 **GitHub**: [github.com/ALWINTR](https://github.com/ALWINTR)  
- ✉️ **Direct Email**: [alwintr2003@gmail.com](mailto:alwintr2003@gmail.com)

---

## 📄 License

This project is licensed under the **MIT License** — see the [LICENSE](LICENSE) file for details.
