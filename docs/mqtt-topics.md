# MQTT reference

Broker: `broker.hivemq.com` — device on TCP `1883`, browser on WebSocket `wss://broker.hivemq.com:8884/mqtt`.

## Telemetry — `smartirrigation/ajay4821/data`
Published by the ESP32 every 2 seconds.

```json
{"moisture":42,"temp":27.4,"humidity":55,"pump":0,"auto":1,"low":30,"high":45,"up":128}
```

| Field | Meaning |
|---|---|
| `moisture` | Soil moisture, 0–100 % (10-sample average) |
| `temp` | Air temperature, °C (DHT22) |
| `humidity` | Relative humidity, % (DHT22) |
| `pump` | `1` running, `0` stopped |
| `auto` | `1` automatic mode, `0` manual |
| `low` / `high` | Current start / stop thresholds (%) |
| `up` | Uptime in seconds |

## Commands — `smartirrigation/ajay4821/cmd`
Published by the dashboard (or any MQTT client).

| Payload | Effect |
|---|---|
| `mode:auto` | Hysteresis control on |
| `mode:manual` | Auto-control off, pump holds its requested state |
| `pump:on` / `pump:off` | Switches to manual mode **and** runs / stops the pump (10 s safety cut-off still applies) |
| `low:N` | Start threshold = N % (clamped 0–95) |
| `high:N` | Stop threshold = N % (clamped 5–100; firmware keeps `high` at least 5 points above `low`) |

Try it from a terminal:
```bash
mosquitto_pub -h broker.hivemq.com -t smartirrigation/ajay4821/cmd -m "mode:manual"
mosquitto_pub -h broker.hivemq.com -t smartirrigation/ajay4821/cmd -m "pump:on"
mosquitto_sub -h broker.hivemq.com -t smartirrigation/ajay4821/data
```

> ⚠️ Public broker = public topic. Use your own unique topic id (see the [setup guide](setup-guide.md)).
