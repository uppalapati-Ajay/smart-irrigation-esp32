/*
 * Smart Irrigation System - ESP32 firmware v2
 * ---------------------------------------------------------------------------
 * Author   : Ajay Chowdary Uppalapati
 * License  : MIT (see LICENSE)
 * Platform : ESP32 + Arduino framework (built and tested in the Wokwi simulator)
 *
 * What it does
 *   1. Reads soil moisture (10-sample average, 12-bit ADC -> 0-100 %).
 *   2. Reads air temperature + humidity from a DHT22.
 *   3. Decides with HYSTERESIS: pump ON below `low` %, OFF above `high` %.
 *   4. Switches the pump (relay / LED) with a 10 s max-run limit and 5 s cool-down.
 *   5. Publishes JSON telemetry over MQTT every 2 s and listens for commands.
 *
 * Wiring (GPIO)
 *   GPIO34 <- soil moisture sensor (analog)   |  GPIO4  <- DHT22 data
 *   GPIO26 -> pump relay (or LED in Wokwi)
 *
 * Libraries (Library Manager): PubSubClient, DHT sensor library, Adafruit Unified Sensor
 *
 * MQTT
 *   Telemetry  -> smartirrigation/ajay4821/data   JSON every 2 s
 *     {"moisture":42,"temp":27.4,"humidity":55,"pump":0,"auto":1,"low":30,"high":45,"up":128}
 *   Commands   <- smartirrigation/ajay4821/cmd    plain text
 *     mode:auto | mode:manual | pump:on | pump:off | low:30 | high:45
 *
 * IMPORTANT: broker.hivemq.com is a PUBLIC broker - anyone who knows the topic can read
 * the data and send commands. Change "ajay4821" to your own unique id in BOTH this file
 * and the dashboard (or use ?topic=... on the dashboard URL). For real hardware use a
 * private broker with TLS + username/password.
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define SOIL_PIN 34
#define DHT_PIN 4
#define PUMP_PIN 26

#define DRY_RAW 4095       // calibrate for your real sensor
#define WET_RAW 0
#define MAX_RUN_MS 10000   // safety: max pump run time
#define COOLDOWN_MS 5000   // safety: rest time after a stop

const char* MQTT_SERVER = "broker.hivemq.com";
const char* TOPIC_DATA  = "smartirrigation/ajay4821/data";
const char* TOPIC_CMD   = "smartirrigation/ajay4821/cmd";

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);
DHT dht(DHT_PIN, DHT22);

int onBelow = 30;          // start pumping below this %
int offAbove = 45;         // stop pumping above this %
bool autoMode = true;      // true = automatic, false = manual
bool manualPump = false;   // requested pump state in manual mode
bool pumpOn = false;

unsigned long pumpStart = 0, lastStop = 0;
unsigned long lastDht = 0, lastPublish = 0, lastReconnect = 0;
float temp = 0, hum = 0;

int readMoisture() {
  long sum = 0;
  for (int i = 0; i < 10; i++) {   // average 10 samples to reduce noise
    sum += analogRead(SOIL_PIN);
    delay(5);
  }
  int pct = map(sum / 10, DRY_RAW, WET_RAW, 0, 100);
  return constrain(pct, 0, 100);
}

// Called by PubSubClient whenever a command arrives
void onMessage(char* topic, byte* payload, unsigned int length) {
  String s;
  for (unsigned int i = 0; i < length; i++) s += (char)payload[i];
  s.trim();
  Serial.println("CMD: " + s);

  if (s == "mode:auto") autoMode = true;
  else if (s == "mode:manual") autoMode = false;
  else if (s == "pump:on") { autoMode = false; manualPump = true; }
  else if (s == "pump:off") { autoMode = false; manualPump = false; }
  else if (s.startsWith("low:")) onBelow = constrain(s.substring(4).toInt(), 0, 95);
  else if (s.startsWith("high:")) offAbove = constrain(s.substring(5).toInt(), 5, 100);

  // keep a gap between the two thresholds
  if (offAbove < onBelow + 5) offAbove = min(100, onBelow + 5);
}

void setup() {
  Serial.begin(115200);
  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW);
  dht.begin();

  WiFi.begin("Wokwi-GUEST", "", 6);   // Wokwi's built-in simulated Wi-Fi
  while (WiFi.status() != WL_CONNECTED) { delay(250); Serial.print("."); }
  Serial.println("\nWiFi connected");

  mqtt.setServer(MQTT_SERVER, 1883);
  mqtt.setBufferSize(256);
  mqtt.setCallback(onMessage);
}

void loop() {
  unsigned long now = millis();

  // (re)connect to the broker every 5 s if needed
  if (!mqtt.connected() && now - lastReconnect > 5000) {
    lastReconnect = now;
    String id = "esp32-" + String(random(0xffff), HEX);
    if (mqtt.connect(id.c_str())) {
      Serial.println("MQTT connected");
      mqtt.subscribe(TOPIC_CMD);
    } else {
      Serial.printf("MQTT failed, rc=%d\n", mqtt.state());
    }
  }
  mqtt.loop();

  int moisture = readMoisture();

  // DHT22 can only be read about every 2 s
  if (now - lastDht >= 2000) {
    lastDht = now;
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    if (!isnan(t) && !isnan(h)) { temp = t; hum = h; }
  }

  // ---- pump control ----
  if (autoMode) {
    if (!pumpOn && moisture < onBelow && now - lastStop > COOLDOWN_MS) {
      pumpOn = true; pumpStart = now;
    }
    if (pumpOn && moisture > offAbove) {
      pumpOn = false; lastStop = now;
    }
  } else {
    if (manualPump && !pumpOn) { pumpOn = true; pumpStart = now; }
    if (!manualPump && pumpOn) { pumpOn = false; lastStop = now; }
  }
  // safety limit applies in both modes
  if (pumpOn && now - pumpStart > MAX_RUN_MS) {
    pumpOn = false; lastStop = now; manualPump = false;
  }
  digitalWrite(PUMP_PIN, pumpOn ? HIGH : LOW);

  // ---- publish telemetry every 2 s ----
  if (now - lastPublish >= 2000) {
    lastPublish = now;
    char msg[192];
    snprintf(msg, sizeof(msg),
      "{\"moisture\":%d,\"temp\":%.1f,\"humidity\":%.0f,\"pump\":%d,\"auto\":%d,\"low\":%d,\"high\":%d,\"up\":%lu}",
      moisture, temp, hum, pumpOn ? 1 : 0, autoMode ? 1 : 0, onBelow, offAbove, now / 1000UL);
    if (mqtt.connected()) mqtt.publish(TOPIC_DATA, msg);
    Serial.println(msg);
  }
  delay(100);
}
