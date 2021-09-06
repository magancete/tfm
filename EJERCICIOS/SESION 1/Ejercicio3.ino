#define LED 13

void setup() {

  pinMode(LED, OUTPUT);

  Serial.begin(9600);

  digitalWrite(LED, LOW);

}

void loop() {

  if (Serial.available())  {

    char c = Serial.read();

    if (c == 'Y'){
      digitalWrite(LED, HIGH);
      Serial.println("LED ENCENDIDO!");

    }else if(c == 'N'){
      digitalWrite(LED, LOW);
      Serial.println("LED APAGADO!");
    }

  }

}
