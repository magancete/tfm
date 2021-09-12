 /* INCLUDES */
#include <PubSubClient.h>
#include <Servo.h>
#include <WiFiEsp.h>
#include <WiFiEspUdp.h>
#include "SoftwareSerial.h"
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif
#include <TinyGPS.h>

/* CONST VAR */

#define PIN_LED 13

//#define PIN_BACK_IR_RIGHT 4
//#define PIN_BACK_IR_LEFT 5

#define PIN_BUZZ A5

#define PIN_ULTRASONIC_TRIG 4
#define PIN_ULTRASONIC_ECHO 5

#define PIN_SERVO 9

#define PIN_MPU_INTERRUPT 2
 
#define PIN_MOTOR_FRONT_LEFT 7
#define PIN_MOTOR_FRONT_RIGHT 12
#define PIN_MOTOR_BACK_LEFT 8
#define PIN_MOTOR_BACK_RIGHT 11

#define PIN_SPEED_RIGHT 3
#define PIN_SPEED_LEFT 6

#define LFSensor_0 A0
#define LFSensor_1 A1
#define LFSensor_2 A2
#define LFSensor_3 A3
#define LFSensor_4 A4

#define GPS_SOFT_RX 15 // GPS Softserial RX port
#define GPS_SOFT_TX 14 // GPS Softserial TX port

#define LEFT_TURN_TIME 300
#define RIGHT_TURN_TIME 300
#define AHEAD_TIME 300
#define BACK_TIME 500

#define FAST_SPEED 180
#define MID_SPEED 130
int SPEED = 120; // speed
#define SPEED_LEFT 255
#define SPEED_RIGHT 255
#define BACK_SPEED 200 // back speed
#define TURN_SPEED 200 // back speed

String WorkMode = "?";

//SoftwareSerial Serial2(15, 14); // RX, TX

/* WIFI SETTINGS */
WiFiEspClient espClient;
int wifi_status = WL_IDLE_STATUS;
WiFiEspUDP Udp;
unsigned int localPort = 8888;
char packetBuffer[5];

/* MQTT SETTINGS */
PubSubClient mqtt_client(espClient);
long lastMsg = 0;
String clientId = "OSOYOO-";
long lastReconnectAttempt = 0;

/* GPS SETTINGS */
TinyGPS gps;
String gpsPosition = "";

/* MPU SETTINGS */
MPU6050 mpu;

/* SERVO SETTINGS */
Servo head;

/* STATE MACHINE */
enum DS {
  MANUAL_DRIVE,
  AUTO_DRIVE_LF, // line follow
  AUTO_DRIVE_UO  // ultrasonic obstruction
} Drive_Status = MANUAL_DRIVE;

enum DN {
  GO_ADVANCE,
  GO_LEFT,
  GO_RIGHT,
  GO_BACK,
  STOP_STOP
} Drive_Num = STOP_STOP;


void setup() {

  /* INITIALICE PINS */

  Serial.begin(9600);  
  Serial2.begin(9600); 

  //pinMode(PIN_BACK_IR_RIGHT, INPUT);
  //pinMode(PIN_BACK_IR_LEFT, INPUT);

  pinMode(PIN_MOTOR_FRONT_RIGHT, OUTPUT);
  pinMode(PIN_MOTOR_BACK_RIGHT, OUTPUT);
  pinMode(PIN_SPEED_RIGHT, OUTPUT);

  pinMode(PIN_MOTOR_FRONT_LEFT, OUTPUT);
  pinMode(PIN_MOTOR_BACK_LEFT, OUTPUT);
  pinMode(PIN_SPEED_LEFT, OUTPUT);

  pinMode(PIN_BUZZ, OUTPUT);
  digitalWrite(PIN_BUZZ, HIGH);
  buzz_OFF();

  stop_Stop(); // stop move

  digitalWrite(PIN_ULTRASONIC_TRIG, LOW);
  head.attach(PIN_SERVO);
  head.write(90);

  /* SETUP MPU */
  setupMpu();

  /* SETUP WIFI */
  setupWifi();

  /* SETUP MQTT */
  setupMqtt();    

  alarm();

} 

void loop() {        
  
  long now = millis();

  if (now - lastReconnectAttempt > 5000) {
    lastReconnectAttempt = now;
    if (!mqtt_client.connected()) {
      if (reconnect()) {
        lastReconnectAttempt = 0;
      }
    }
  }else {
    mqtt_client.loop();
  }

  delay(200);  

  car_control();   

  send_sensor_values();

}

void car_control() {

    switch (Drive_Status) {

    case MANUAL_DRIVE:

      switch (Drive_Num) {
      case GO_ADVANCE:
        go_Advance();
        break;
      case GO_LEFT:
        go_Left();
        break;
      case GO_RIGHT:
        go_Right();
        break;
      case GO_BACK:
        go_Back();
        break;
      case STOP_STOP:
        stop_Stop();
        break;
      default:
        break;
      }
      break;

    case AUTO_DRIVE_LF:
      auto_tracking();
      break;

    case AUTO_DRIVE_UO:
      auto_avoidance();
      break;
    }  
}

void getMode(DS ds) {
  switch (ds) {
  case MANUAL_DRIVE:
    Serial.print("MANUAL_DRIVE");
    break;
  case AUTO_DRIVE_LF:
    Serial.print("AUTO_DRIVE_LF");
    break;
  case AUTO_DRIVE_UO:
    Serial.print("AUTO_DRIVE_UO");
    break;
  default:
    Serial.print("STATE_NOT_DEFINED");
    break;
  }
}

void changeMode(DS ds) {
  Serial.print("[STATE MACHINE] Status changed | ");
  getMode(Drive_Status);
  Serial.print(" -> ");
  getMode(ds);
  Serial.println();
  Drive_Status = ds;
}
