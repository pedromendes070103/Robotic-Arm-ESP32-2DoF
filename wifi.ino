#include <ESP32Servo.h>
#include <WiFi.h>
#include <WebServer.h>

// ===================== WIFI =====================
const char* ssid = "ESP32_ROBOT";
const char* password = "12345678";
WebServer server(80);

// ===================== MOTORS =====================
const byte dirM1 = 33;
const byte enM1  = 14;
const byte stepM1 = 32;
const int M1_stepsPerRev = 200;
const float M1_gearRatio = 20.0;

const byte dirM2 = 26;
const byte enM2  = 27;
const byte stepM2 = 25;
const int M2_stepsPerRev = 200;

// ===================== SERVO =====================
Servo gripper;
const int servoPin = 17;
const int GRIP_OPEN  = 105;
const int GRIP_CLOSE = 87;
const int GRIP_STOP  = 90;

// ===================== MOTOR CONTROL =====================
const int pulseWidthMicros = 5;
const float RPM_output = 20.0;
unsigned long microsBetweenStepsM1;
unsigned long microsBetweenStepsM2;

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
  degrees = constrain(degrees, 0, 360);
  int steps = round(degrees * (M1_stepsPerRev / 360.0) * M1_gearRatio);
  moveMotor(dirM1, stepM1, enM1, steps, 0, microsBetweenStepsM1);
  delay(1000);
}

void launchBall() {
  int M2_steps_96 = round(96.0 * (M2_stepsPerRev / 360.0));
  moveMotor(dirM2, stepM2, enM2, M2_steps_96, 0, microsBetweenStepsM2);
  gripper.write(GRIP_OPEN);
  delay(1000);
  gripper.write(GRIP_STOP);
  moveMotor(dirM2, stepM2, enM2, M2_steps_96, 1, microsBetweenStepsM2);
}

// ===================== WEB PAGE =====================
const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Robot Control</title>
  <style>
    body { font-family: Arial; text-align:center; background: #222; color: #fff; }
    h1 { color: #f39c12; }
    input, button { font-size: 1.5em; padding: 10px; margin: 10px; border-radius: 10px; border:none; cursor:pointer; }
    #angle { width: 80px; text-align:center; }
    #Rbtn { background:#3498db; color:white; }
    #Launch { background:#e74c3c; color:white; }
    #Reset { background:#2ecc71; color:white; }
    button:hover { transform: scale(1.05); transition:0.2s; }
  </style>
</head>
<body>
  <h1>Robot Controller</h1>
  <div>
    <input type="number" id="angle" placeholder="0-360">
    <button id="Rbtn" onclick="sendCmd('R')">Rotate M1</button>
  </div>
  <div>
    <button id="Launch" onclick="sendCmd('L')">Launch</button>
    <button id="Reset" onclick="sendCmd('RESET')">Reset</button>
  </div>
  <p id="status">Status: idle</p>
<script>
function sendCmd(cmd){
  let url = '/' + cmd;
  if(cmd=='R'){
    let ang = document.getElementById('angle').value;
    if(ang=='') { alert('Enter angle!'); return; }
    url += '?val='+ang;
  }
  fetch(url)
    .then(resp=>resp.text())
    .then(txt=>document.getElementById('status').innerText='Status: '+txt)
    .catch(err=>document.getElementById('status').innerText='Error');
}
</script>
</body>
</html>
)rawliteral";

// ===================== HANDLERS =====================
void handleRoot() { server.send(200, "text/html", htmlPage); }

void handleR() {
  if(server.hasArg("val")){
    float ang = server.arg("val").toFloat();
    rotateM1(ang);
    gripper.write(GRIP_CLOSE);
    delay(1700);
    gripper.write(GRIP_STOP);
    server.send(200,"text/plain","M1 rotated "+String(ang)+"º");
  } else {
    server.send(400,"text/plain","Angle value missing");
  }
}

void handleLaunch() { launchBall(); server.send(200,"text/plain","Launch OK"); }
void handleReset() { gripper.write(GRIP_STOP); server.send(200,"text/plain","Reset OK"); }

// ===================== SETUP =====================
void setup() {
  Serial.begin(115200);

  // Motor pins
  pinMode(dirM1, OUTPUT); pinMode(enM1, OUTPUT); pinMode(stepM1, OUTPUT); digitalWrite(enM1,HIGH);
  pinMode(dirM2, OUTPUT); pinMode(enM2, OUTPUT); pinMode(stepM2, OUTPUT); digitalWrite(enM2,HIGH);

  // Servo
  gripper.attach(servoPin);
  gripper.write(GRIP_STOP);
  delay(300);

  // Motor timings
  microsBetweenStepsM1 = calcMicrosBetweenSteps(RPM_output, M1_stepsPerRev, M1_gearRatio);
  microsBetweenStepsM2 = calcMicrosBetweenSteps(RPM_output, M2_stepsPerRev);

  // Wi-Fi AP
  WiFi.softAP(ssid, password);
  Serial.println("WiFi AP started: ESP32_ROBOT");

  // Web routes
  server.on("/", handleRoot);
  server.on("/R", handleR);
  server.on("/L", handleLaunch);
  server.on("/RESET", handleReset);
  server.begin();
  Serial.println("Web server started");
}

// ===================== LOOP =====================
void loop() {
  server.handleClient();
}
