// Simples aplicação

/*const byte directionPin = 26;
const byte enablePin    = 27;
const byte stepPin      = 25;
const int stepsPerRev = 200;
// Escolhido pelo utilizador
const float degreesToMove = 30.0;
const float RPM = 10.0;  
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
  // Calcular nr de passos
  steps = round(degreesToMove * (stepsPerRev / 360.0));
  // Calcular frequencia (0-200kHz)
  float stepsPerSecond = (RPM / 60.0) * stepsPerRev;
  // Período total em micro s
  float periodMicros = 1e6 / stepsPerSecond;
  // Tempo LOW
  microsBetweenStepsLow = periodMicros - pulseWidthMicros;
  if(microsBetweenStepsLow < 1){
    microsBetweenStepsLow = 1; // Proteção
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
}*/

// Controlo via comunicação série com PC:
const byte directionPin = 26;
const byte enablePin    = 27;
const byte stepPin      = 25;

const int stepsPerRev = 200;

// ===== Valores configuráveis =====
float degreesToMove = 30.0;
float RPM = 10.0;
bool direction = LOW;

// Datasheet: > 2.5 µs HIGH
const int pulseWidthMicros = 5;

int steps;
unsigned long microsBetweenStepsLow;

// ===== Atualiza parâmetros =====
void updateMotionParams() {
  steps = round(degreesToMove * (stepsPerRev / 360.0));

  float stepsPerSecond = (RPM / 60.0) * stepsPerRev;
  float periodMicros = 1e6 / stepsPerSecond;

  microsBetweenStepsLow = periodMicros - pulseWidthMicros;
  if (microsBetweenStepsLow < 1) microsBetweenStepsLow = 1;
}

void setup() {
  Serial.begin(115200);

  pinMode(enablePin, OUTPUT);
  pinMode(directionPin, OUTPUT);
  pinMode(stepPin, OUTPUT);

  digitalWrite(enablePin, HIGH);

  updateMotionParams();

  Serial.println("=== CONTROLO STEPPER ===");
  Serial.println("Rxxx  -> roda xxx graus (ex: R180)");
  Serial.println("D     -> muda direcao");
  Serial.println("Vxx   -> velocidade em RPM (ex: V20)");
  Serial.println("RESET -> valores iniciais");
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    cmd.toUpperCase();

    // ---- Rxxx ----
    if (cmd.startsWith("R")) {
      degreesToMove = cmd.substring(1).toFloat();
      updateMotionParams();

      digitalWrite(directionPin, direction);
      moveSteps(steps);

      Serial.print("Rodou ");
      Serial.print(degreesToMove);
      Serial.println(" graus");
    }

    // ---- D ----
    else if (cmd == "D") {
      direction = !direction;
      Serial.print("Direcao: ");
      Serial.println(direction ? "HIGH" : "LOW");
    }

    // ---- Vxx ----
    else if (cmd.startsWith("V")) {
      RPM = cmd.substring(1).toFloat();
      updateMotionParams();

      Serial.print("Velocidade = ");
      Serial.print(RPM);
      Serial.println(" RPM");
    }

    // ---- RESET ----
    else if (cmd == "RESET") {
      degreesToMove = 30.0;
      RPM = 10.0;
      direction = LOW;
      updateMotionParams();

      Serial.println("Reset para valores iniciais");
    }

    else {
      Serial.println("Comando invalido");
    }
  }
}

void moveSteps(int n) {
  for (int i = 0; i < n; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(pulseWidthMicros);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(microsBetweenStepsLow);
  }
}
