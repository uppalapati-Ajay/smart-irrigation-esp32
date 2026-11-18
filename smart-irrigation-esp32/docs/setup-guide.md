# Setup guide

Run the whole system in about five minutes — no hardware needed.

## 1. Run the firmware in Wokwi
1. Open [wokwi.com](https://wokwi.com) → **New Project → ESP32**.
2. Replace `sketch.ino` with [`firmware/smart_irrigation_v2.ino`](../firmware/smart_irrigation_v2.ino).
3. Replace `diagram.json` with [`wokwi/diagram.json`](../wokwi/diagram.json) and add the libraries in [`wokwi/libraries.txt`](../wokwi/libraries.txt).
4. Press **▶**. The serial monitor should show `WiFi connected`, `MQTT connected`, then a JSON line every 2 s.
5. Turn the potentiometer knob (soil moisture). Below **30 %** the LED (pump) turns ON; it turns OFF above **45 %**.

## 2. Open the dashboard
- Hosted: `https://YOUR-USERNAME.github.io/smart-irrigation-esp32/dashboard/irrigation_dashboard_v3.html`
- Or open the file locally in any browser (needs internet for Chart.js and mqtt.js from a CDN).
- No device running? Add `?demo=1` to the URL — the built-in demo mode simulates data.

URL options:

| Parameter | Default | Meaning |
|---|---|---|
| `topic` | `smartirrigation/ajay4821/data` | Telemetry topic to subscribe to |
| `broker` | `wss://broker.hivemq.com:8884/mqtt` | Broker WebSocket URL |
| `demo` | – | `1` starts simulated data |

## 3. Make the topic yours (important)
The public HiveMQ broker has **no authentication**: anyone who knows your topic can read your data **and send pump commands**.
Pick a unique id and change it in **both** places:

```cpp
// firmware/smart_irrigation_v2.ino
const char* TOPIC_DATA = "smartirrigation/<your-unique-id>/data";
const char* TOPIC_CMD  = "smartirrigation/<your-unique-id>/cmd";
```
```
.../irrigation_dashboard_v3.html?topic=smartirrigation/<your-unique-id>/data
```
The dashboard derives the `/cmd` topic from the `/data` topic.

## 4. Move to real hardware
1. Calibrate `DRY_RAW` (sensor in air) and `WET_RAW` (sensor in water) in the firmware.
2. Replace `WiFi.begin("Wokwi-GUEST", "", 6)` with your SSID and password (keep them out of Git — see `.gitignore`).
3. Wire the relay module to GPIO26 and power the pump from its own supply with a flyback diode; share GND with the ESP32.
4. Use a private broker with TLS and credentials (HiveMQ Cloud has a free tier) instead of the public one.

## Troubleshooting
| Symptom | Fix |
|---|---|
| Dashboard says *connecting* forever | Browser blocks WebSockets, or broker URL is wrong; try `?demo=1` to confirm the page works |
| No data but ESP32 prints MQTT connected | Topic mismatch — check both topics match exactly |
| Pump never turns on | In auto mode moisture must be **< 30 %** and the 5 s cool-down must have elapsed; after a `pump:*` command the device is in manual mode — send `mode:auto` |
| Pump stops after 10 s | Safety cut-off (`MAX_RUN_MS`) — by design |
