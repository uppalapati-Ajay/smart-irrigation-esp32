# Wokwi project files

| File | Purpose |
|---|---|
| `diagram.json` | Circuit: ESP32 DevKit + potentiometer (soil sensor stand-in) + DHT22 + LED (pump) |
| `libraries.txt` | Libraries to add in Wokwi's Library Manager |

**Use it:** create a new ESP32 project on [wokwi.com](https://wokwi.com), paste `diagram.json` into the *diagram.json* tab,
paste `../firmware/smart_irrigation_v2.ino` into `sketch.ino`, add the libraries, press ▶.

> `diagram.json` was written by hand to match the GPIO map in the firmware. If Wokwi shows a part or pin warning,
> rewire it in the editor (potentiometer SIG → GPIO34, DHT22 SDA → GPIO4, LED anode → GPIO26) and copy your own diagram back here.
