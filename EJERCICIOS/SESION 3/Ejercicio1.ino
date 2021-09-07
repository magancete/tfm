
#include "WiFiEsp.h"
#include <WiFiEspUdp.h>

WiFiEspUDP Udp;
unsigned int localPort = 8888;

// Los puertos a los que está conectado el ESP
#define SOFT_RX 4 // RX
#define SOFT_TX 5 // TX

#define PIN_BUZZ 14

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

void setup(){

  Serial_.begin(9600);
  setup_wifi();

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
  }

}
