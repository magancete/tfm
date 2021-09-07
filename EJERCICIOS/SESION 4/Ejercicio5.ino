#include <PubSubClient.h>
#include <WiFiEsp.h>

#define PIN_BUZZ 14

#define PIN_MOTOR_FRONT_LEFT 7
#define PIN_MOTOR_FRONT_RIGHT 12
#define PIN_MOTOR_BACK_LEFT 8
#define PIN_MOTOR_BACK_RIGHT 11

#define PIN_SPEED_RIGHT 3
#define PIN_SPEED_LEFT 6

int SPEED = 120;
#define SPEED_LEFT 255
#define SPEED_RIGHT 255
#define BACK_SPEED 200

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

  switch (c){
    case 'A':
    go_Advance();
    break;
    case 'L':
    go_Left();
    break;
    case 'R':
    go_Right();
    break;
    case 'B':
    go_Back();
    break;
    case 'S':
    stop_Stop();
    break;
    case 'G':
    SPEED=SPEED+10;
    set_Motorspeed(SPEED,SPEED);
    break;
    case 'J':
    SPEED=SPEED-10;
    set_Motorspeed(SPEED,SPEED);
    break;
    default:
    break;
  }

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

void go_Advance()  //Forward
{
  digitalWrite(PIN_MOTOR_FRONT_RIGHT, HIGH);
  digitalWrite(PIN_MOTOR_BACK_RIGHT,LOW);
  digitalWrite(PIN_MOTOR_FRONT_LEFT,HIGH);
  digitalWrite(PIN_MOTOR_BACK_LEFT,LOW);
  set_Motorspeed(SPEED,SPEED);
}
void go_Left()  //Turn left
{
  digitalWrite(PIN_MOTOR_FRONT_RIGHT, HIGH);
  digitalWrite(PIN_MOTOR_BACK_RIGHT,LOW);
  digitalWrite(PIN_MOTOR_FRONT_LEFT,LOW);
  digitalWrite(PIN_MOTOR_BACK_LEFT,HIGH);
  set_Motorspeed(0,SPEED_RIGHT);
}
void go_Right()  //Turn right
{
  digitalWrite(PIN_MOTOR_FRONT_RIGHT, LOW);
  digitalWrite(PIN_MOTOR_BACK_RIGHT,HIGH);
  digitalWrite(PIN_MOTOR_FRONT_LEFT,HIGH);
  digitalWrite(PIN_MOTOR_BACK_LEFT,LOW);
  set_Motorspeed(SPEED_LEFT,0);
}
void go_Back()  //Reverse
{
  digitalWrite(PIN_MOTOR_FRONT_RIGHT, LOW);
  digitalWrite(PIN_MOTOR_BACK_RIGHT,HIGH);
  digitalWrite(PIN_MOTOR_FRONT_LEFT,LOW);
  digitalWrite(PIN_MOTOR_BACK_LEFT,HIGH);
  set_Motorspeed(BACK_SPEED,BACK_SPEED);
}
void stop_Stop()    //Stop
{
  digitalWrite(PIN_MOTOR_FRONT_RIGHT, LOW);
  digitalWrite(PIN_MOTOR_BACK_RIGHT,LOW);
  digitalWrite(PIN_MOTOR_FRONT_LEFT,LOW);
  digitalWrite(PIN_MOTOR_BACK_LEFT,LOW);
  set_Motorspeed(0,0);
}

void set_Motorspeed(int speed_L,int speed_R) {

  analogWrite(PIN_SPEED_LEFT,speed_L);
  analogWrite(PIN_SPEED_RIGHT,speed_R);

}

void setup(){

  pinMode(PIN_MOTOR_FRONT_RIGHT, OUTPUT);
  pinMode(PIN_MOTOR_BACK_RIGHT, OUTPUT);
  pinMode(PIN_SPEED_RIGHT, OUTPUT);

  pinMode(PIN_MOTOR_FRONT_LEFT, OUTPUT);
  pinMode(PIN_MOTOR_BACK_LEFT, OUTPUT);
  pinMode(PIN_SPEED_LEFT, OUTPUT);

  Serial.begin(9600);

  setupWifi();

  setup_mqtt();

  int qos = 0;

  mqtt_client.subscribe("drive", qos);

 }

void loop(){

  if (!mqtt_client.connected()) {
    reconnect();
  }else {
    mqtt_client.loop();
  }

}
