#include <PubSubClient.h>
#include <WiFiEsp.h>

#define PIN_BUZZ 14

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

void subscribeTopic(char* topic){

  int qos = 1;
  if(mqtt_client.subscribe(topic,qos)){
    Serial.print("[MQTT] Subscribed to topic -> ");
    Serial.println(topic);
  }else{
    Serial.print("[MQTT] Error subscibing to topic -> ");
    Serial.println(topic);
  }
}

void buzz_ON() {

  digitalWrite(PIN_BUZZ,LOW);
  delay(50); // Con este valor modificamos la duración del zumbido
  digitalWrite(PIN_BUZZ,HIGH);

}

void buzz_OFF() {

  digitalWrite(PIN_BUZZ, HIGH);

}

void alarm() {

  buzz_ON();
  buzz_OFF();

}

void setup(){

  Serial.begin(9600);

  setupWifi();

  setup_mqtt();

  int qos = 0;

  mqtt_client.subscribe("test", qos);

 }

void loop(){

  if (!mqtt_client.connected()) {
    reconnect();
  }else {
    mqtt_client.loop();
  }

}
