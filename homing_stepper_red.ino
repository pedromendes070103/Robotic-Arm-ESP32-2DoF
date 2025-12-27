// Pinos
const byte directionPin = 33;
const byte enablePin    = 14;
const byte stepPin      = 32;
const byte reedStepper  = 5;   // INPUT_PULLUP

// Motor/Redutor
const int stepsPerRev = 200;
const float gearRatio = 20.0;

// Homing
const bool HOMING_DIR = LOW;      // direção onde está o reed
const float HOMING_RPM = 5.0;
const float HOMING_MAX_DEG = 360.0;

// ===== UTILIZADOR =====
float RPM_output = 10.0;

// Datasheet
const int pulseWidthMicros = 5;

//Variables
unsigned long microsBetweenStepsLow;

void updateSpeed(float rpm_out)
{
  float RPM_motor = rpm_out * gearRatio;
  float stepsPerSecond = (RPM_motor / 60.0) * stepsPerRev;
  float periodMicros = 1e6 / stepsPerSecond;

  microsBetweenStepsLow = periodMicros - pulseWidthMicros;
  if (microsBetweenStepsLow < 1) microsBetweenStepsLow = 1;
}

void doStep()
{
  digitalWrite(stepPin, HIGH);
  delayMicroseconds(pulseWidthMicros);
  digitalWrite(stepPin, LOW);
  delayMicroseconds(microsBetweenStepsLow);
}

// Homing process
bool doHoming()
{
  Serial.println("Homing iniciado");
  updateSpeed(HOMING_RPM);

  int maxSteps = round(HOMING_MAX_DEG * (stepsPerRev / 360.0) * gearRatio);

  // Procura homing
  digitalWrite(directionPin, HOMING_DIR);
  for (int i = 0; i < maxSteps; i++)
  {
    if (digitalRead(reedStepper) == HIGH)
    {
      Serial.println("HOME encontrado");
      return true;
    }
    doStep();
  }

  // Não encontrou, volta para trás
  Serial.println("Home nao encontrado, a voltar...");
  digitalWrite(directionPin, !HOMING_DIR);

  for (int i = 0; i < maxSteps; i++)
  {
    doStep();
  }

  Serial.println("Falha no homing");
  return false;
}

void setup()
{
  Serial.begin(115200);

  pinMode(enablePin, OUTPUT);
  pinMode(directionPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(reedStepper, INPUT_PULLUP);

  digitalWrite(enablePin, HIGH);

  if (!doHoming())
  {
    Serial.println("ATENCAO: homing falhou");
  }

  updateSpeed(RPM_output);
  Serial.println("Sistema pronto para comandos");
}

void loop()
{
  if (!Serial.available()) return;

  String cmd = Serial.readStringUntil('\n');
  cmd.trim();
  cmd.toUpperCase();

  // R<angulo> 
  if (cmd.startsWith("R"))
  {
    float deg = cmd.substring(1).toFloat();
    int steps = round(abs(deg) * (stepsPerRev / 360.0) * gearRatio);

    bool dir = (deg >= 0) ? HIGH : LOW;
    digitalWrite(directionPin, dir);

    Serial.println("A mover...");

    for (int i = 0; i < steps; i++)
    {
      if (digitalRead(reedStepper) == HIGH && dir == LOW )
      {
        Serial.println("REED ATIVO — PAROU");
        break;
      }
      doStep();
    }

    Serial.println("Movimento terminado");
  }

  // Vxx
  else if (cmd.startsWith("V"))
  {
    RPM_output = cmd.substring(1).toFloat();
    updateSpeed(RPM_output);
    Serial.println("Velocidade atualizada");
  }


  else
  {
    Serial.println("Comando invalido");
  }
}
