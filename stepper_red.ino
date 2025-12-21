const byte directionPin = 33;
const byte enablePin    = 14;
const byte stepPin      = 32;
const int stepsPerRev = 200;  // passos por volta do MOTOR
const float gearRatio = 20.0; // redutor 1:20
// Escolhido pelo utilizador
const float degreesToMove = 90.0;
const float RPM_output = 10.0;
// Datasheet: > 2.5 micro s high
const int pulseWidthMicros = 5; 
int steps;
unsigned long microsBetweenStepsLow;

void setup() 
{
  pinMode(enablePin, OUTPUT);
  pinMode(directionPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  // Ativa motores
  digitalWrite(enablePin, HIGH);
  
  // Calcular nr de passos c/Redutor
  steps = round(degreesToMove * (stepsPerRev / 360.0) * gearRatio);

  // Velocidade motor = RPM_motor = RPM_saida * gearRatio
  float RPM_motor = RPM_output * gearRatio;

  // Calcular frequencia (0-200kHz)
  float stepsPerSecond = (RPM_motor / 60.0) * stepsPerRev;

  // Período total em micro s
  float periodMicros = 1e6 / stepsPerSecond;

  // Tempo LOW
  microsBetweenStepsLow = periodMicros - pulseWidthMicros;
  if(microsBetweenStepsLow < 1){
    microsBetweenStepsLow = 1; // proteção
  }
}

void loop()
{
  digitalWrite(directionPin, LOW);
  moveSteps(steps);

  delay(200);

  digitalWrite(directionPin, HIGH);
  moveSteps(steps);

  delay(200);
}

void moveSteps(int n)
{
  for (int i = 0; i < n; i++) 
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(pulseWidthMicros);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(microsBetweenStepsLow);
  }
}