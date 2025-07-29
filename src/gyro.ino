#include <WiFi.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <DFRobot_WT61PC.h>

// MQTT credentials
const char* mqtt_server = "xxx";
const int mqtt_port = xxx;
const char* mqtt_user = "xxx";
const char* mqtt_password = "xxx";
const char* mqtt_topic = "xxx";

// Serial untuk sensor (UART1: GPIO4=RX, GPIO5=TX)
#define RXD2 4
#define TXD2 5
HardwareSerial mySerial(1);
DFRobot_WT61PC sensor(&mySerial);

// MQTT client
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);

  // Inisialisasi sensor serial
  mySerial.begin(9600, SERIAL_8N1, RXD2, TXD2);

  // WiFi Manager
  WiFiManager wifiManager;
  wifiManager.autoConnect("ESP32-SensorAP");

  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  // MQTT setup
  client.setServer(mqtt_server, mqtt_port);
}

void reconnect_mqtt() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect_mqtt();
  }
  client.loop();

  if (sensor.available()) {
    String payload = "{";
    payload += "\"acc_x\":" + String(sensor.Acc.X, 2) + ",";
    payload += "\"acc_y\":" + String(sensor.Acc.Y, 2) + ",";
    payload += "\"acc_z\":" + String(sensor.Acc.Z, 2) + ",";
    payload += "\"gyro_x\":" + String(sensor.Gyro.X, 2) + ",";
    payload += "\"gyro_y\":" + String(sensor.Gyro.Y, 2) + ",";
    payload += "\"gyro_z\":" + String(sensor.Gyro.Z, 2) + ",";
    payload += "\"angle_x\":" + String(sensor.Angle.X, 2) + ",";
    payload += "\"angle_y\":" + String(sensor.Angle.Y, 2) + ",";
    payload += "\"angle_z\":" + String(sensor.Angle.Z, 2);
    payload += "}";

    Serial.println(payload);
    client.publish(mqtt_topic, payload.c_str());
  }

  delay(100);  // sesuaikan dengan kecepatan pembacaan data
}
