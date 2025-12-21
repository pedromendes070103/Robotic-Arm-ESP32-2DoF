// Reed switches (NO + INPUT_PULLUP)
const byte reedGarra = 4;    // fim abertura garra
const byte reedM2    = 16;   // home M2
const byte reedM1    = 5;    // home M1

bool lastGarra = HIGH;
bool lastM2    = HIGH;
bool lastM1    = HIGH;

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(reedGarra, INPUT_PULLUP);
  pinMode(reedM2, INPUT_PULLUP);
  pinMode(reedM1, INPUT_PULLUP);

  Serial.println("TESTE DE REED SWITCHES");
  Serial.println("Aproxime um iman a cada sensor...");
}

void loop() {
  bool g = digitalRead(reedGarra);
  bool m2 = digitalRead(reedM2);
  bool m1 = digitalRead(reedM1);

  // Garra
  if (g != lastGarra) {
    Serial.print("Garra: ");
    Serial.println(g == LOW ? "DETETADO" : "LIVRE");
    lastGarra = g;
  }

  // M2
  if (m2 != lastM2) {
    Serial.print("M2: ");
    Serial.println(m2 == LOW ? "DETETADO" : "LIVRE");
    lastM2 = m2;
  }

  // M1
  if (m1 != lastM1) {
    Serial.print("M1: ");
    Serial.println(m1 == LOW ? "DETETADO" : "LIVRE");
    lastM1 = m1;
  }

  delay(20); // pequeno debounce + evita spam
}
