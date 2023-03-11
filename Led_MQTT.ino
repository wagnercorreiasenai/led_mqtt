//Declaração de variáveis
int ledStatus = 2;
int led = 22;
int btn1 = 23;
int estadoLed = 0;

// variáveis para o botão:
int ultimoEstado = 1;  //o estado anterior do pino de entrada
int estadoAtual;       //a leitura atual do pino de entrada

void setup() {

  //Ativando saída serial
  Serial.begin(9600);

  //Ativando principais portas
  pinMode(btn1, INPUT_PULLUP);
  pinMode(led, OUTPUT);

  //Informação de Placa ligada
  pinMode(ledStatus, OUTPUT);
  digitalWrite(ledStatus, HIGH);
}

void alternarLed() {
  if(estadoLed == 0) {
    //Liga o led
    estadoLed = 1;
    digitalWrite(led, HIGH);
  } else {
    //Desliga o led
    estadoLed = 0;
    digitalWrite(led, LOW);
  }
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
  delay(50);
}
