#include <ESP32Servo.h>
Servo servo;
void setup() {
  servo.attach(17);  // GPIO17
}
void loop() {

  servo.write(100); // OPEN
  delay(1500);
  
  servo.write(90); // STOP
  delay(3000);

  servo.write(87); // CLOSE
  delay(1100);

  servo.write(90); // STOP
  delay(3000);
}