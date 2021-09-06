#define LED 13

void setup() {

  pinMode(LED, OUTPUT);

  Serial.begin(9600);

}

void loop() {

  digitalWrite(LED, HIGH);
  Serial.println("LED ENCENDIDO!");
  delay(1000);
  digitalWrite(LED, LOW);
  delay(5000);

}
