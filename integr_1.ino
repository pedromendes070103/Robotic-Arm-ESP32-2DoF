#include <ESP32Servo.h> 

// Config do motor 1 da base (com redutor)
const byte dirM1 = 33;
const byte enM1  = 14;
const byte stepM1 = 32;

const int M1_stepsPerRev = 200;
const float M1_gearRatio = 20.0;    // redutor 1:20

// Config do motor 2 do "meio" (sem redutor)
const byte dirM2 = 26;
const byte enM2  = 27;
const byte stepM2 = 25;

const int M2_stepsPerRev = 200;

// Servo - garra
Servo garra;
const int servoPin = 17;

// Posições da garra
const int GAR_OPEN  = 100;   
const int GAR_CLOSE = 87;    
const int GAR_STOP  = 90;    

const int pulseWidthMicros = 5;  // datasheet: >2.5µs

// Velocidade a definir pelo utilizador
const float RPM_output = 60.0;

// Variáveis globais
unsigned long microsBetweenStepsM1;
unsigned long microsBetweenStepsM2;

// Função para calcular tempo entre passos  (ms)
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

void setup() {
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
  delay(500);

  // Calcular tempos entre passos
  microsBetweenStepsM1 = calcMicrosBetweenSteps(RPM_output, M1_stepsPerRev, M1_gearRatio);
  microsBetweenStepsM2 = calcMicrosBetweenSteps(RPM_output, M2_stepsPerRev);
}

void loop() {
  // Step 1 - M1 gira + 90º
  int M1_steps_90 = round(90.0 * (M1_stepsPerRev / 360.0) * M1_gearRatio);
  moveMotor(dirM1, stepM1, enM1, M1_steps_90, 0, microsBetweenStepsM1);
  delay(300);

  // Step 2 - Garra fecha um pouco
  garra.write(GAR_CLOSE);
  delay(1100);
  garra.write(GAR_STOP);
  delay(500);

  // Step 3 - M2 gira +96º
  int M2_steps_96 = round(96.0 * (M2_stepsPerRev / 360.0));
  moveMotor(dirM2, stepM2, enM2, M2_steps_96, 0, microsBetweenStepsM2);
  delay(300);

  // Step 4 - Abrir garra
  garra.write(GAR_OPEN);
  delay(1500);
  garra.write(GAR_STOP);
  delay(300);

  // Step 5 - Voltar às posições iniciais
  moveMotor(dirM2, stepM2, enM2, M2_steps_96, 1, microsBetweenStepsM2);
  delay(300);
  moveMotor(dirM1, stepM1, enM1, M1_steps_90, 1, microsBetweenStepsM1);
  delay(300);

  // Tempo entre repetições
  delay(2000);
}
