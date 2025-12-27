#include <ESP32Servo.h>

// Servo
Servo servo;
const byte servoPin = 17;

// Servo contínuo
const int SERVO_OPEN  = 100;
const int SERVO_CLOSE = 87;
const int SERVO_STOP  = 90;

// Reed Switch
const byte reedGarra = 4;   // NO + INPUT_PULLUP
// HIGH = fim de abertura
// LOW  = ainda não chegou

// Tempos
const unsigned long OPEN_TIMEOUT_MS = 3000; // máx tempo a abrir
const unsigned long CLOSE_TIME_MS   = 900; // fecho ligeiro
const unsigned long PAUSE_TIME_MS   = 1000;  // pausas

void setup() {
  Serial.begin(115200);

  servo.attach(servoPin);
  servo.write(SERVO_STOP);

  pinMode(reedGarra, INPUT_PULLUP);

  Serial.println("Garra com protecao por timeout (reed HIGH) iniciada");
}

void loop() {

  // Abrir garra
  Serial.println("A abrir garra...");
  servo.write(SERVO_OPEN);

  unsigned long t0 = millis();

  while (true) {

    // >>> LOGICA INVERTIDA <<<
    if (digitalRead(reedGarra) == HIGH) {
      Serial.println("Fim de abertura detetado (HIGH)");
      break;
    }

    if (millis() - t0 > OPEN_TIMEOUT_MS) {
      Serial.println("TIMEOUT de abertura!");
      break;
    }

    delay(5);
  }

  // parar
  servo.write(SERVO_STOP);
  delay(PAUSE_TIME_MS);

  // Fechar
  Serial.println("A fechar ligeiramente...");
  servo.write(SERVO_CLOSE);
  delay(CLOSE_TIME_MS);

  // Parar
  servo.write(SERVO_STOP);
  delay(PAUSE_TIME_MS);
}
