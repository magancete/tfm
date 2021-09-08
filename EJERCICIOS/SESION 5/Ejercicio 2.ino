#include <PubSubClient.h>
#include <WiFiEsp.h>
#include <Servo.h>

#define PIN_BUZZ 14

#define PIN_ULTRASONIC_TRIG 4
#define PIN_ULTRASONIC_ECHO 5

#define PIN_SERVO 9

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

Servo head;

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

int leftscanval;
int centerscanval;
int rightscanval;
int ldiagonalscanval;
int rdiagonalscanval;

int numcycles = 0;

const int distancelimit = 30; // Distancia límite de parada
const int sidedistancelimit = 30; // Distancia límite de parada

int thereis=0;

const int loop_counter = 2; // Cuantas veces deber analizar el servo

const int turntime = 250; // Time the robot spends turning (miliseconds)
const int backtime = 300; // Time the robot spends turning (miliseconds)

int calc_distancia(){

  long echo_distance;
  digitalWrite(PIN_ULTRASONIC_TRIG,LOW);
  delayMicroseconds(5);
  digitalWrite(PIN_ULTRASONIC_TRIG,HIGH);
  delayMicroseconds(15);
  digitalWrite(PIN_ULTRASONIC_TRIG,LOW);

  echo_distance=pulseIn(PIN_ULTRASONIC_ECHO,HIGH);

  echo_distance=echo_distance*0.01657; // Devuelve la distancia en cm

  return round(echo_distance);

}

//Meassures distances to the right, left, front, left diagonal, right diagonal and asign them in cm to the variables rightscanval,
//leftscanval, centerscanval, ldiagonalscanval and rdiagonalscanval (there are 5 points for distance testing)
String watchsurrounding(){
/*  obstacle_status is a binary integer, its last 5 digits stands for if there is any obstacles in 5 directions,
 *   for example B101000 last 5 digits is 01000, which stands for Left front has obstacle, B100111 means front, right front and right ha
 */

int obstacle_status = B100000;

  centerscanval = calc_distancia();

  if(centerscanval < distancelimit){
    stop_Stop();
    alarm();
    obstacle_status  = obstacle_status | B100;
  }

  head.write(120);
  delay(100);

  ldiagonalscanval = calc_distancia();

  if(ldiagonalscanval<distancelimit){
    stop_Stop();
    alarm();
    obstacle_status = obstacle_status | B1000;
  }

  head.write(170);
  delay(300);

  leftscanval = calc_distancia();

  if(leftscanval<sidedistancelimit){
    stop_Stop();
    alarm();
    obstacle_status  =obstacle_status | B10000;
  }

  head.write(90);
  delay(100);

  centerscanval = calc_distancia();

  if(centerscanval<distancelimit){
    stop_Stop();
    alarm();
    obstacle_status = obstacle_status | B100;
  }

  head.write(40);
  delay(100);

  rdiagonalscanval = calc_distancia();

  if(rdiagonalscanval<distancelimit){
    stop_Stop();
    alarm();
    obstacle_status = obstacle_status | B10;
  }

  head.write(0);
  delay(100);

  rightscanval = calc_distancia();

  if(rightscanval<sidedistancelimit){
    stop_Stop();
    alarm();
    obstacle_status = obstacle_status | 1;
  }

  head.write(90);
  delay(300);

  String obstacle_str = String(obstacle_status,BIN);
  obstacle_str= obstacle_str.substring(1,6);

  return obstacle_str;

}

void auto_avoidance(){

  ++numcycles;

  if(numcycles >= loop_counter){

    stop_Stop();
    String obstacle_sign = watchsurrounding(); // 5 digits of obstacle_sign binary value means the 5 direction obstacle status

    if( obstacle_sign=="10000"){
     set_Motorspeed(FAST_SPEED,SPEED);
     go_Advance();
     delay(turntime);
     stop_Stop();
    }else if( obstacle_sign=="00001"  ){
      set_Motorspeed(SPEED,FAST_SPEED);
      go_Advance();
      delay(turntime);
      stop_Stop();
    }
    else if( obstacle_sign=="11100" || obstacle_sign=="01000" || obstacle_sign=="11000"  || obstacle_sign=="10100"  || obstacle_sign=="01100" ||obstacle_sign=="00100"  ||obstacle_sign=="01000" ){
	    go_Right();
      set_Motorspeed(TURN_SPEED,TURN_SPEED);
      delay(turntime);
      stop_Stop();
    }
    else if( obstacle_sign=="00010" || obstacle_sign=="00111" || obstacle_sign=="00011"  || obstacle_sign=="00101" || obstacle_sign=="00110" || obstacle_sign=="01010" ){
      go_Left();//Turn left
      set_Motorspeed(TURN_SPEED,TURN_SPEED);
      delay(turntime);
      stop_Stop();
    }else if(  obstacle_sign=="01111" ||  obstacle_sign=="10111" || obstacle_sign=="11111"  ){
   	  go_Left();
		  set_Motorspeed( FAST_SPEED,SPEED);
      delay(backtime);
      stop_Stop();
    }else if( obstacle_sign=="11011"  ||    obstacle_sign=="11101"  ||  obstacle_sign=="11110"  || obstacle_sign=="01110"  ){
      go_Right();
      set_Motorspeed( SPEED,FAST_SPEED);
      delay(backtime);
      stop_Stop();
    }else{
      numcycles = 0;
    }
  } else {
     set_Motorspeed(SPEED,SPEED);
     go_Advance();
     delay(backtime);
     stop_Stop();
  }

  distance = watch();
  if (distance<distancelimit){
    go_Right();
    set_Motorspeed( SPEED,FAST_SPEED);
    delay(backtime*3/2);
      ++thereis;
  }
  if (distance>distancelimit){
      thereis=0;
  }
  if (thereis > 25){
    stop_Stop();
    thereis=0;
  }

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

  auto_avoidance();

 }

void loop(){

  if (!mqtt_client.connected()) {
    reconnect();
  }else {
    mqtt_client.loop();
  }

}
