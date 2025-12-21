#include <ESP32Servo.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// UUIDs BLE (podes mudar)
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"

// BLE global
BLECharacteristic *pCharacteristicRX;

// Motors
const byte dirM1 = 33;
const byte enM1  = 14;
const byte stepM1 = 32;

const int M1_stepsPerRev = 200;
const float M1_gearRatio = 20.0;

const byte dirM2 = 26;
const byte enM2  = 27;
const byte stepM2 = 25;
const int M2_stepsPerRev = 200;

// Servo
Servo garra;
const int servoPin = 17;
const int GAR_OPEN  = 105;
const int GAR_CLOSE = 88;
const int GAR_STOP  = 90;

const int pulseWidthMicros = 5;

// Vel
const float RPM_output = 60.0;

unsigned long microsBetweenStepsM1;
unsigned long microsBetweenStepsM2;

String bleCommand = "";

unsigned long calcMicrosBetweenSteps(float RPM_output, int stepsPerRev, float gearRatio = 1.0) {
  float RPM_motor = RPM_output * gearRatio;
  float stepsPerSecond = (RPM_motor / 60.0) * stepsPerRev;
  float periodMicros = 1e6 / stepsPerSecond;
  long lowTime = periodMicros - pulseWidthMicros;
  if(lowTime < 1) lowTime = 1;
  return lowTime;
}

void moveMotor(byte dirPin, byte stepPin, byte enPin, int steps, bool direction, unsigned long microsBetweenSteps) {
  digitalWrite(enPin, HIGH);
  digitalWrite(dirPin, direction ? HIGH : LOW);

  for (int i = 0; i < steps; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(pulseWidthMicros);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(microsBetweenSteps);
  }
}

void rotateM1(float degrees) {
  degrees = constrain(degrees, 0, 180);
  int steps = round(degrees * (M1_stepsPerRev / 360.0) * M1_gearRatio);
  moveMotor(dirM1, stepM1, enM1, steps, 0, microsBetweenStepsM1);
}

void launchBall() {
  int M2_steps_96 = round(96.0 * (M2_stepsPerRev / 360.0));
  moveMotor(dirM2, stepM2, enM2, M2_steps_96, 0, microsBetweenStepsM2);
  garra.write(GAR_OPEN);
  delay(1000);
  garra.write(GAR_STOP);
  moveMotor(dirM2, stepM2, enM2, M2_steps_96, 1, microsBetweenStepsM2);
}


// BLE callback – quando recebemos caracteres
class MyCallback: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    String rxValue = pCharacteristic->getValue();
    rxValue.trim();
    if(rxValue.length() > 0){
      bleCommand = rxValue;
    }
  }
};


void setup() {
  Serial.begin(115200);

  // BLE
  BLEDevice::init("ESP32_ROBOT_BLE");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristicRX = pService->createCharacteristic(
                       CHARACTERISTIC_UUID_RX,
                       BLECharacteristic::PROPERTY_WRITE
                     );

  pCharacteristicRX->setCallbacks(new MyCallback());
  pService->start();
  BLEDevice::startAdvertising();
  Serial.println("BLE pronto. A escrever comandos na característica RX.");

  // M1
  pinMode(dirM1, OUTPUT);
  pinMode(enM1, OUTPUT);
  pinMode(stepM1, OUTPUT);
  digitalWrite(enM1, HIGH);

  // M2
  pinMode(dirM2, OUTPUT);
  pinMode(enM2, OUTPUT);
  pinMode(stepM2, OUTPUT);
  digitalWrite(enM2, HIGH);

  // Garra
  garra.attach(servoPin);
  garra.write(GAR_STOP);
  delay(300);

  microsBetweenStepsM1 = calcMicrosBetweenSteps(RPM_output, M1_stepsPerRev, M1_gearRatio);
  microsBetweenStepsM2 = calcMicrosBetweenSteps(RPM_output, M2_stepsPerRev);

  Serial.println("Comandos:");
  Serial.println("R<angulo>");
  Serial.println("L");
  Serial.println("RESET");
}


void loop() {

  if(bleCommand.length() > 0){
    String cmd = bleCommand;
    bleCommand = "";

    if (cmd.startsWith("R")) {
      float ang = cmd.substring(1).toFloat();
      ang = round(ang);
      rotateM1(ang);
      garra.write(GAR_CLOSE);
      delay(1700);
      garra.write(GAR_STOP);
      Serial.println("Motor1_OK");
    }

    else if (cmd == "L") {
      launchBall();
      Serial.println("Launch_OK");
    }

    else if(cmd == "RESET"){
      garra.write(GAR_STOP);
      Serial.println("Reset_OK");
    }

    else {
      Serial.println("Invalid");
    }
  }
}
