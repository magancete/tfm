#include <PubSubClient.h>
#include <WiFiEsp.h>

#define PIN_BUZZ 14

#define LFSensor_0 A0
#define LFSensor_1 A1
#define LFSensor_2 A2
#define LFSensor_3 A3
#define LFSensor_4 A4

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

char frontal_sensor[5];

void auto_tracking() {

  String sensorval = read_front_sensor_values();

  if (sensorval == "11000" || sensorval == "10000" ||
      sensorval ==
          "01000") // The black line is in the left of the car, need  left turn
  {

    go_Left();
    set_Motorspeed(FAST_SPEED, FAST_SPEED);
  } else if (sensorval == "10100" || sensorval == "00100" ||
             sensorval == "01100" || sensorval == "11100" ||
             sensorval == "10010" || sensorval == "11010") {
    go_Advance();
    set_Motorspeed(MID_SPEED, FAST_SPEED);

  } else if (sensorval == "00011" || sensorval == "00001" ||
             sensorval == "00010") { // The black line is  on the right of the
                                     // car, need  right turn

    go_Right();
    set_Motorspeed(FAST_SPEED, FAST_SPEED);
  } else if (sensorval == "00101" || sensorval == "00110" ||
             sensorval == "00111" || sensorval == "01101" ||
             sensorval == "01111" || sensorval == "01011" ||
             sensorval == "01110" || sensorval == "01001") {
    go_Advance();
    set_Motorspeed(FAST_SPEED, MID_SPEED);
  } else if (sensorval == "01110" || sensorval == "00100") {
    go_Advance();
    set_Motorspeed(SPEED, SPEED);
  } else if (sensorval == "11111") {
    stop_Stop();
    set_Motorspeed(0, 0);
  }

  else if (sensorval == "00000") {

    stop_Stop();
    go_Advance();
    set_Motorspeed(SPEED, SPEED);
  }
}

/* Read sensor value string, 1 stands for black, 0 starnds for white, i.e 10000
 * means the first sensor(from left) detect black line, other 4 sensors detected
 * white ground */
String read_front_sensor_values() {

  int sensorvalue = 32;

  frontal_sensor[0] = digitalRead(LFSensor_0);

  frontal_sensor[1] = digitalRead(LFSensor_1);

  frontal_sensor[2] = digitalRead(LFSensor_2);

  frontal_sensor[3] = digitalRead(LFSensor_3);

  frontal_sensor[4] = digitalRead(LFSensor_4);

  sensorvalue += frontal_sensor[0] * 16 + frontal_sensor[1] * 8 + frontal_sensor[2] * 4 +
                 frontal_sensor[3] * 2 + frontal_sensor[4];

  sensorvalue = ~sensorvalue;
  String senstr = String(sensorvalue, BIN);
  senstr = senstr.substring(11, 16);

  return senstr;
}


char sensor[5];

String read_sensor_values(){

  int sensorvalue = 32; // Corresponde con 11111

  sensor[0]= !digitalRead(LFSensor_0);

  sensor[1]=!digitalRead(LFSensor_1);

  sensor[2]=!digitalRead(LFSensor_2);

  sensor[3]=!digitalRead(LFSensor_3);

  sensor[4]=!digitalRead(LFSensor_4);

  sensorvalue += sensor[0] * 16 + sensor[1] *8 + sensor[2]  *4 + sensor[3] * 2 + sensor[4];

  String senstr= String(sensorvalue,BIN);

  senstr=senstr.substring(1,6);

  return senstr;

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

  auto_tracking();

 }

void loop(){

  if (!mqtt_client.connected()) {
    reconnect();
  }else {
    mqtt_client.loop();
  }

}
