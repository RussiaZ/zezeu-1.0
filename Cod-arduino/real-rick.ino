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

// Página HTML
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Controle ESP8266</title>
  <style>
    body { font-family: Arial; text-align: center; margin: 0; padding: 0; background-color: #f0f0f0; }
    button { padding: 10px 20px; margin: 10px; font-size: 16px; background-color: #007bff; color: white; border: none; border-radius: 5px; cursor: pointer; }
    button:hover { background-color: #0056b3; }
    label { display: block; margin: 10px 0; }
    input[type="range"] { width: 80%; }
  </style>
</head>
<body>
  <h1>Controle do ESP8266</h1>
  <button onclick="toggleLed(1)">LED 1</button>
  <button onclick="toggleLed(2)">LED 2</button>
  <h2>Controle dos Motores</h2>
  <button onclick="setMotorDirection(1, 1)">Motor 1 Frente</button>
  <button onclick="setMotorDirection(1, 0)">Motor 1 Trás</button>
  <button onclick="stopMotor(1)">Parar Motor 1</button>
  <br>
  <button onclick="setMotorDirection(2, 1)">Motor 2 Frente</button>
  <button onclick="setMotorDirection(2, 0)">Motor 2 Trás</button>
  <button onclick="stopMotor(2)">Parar Motor 2</button>
  <h2>Controle dos Servos</h2>
  <label>Servo 1: <input type="range" min="0" max="180" id="servo1" onchange="setServo(1)"></label>
  <label>Servo 2: <input type="range" min="0" max="180" id="servo2" onchange="setServo(2)"></label>
  <label>Servo 3: <input type="range" min="0" max="180" id="servo3" onchange="setServo(3)"></label>
  <script>
    function toggleLed(led) {
      fetch(`/toggleLed?led=${led}`);
    }
    function setMotorDirection(motor, direction) {
      fetch(`/setMotorDirection?motor=${motor}&direction=${direction}`);
    }
    function stopMotor(motor) {
      fetch(`/stopMotor?motor=${motor}`);
    }
    function setServo(servo) {
      const angle = document.getElementById(`servo${servo}`).value;
      fetch(`/setServo?servo=${servo}&angle=${angle}`);
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
  Serial.println("Servos configurados com sucesso.");

  // Configuração Wi-Fi
  WiFi.mode(WIFI_AP); // Define o modo AP (ponto de acesso)
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

  server.on("/setMotorDirection", HTTP_GET, []() {
    if (server.hasArg("motor") && server.hasArg("direction")) {
      int motor = server.arg("motor").toInt();
      int direction = server.arg("direction").toInt();
      if (motor == 1) {
        digitalWrite(motor1In1Pin, direction);
        digitalWrite(motor1In2Pin, !direction);
      } else if (motor == 2) {
        digitalWrite(motor2In1Pin, direction);
        digitalWrite(motor2In2Pin, !direction);
      }
    }
    server.send(200, "text/plain", "OK");
  });

  server.on("/stopMotor", HTTP_GET, []() {
    if (server.hasArg("motor")) {
      int motor = server.arg("motor").toInt();
      if (motor == 1) {
        digitalWrite(motor1In1Pin, LOW);
        digitalWrite(motor1In2Pin, LOW);
      } else if (motor == 2) {
        digitalWrite(motor2In1Pin, LOW);
        digitalWrite(motor2In2Pin, LOW);
      }
    }
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
