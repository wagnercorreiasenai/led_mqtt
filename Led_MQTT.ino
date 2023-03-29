//Bibliotecas
#include "ArduinoJson.h"
#include "EspMQTTClient.h"

//Declaração de variáveis
int ledStatus = 2;
int led = 22;
int btn1 = 23;
int estadoLed = 0;

// variáveis para o botão:
int ultimoEstado = 1;  //o estado anterior do pino de entrada
int estadoAtual;       //a leitura atual do pino de entrada

//-------------------------------------------------//
//configurações da conexão MQTT
EspMQTTClient client
(
  "FIESC_IOT", //nome da sua rede Wi-Fi
  "7FuhM4@Io9", //senha da sua rede Wi-Fi
  "mqtt.tago.io",  // MQTT Broker server ip padrão da tago
  "Default",   // username
  "",   // Código do Token DA PLATAFORMA TAGOIO
  "senaiesp001",     // Client name that uniquely identify your device
  1883              // The MQTT port, default to 1883. this line can be omitted
);

//Tópico
char topicoMqtt[] = "senai/esp32/001";

/*
Esta função é chamada assim que tudo estiver conectado (Wifi e MQTT)]
AVISO: VOCÊ DEVE IMPLEMENTÁ-LO SE USAR EspMQTTClient
*/ 
void onConnectionEstablished()
{
    client.subscribe(topicoMqtt,onMessageReceived);
}

void onMessageReceived(const String &msg) {
    digitalWrite(ledStatus, LOW);
    Serial.println("Mensagem recebida:");
    Serial.println(msg);

    StaticJsonBuffer<300> JSONBuffer;                         //Memory pool
    JsonObject& parsed = JSONBuffer.parseObject(msg); //Parse message

    if (parsed.success()) {
      int ledMqtt = parsed["led"];

      if (ledMqtt == 1) {
        ligarLed();
        Serial.println("Led ligado por MQTT");
      } else if (ledMqtt == 2) {
        desligarLed();
        Serial.println("Led desligado por MQTT");
      }

    } else {
      Serial.println("Falha ao realizar parsing do JSON");
    }
}

void transmitirStatusLed() {
  StaticJsonBuffer<300> JSONbuffer;
  JsonObject& JSONencoder = JSONbuffer.createObject();
  JSONencoder["statusLed"] = estadoLed;

  char JSONmessageBuffer[300];
  JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));

  client.publish(topicoMqtt, JSONmessageBuffer);
}

void setup() {

  //Ativando saída serial
  Serial.begin(9600);

  //Ativando principais portas
  pinMode(btn1, INPUT_PULLUP);
  pinMode(led, OUTPUT);

  //Informação de Placa ligada
  pinMode(ledStatus, OUTPUT);
  digitalWrite(ledStatus, LOW);
}

void alternarLed() {
  if(estadoLed == 0) {
    //Liga o led
    estadoLed = 1;
    digitalWrite(led, HIGH);
    Serial.println("Led ligado por botão");
  } else {
    //Desliga o led
    estadoLed = 0;
    digitalWrite(led, LOW);
    Serial.println("Led desligado por botão");
  }

  transmitirStatusLed();
}

void ligarLed() {
  estadoLed = 1;
  digitalWrite(led, HIGH);
  transmitirStatusLed();
}

void desligarLed() {
  estadoLed = 0;
  digitalWrite(led, LOW);
  transmitirStatusLed();
}

void loop() {

  estadoAtual = digitalRead(btn1);

  if (ultimoEstado == HIGH && estadoAtual == LOW) {
    Serial.println("Botão precionado");
    alternarLed();
  } else if (ultimoEstado == LOW && estadoAtual == HIGH) {
    Serial.println("Botão liberado");
  }

  ultimoEstado = estadoAtual;

  //Alternar o led "azul" do ESP32 conforme estado na conexão
  if (client.isWifiConnected()) {
    digitalWrite(ledStatus, HIGH);
  } else {
    digitalWrite(ledStatus, LOW);
  }

  client.loop();
  delay(10);
}
