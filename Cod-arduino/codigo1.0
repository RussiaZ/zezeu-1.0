#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

// Configuração Wi-Fi
const char* ssid = "ROBO Zezeu-1.0"; // Nome da rede Wi-Fi
const char* password = "98968133";  // Senha da rede Wi-Fi

// Servidor Web
ESP8266WebServer server(80);

// Configuração dos pinos da ponte H L298N
const int motor1In1Pin = 5;  // GPIO5 (D1)
const int motor1In2Pin = 4;  // GPIO4 (D2)
const int motor2In1Pin = 0;  // GPIO0 (D3)
const int motor2In2Pin = 2;  // GPIO2 (D4)

// LEDs
const int led1Pin = 14; // GPIO14 (D5)
const int led2Pin = 12; // GPIO12 (D6)

// Servos
Servo servo1, servo2, servo3;
const int servo1Pin = 13; // GPIO13 (D7)
const int servo2Pin = 15; // GPIO15 (D8)
const int servo3Pin = 3;  // GPIO3 (RX)


// Página HTML responsiva
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Controle ESP8266</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      margin: 0;
      padding: 0;
      text-align: center;
      background-color: #282c34;
      color: white;
    }
    .controller {
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      height: 100vh;
    }
    .buttons {
      display: grid;
      grid-template-columns: repeat(3, 1fr);
      gap: 10px;
    }
    .buttons button {
      padding: 20px;
      font-size: 18px;
      background-color: #61dafb;
      border: none;
      border-radius: 5px;
      cursor: pointer;
      transition: background-color 0.3s;
    }
    .buttons button:active {
      background-color: #21a1f1;
    }
    .sliders {
      margin-top: 20px;
    }
    .sliders label {
      display: block;
      margin: 10px 0;
    }
    .sliders input {
      width: 80%;
    }
  </style>
</head>
<body>
  <div class="controller">
    <h1>Controle do ESP8266</h1>
    <div class="buttons">
      <button onmousedown="setMotor('frente')" onmouseup="stopMotors()">Frente</button>
      <button onmousedown="setMotor('esquerda')" onmouseup="stopMotors()">Esquerda</button>
      <button onmousedown="setMotor('direita')" onmouseup="stopMotors()">Direita</button>
      <button onmousedown="setMotor('tras')" onmouseup="stopMotors()">Trás</button>
    </div>
    <div class="sliders">
      <label>Servo 1: <input type="range" min="0" max="180" id="servo1" onchange="setServo(1)"></label>
      <label>Servo 2: <input type="range" min="0" max="180" id="servo2" onchange="setServo(2)"></label>
      <label>Servo 3: <input type="range" min="0" max="180" id="servo3" onchange="setServo(3)"></label>
      <button onclick="toggleLed(1)">Alternar LED 1</button>
      <button onclick="toggleLed(2)">Alternar LED 2</button>
    </div>
  </div>
  <script>
    function setMotor(direction) {
      fetch(`/setMotor?direction=${direction}`);
    }
    function stopMotors() {
      fetch('/stopMotors');
    }
    function setServo(servo) {
      const angle = document.getElementById(`servo${servo}`).value;
      fetch(`/setServo?servo=${servo}&angle=${angle}`);
    }
    function toggleLed(led) {
      fetch(`/toggleLed?led=${led}`);
    }
  </script>
</body>
</html>
)rawliteral";

// Variáveis de controle
int led1State = 0;
int led2State = 0;
int servo1Angle = 90;
int servo2Angle = 90;
int servo3Angle = 90;

void setup() {
  Serial.begin(115200);

  // Configuração dos pinos dos motores
  pinMode(motor1In1Pin, OUTPUT);
  pinMode(motor1In2Pin, OUTPUT);
  pinMode(motor2In1Pin, OUTPUT);
  pinMode(motor2In2Pin, OUTPUT);

  // Configuração dos LEDs
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);

  // Configuração dos servos
  servo1.attach(servo1Pin);
  servo2.attach(servo2Pin);
  servo3.attach(servo3Pin);
  servo1.write(servo1Angle);
  servo2.write(servo2Angle);
  servo3.write(servo3Angle);

  // Configuração Wi-Fi
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  // Aguarda estabilização
  delay(1000);

  // Log no Serial
  Serial.println("Servidor iniciado!");
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Configuração do servidor
  server.on("/", HTTP_GET, []() {
    server.send_P(200, "text/html", index_html);
  });

  server.on("/toggleLed", HTTP_GET, []() {
    if (server.hasArg("led")) {
      int led = server.arg("led").toInt();
      if (led == 1) {
        led1State = !led1State;
        digitalWrite(led1Pin, led1State);
      } else if (led == 2) {
        led2State = !led2State;
        digitalWrite(led2Pin, led2State);
      }
    }
    server.send(200, "text/plain", "OK");
  });

  server.on("/setMotor", HTTP_GET, []() {
    if (server.hasArg("direction")) {
      String direction = server.arg("direction");
      if (direction == "frente") {
        digitalWrite(motor1In1Pin, HIGH);
        digitalWrite(motor1In2Pin, LOW);
        digitalWrite(motor2In1Pin, HIGH);
        digitalWrite(motor2In2Pin, LOW);
      } else if (direction == "tras") {
        digitalWrite(motor1In1Pin, LOW);
        digitalWrite(motor1In2Pin, HIGH);
        digitalWrite(motor2In1Pin, LOW);
        digitalWrite(motor2In2Pin, HIGH);
      } else if (direction == "esquerda") {
        digitalWrite(motor1In1Pin, LOW);
        digitalWrite(motor1In2Pin, HIGH);
        digitalWrite(motor2In1Pin, HIGH);
        digitalWrite(motor2In2Pin, LOW);
      } else if (direction == "direita") {
        digitalWrite(motor1In1Pin, HIGH);
        digitalWrite(motor1In2Pin, LOW);
        digitalWrite(motor2In1Pin, LOW);
        digitalWrite(motor2In2Pin, HIGH);
      }
    }
    server.send(200, "text/plain", "OK");
  });

  server.on("/stopMotors", HTTP_GET, []() {
    digitalWrite(motor1In1Pin, LOW);
    digitalWrite(motor1In2Pin, LOW);
    digitalWrite(motor2In1Pin, LOW);
    digitalWrite(motor2In2Pin, LOW);
    server.send(200, "text/plain", "OK");
  });

  server.on("/setServo", HTTP_GET, []() {
    if (server.hasArg("servo") && server.hasArg("angle")) {
      int servo = server.arg("servo").toInt();
      int angle = server.arg("angle").toInt();
      if (servo == 1) servo1.write(angle);
      if (servo == 2) servo2.write(angle);
      if (servo == 3) servo3.write(angle);
    }
    server.send(200, "text/plain", "OK");
  });

  // Inicia o servidor
  server.begin();
}

void loop() {
  server.handleClient();
}
