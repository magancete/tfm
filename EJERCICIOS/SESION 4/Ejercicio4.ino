#include <PubSubClient.h>
#include <WiFiEsp.h>

WiFiEspClient espClient;

PubSubClient mqtt_client(espClient);

const char* ssid = "ssid";
const char* pass = "pass";

void setupWifi(){

  Serial1.begin(115200);
  Serial1.write("AT+CIOBAUD=9600\r\n");
  Serial1.write("AT+RST\r\n");
  Serial1.begin(9600);
  WiFi.init(&Serial1);
  // Check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial1.println("WiFi shield not present");
    while (true);
  }
  // Attempt to connect to WiFi network
  while ( wifi_status != WL_CONNECTED) {
    wifi_status = WiFi.begin(ssid, pass);
  }
  delay(1500);

}

const char* mqtt_server = "192.168.1.25";

 void callback(char* topic, byte* payload, unsigned int length) {

  alarm();

  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

  Serial.println();
  char c = payload[0];

 }

 void setup_mqtt(){

  mqtt_client.setServer(mqtt_server, 1883);
  mqtt_client.setCallback(callback);

}

bool reconnect(){

  Serial.println("[MQTT] Attempting MQTT connection...");
  if (mqtt_client.connect("OSOYOO")) {
    Serial.println("[MQTT] Connected");
  }else {
      Serial.print("[MQTT] Failed, rc=");
      Serial.println(mqtt_client.state());
  }
  return mqtt_client.connected();

}

void setup(){

  Serial.begin(9600);

  setupWifi();

  setup_mqtt();

 }

void loop(){

  if (!mqtt_client.connected()) {
    reconnect();
  }else {
    mqtt_client.loop();
  }

  mqtt_client.publish("test", "Hola Mundo!");

  delay(5000);

}
