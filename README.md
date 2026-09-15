# 📡 ESP32 HLK-LD2410C 24GHz mmWave Human Presence & Micro-Motion Radar

[![GitHub Repository](https://img.shields.io/badge/GitHub-Repository-00f0ff?style=for-the-badge&logo=github&logoColor=white)](https://github.com/ALWINTR/esp32-hlk-ld2410c-presence-radar)
[![Developer](https://img.shields.io/badge/Developer-Alwin_T_R-0284c7?style=for-the-badge&logo=linkedin&logoColor=white)](https://www.linkedin.com/in/alwintr)
[![Platform](https://img.shields.io/badge/Platform-ESP32_&_24GHz_mmWave-38bdf8?style=for-the-badge&logo=espressif&logoColor=white)](https://github.com/ALWINTR)
[![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)](LICENSE)

24GHz mmWave human presence and micro-motion tracking radar system using ESP32 and HLK-LD2410C sensor.

---

## 📌 System Architecture

A high-precision 24GHz millimeter-wave (mmWave) FMCW human presence sensing system powered by the **HLK-LD2410C** radar sensor and **ESP32**, capable of detecting stationary micro-movements (breathing, typing) and dynamic moving targets up to 6 meters with gate distance classification.

```
       ┌───────────────────────────────┐
       │   HLK-LD2410C 24GHz mmWave    │
       │   • Moving Target Energy      │
       │   • Static Target Energy      │
       └───────────────┬───────────────┘
                       │ UART Stream (256000 Baud)
                       ▼
       ┌───────────────────────────────┐
       │     ESP32 Microcontroller     │
       │   • Target Gate Parsing       │
       │   • Real-Time BLE / Telemetry │
       └───────────────┬───────────────┘
                       ▼
       ┌───────────────────────────────┐
       │   Smart Home / IoT Relay      │
       └───────────────────────────────┘
```

---

## ⚙️ Hardware Specifications

| Component | Technical Specification | Function |
| :--- | :--- | :--- |
| **Microcontroller** | ESP32-WROOM-32 (Dual-Core @ 240MHz) | High-speed serial parsing & BLE telemetry |
| **Radar Sensor** | Hi-Link HLK-LD2410C (24GHz FMCW) | Stationary human & motion detection |
| **Detection Range** | 0.75m to 6.0m (Configurable across 8 Gates) | Multi-zone distance resolution |
| **Interface** | Hardware Serial UART (256000 / 115200 Baud) | Engineering data frame output |
| **Auxiliary Output** | Digital OUT Pin | Instant hardware GPIO presence trip |

---

## 🔌 Circuit Pinout Table

| HLK-LD2410C Pin | ESP32 GPIO Pin | Description |
| :--- | :--- | :--- |
| **VCC** | 5V External Rail | Sensor power input (79mA operating current) |
| **GND** | Common GND | Ground reference |
| **TX** | GPIO 16 (RX2) | Radar data frame output to ESP32 |
| **RX** | GPIO 17 (TX2) | Configuration parameter commands from ESP32 |
| **OUT** | GPIO 4 | Hardware High-Level presence output (3.3V Logic) |

---

## 👨‍💻 Author

**Alwin T R** — Robotics & Automation Engineer  
- 💼 LinkedIn: [linkedin.com/in/alwintr](https://www.linkedin.com/in/alwintr)  
- 🌌 Portfolio: [alwintr.github.io](https://alwintr.github.io)  
- 💻 GitHub: [github.com/ALWINTR](https://github.com/ALWINTR)

---

## 📄 License

This project is licensed under the **MIT License** — see the [LICENSE](LICENSE) file for details.
