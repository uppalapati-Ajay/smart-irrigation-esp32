<p align="center">
  <img src="docs/assets/banner.svg" alt="Smart Irrigation System — ESP32 · MQTT · Live Dashboard" width="100%">
</p>

<p align="center">
  <a href="https://YOUR-USERNAME.github.io/smart-irrigation-esp32/dashboard/irrigation_dashboard_v3.html?demo=1"><img alt="Live demo" src="https://img.shields.io/badge/▶_live_demo-dashboard-4ade80?style=for-the-badge&labelColor=0b2a22"></a>
  <a href="https://YOUR-USERNAME.github.io/smart-irrigation-esp32/"><img alt="Project page" src="https://img.shields.io/badge/project_page-animated-38bdf8?style=for-the-badge&labelColor=0b2a22"></a>
  <a href="LICENSE"><img alt="MIT" src="https://img.shields.io/badge/license-MIT-fbbf24?style=for-the-badge&labelColor=0b2a22"></a>
</p>

<p align="center">
  <img alt="ESP32" src="https://img.shields.io/badge/ESP32-Arduino_C++-0b3a4a?logo=espressif&logoColor=white">
  <img alt="MQTT" src="https://img.shields.io/badge/MQTT-HiveMQ-660066?logo=mqtt&logoColor=white">
  <img alt="Wokwi" src="https://img.shields.io/badge/simulated_in-Wokwi-1f6feb">
  <img alt="Dashboard" src="https://img.shields.io/badge/dashboard-Chart.js_·_mqtt.js-ff6384">
</p>

# 🌱 Smart Irrigation System

> **What if the soil could talk?**
> An ESP32 reads soil moisture, temperature and humidity, decides when to water using hysteresis control, publishes everything over MQTT, and a live browser dashboard lets you watch — and switch the pump from anywhere.

Built and tested entirely in the [Wokwi](https://wokwi.com) simulator — **no hardware needed to try it.**
By **Ajay Chowdary Uppalapati**, B.Sc. Horticulture.

<p align="center">
  <img src="docs/assets/demo.gif" alt="Project walkthrough" width="720">
</p>

## ✨ Features

| | |
|---|---|
| 📡 **Live telemetry** | Moisture, temperature, humidity and pump state every **2 s** over MQTT |
| 🧮 **Smart averaging** | 10 ADC samples per reading smooth out sensor noise |
| 🎯 **Hysteresis control** | ON below **30 %**, OFF above **45 %** — the pump never flickers |
| 🛡️ **Safety first** | 10 s maximum run + 5 s cool-down in every mode |
| 🎛️ **Remote control** | Switch auto/manual, run the pump, change thresholds from the dashboard |
| 🖥️ **Dashboard** | Gauges, history chart, event log, animated field scene, built-in demo mode |

## 🧭 How it works

```mermaid
flowchart LR
    S["🌱 Soil sensor<br/>GPIO34"] --> E
    D["🌡️ DHT22<br/>GPIO4"] --> E
    E{{"ESP32<br/>average · decide"}} -->|GPIO26| P["💧 Relay + pump"]
    E -- "…/data (JSON, 2 s)" --> B(("HiveMQ<br/>MQTT broker"))
    B -- "…/cmd" --> E
    B <-->|WebSocket| W["🖥️ Live dashboard"]
```

<p align="center"><img src="docs/assets/architecture.svg" alt="Architecture" width="100%"></p>

### Hysteresis in one picture

<p align="center"><img src="docs/assets/hysteresis.svg" alt="Hysteresis" width="720"></p>

## 📁 Repository structure

```
smart-irrigation-esp32/
├── firmware/
│   └── smart_irrigation_v2.ino        # ESP32 firmware (Arduino C++)
├── dashboard/
│   └── irrigation_dashboard_v3.html   # Single-file live dashboard
├── docs/
│   ├── setup-guide.md                 # Run it in 5 minutes
│   ├── how-it-works.md                # Control loop explained
│   ├── mqtt-topics.md                 # Topics, JSON, commands
│   ├── hardware.md                    # Wiring & parts list
│   └── assets/                        # Animated SVGs, GIF, screenshots
├── wokwi/
│   ├── diagram.json                   # Simulator circuit
│   └── libraries.txt
├── index.html                         # Animated project page (GitHub Pages)
├── LICENSE                            # MIT
└── README.md
```

## 🚀 Quick start

1. **Simulate** — follow the [setup guide](docs/setup-guide.md): paste the firmware into a new Wokwi ESP32 project and press ▶.
2. **Watch** — open the [dashboard](https://YOUR-USERNAME.github.io/smart-irrigation-esp32/dashboard/irrigation_dashboard_v3.html) (add `?demo=1` to try it without a device).
3. **Control** — switch to manual and run the pump from the dashboard, or from a terminal:

```bash
mosquitto_pub -h broker.hivemq.com -t smartirrigation/ajay4821/cmd -m "pump:on"
```

## 🔌 Wiring

<p align="center"><img src="docs/assets/wiring.svg" alt="Wiring" width="100%"></p>

| Part | GPIO |
|---|---|
| Soil moisture sensor (analog) — potentiometer in Wokwi | 34 |
| DHT22 temperature + humidity | 4 |
| Relay → pump — LED in Wokwi | 26 |

## 📸 Screenshots

<table>
  <tr>
    <td><img src="docs/assets/preview-dataflow.jpg" alt="One reading's journey"></td>
    <td><img src="docs/assets/preview-firmware.jpg" alt="Firmware logic"></td>
  </tr>
  <tr>
    <td><img src="docs/assets/preview-architecture.jpg" alt="System overview"></td>
    <td><img src="docs/assets/preview-dashboard.jpg" alt="Dashboard"></td>
  </tr>
</table>

## ⚠️ Security note

The default setup uses the **public** HiveMQ broker with no authentication. Anyone who knows the topic can read the data **and send pump commands**.
Change the `ajay4821` topic id to something unique before sharing, and use a private broker with TLS + credentials for real hardware.
Details in the [setup guide](docs/setup-guide.md#3-make-the-topic-yours-important).

## 🗺️ Roadmap

- [ ] FreeRTOS tasks for sensing / control / networking
- [ ] Real hardware build with capacitive sensor and relay
- [ ] Solar power with battery monitoring
- [ ] Private broker with TLS and authentication
- [ ] Rain forecast integration

## 📄 License

[MIT](LICENSE) © 2026 Ajay Chowdary Uppalapati

---
<p align="center">If this helped you, drop a ⭐ — and tell me what you'd build next.</p>
