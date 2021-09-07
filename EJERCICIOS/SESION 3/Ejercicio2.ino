
#include "WiFiEsp.h"
#include <WiFiEspUdp.h>

WiFiEspUDP Udp;
unsigned int localPort = 8888;

// Los puertos a los que está conectado el ESP
#define SOFT_RX 4 // RX
#define SOFT_TX 5 // TX

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

SoftwareSerial Serial1_(SOFT_RX, SOFT_TX);

int status = WL_IDLE_STATUS;

// Cambiar por los valores reales
char ssid[] = "******";
char pass[] = "******";

// Buffer UDP
char packetBuffer[5];

void setup_wifi(){

  Serial1_.begin(115200);    // Inicializa la comunicación serial con el módulo ESP
  Serial1_.print("AT+CIOBAUD=9600\r\n");
  Serial1_.write("AT+RST\r\n");
  Serial1_.begin(9600);    // Inicializa la comunicación serial con el módulo ESP

  WiFi.init(&Serial1_);    // Inicializa el módulo ESP

  if (WiFi.status() == WL_NO_SHIELD) {
    Serial_.println("No se ha conectado un WiFi shield!");
  }

 while ( status != WL_CONNECTED) {
  Serial_.print("Intentando conectar al SSID: ");
  Serial_.println(ssid);
  status = WiFi.begin(ssid, pass);
 }

  IPAddress ip = WiFi.localIP();
  Serial_.println("Conectado con la dirección IP: ");
  Serial_.println(ip);

  Udp.begin(localPort);

  Serial_.print("Escuchando en el puerto ");
  Serial_.println(localPort);

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

  Serial_.begin(9600);

  pinMode(PIN_MOTOR_FRONT_RIGHT, OUTPUT);
  pinMode(PIN_MOTOR_BACK_RIGHT, OUTPUT);
  pinMode(PIN_SPEED_RIGHT, OUTPUT);

  pinMode(PIN_MOTOR_FRONT_LEFT, OUTPUT);
  pinMode(PIN_MOTOR_BACK_LEFT, OUTPUT);
  pinMode(PIN_SPEED_LEFT, OUTPUT);

  setup_wifi();

  stop_Stop();

}

void loop() {

  int packetSize = Udp.parsePacket();
  if (packetSize) {

    alarm();

    Serial_.print("Paquete recibido de tamaño ");
    Serial_.println(packetSize);

    int len = Udp.read(packetBuffer, 255);

    if (len > 0) {
      packetBuffer[len] = 0;
    }
      char c=packetBuffer[0];

      Serial_.print("Caracter recibido -> " + c);

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

}
