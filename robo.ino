#define SEGUIR_LINHA 0
#define CURVA 1
#define PARADA 2

#define LED_DEBUG 13
#define IR_CONTADOR_LINHA 8
#define IR_OBSTACULO 2
#define IR_SEGUIDOR_DIREITO 7
#define IR_SEGUIDOR_ESQUERDO 12
#define PWM_MOTOR_DIREITO 3 
#define PWM_MOTOR_ESQUERDO 11
#define MOTOR_DIREITO_IN_0 5 
#define MOTOR_DIREITO_IN_1 6 
#define MOTOR_ESQUERDO_IN_0 9
#define MOTOR_ESQUERDO_IN_1 10

int estado = SEGUIR_LINHA;
int iCount = 0;

volatile unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 300;

void setup() {
  pinMode(LED_DEBUG, OUTPUT);
  pinMode(IR_CONTADOR_LINHA, INPUT);
  pinMode(IR_OBSTACULO, INPUT);
  pinMode(IR_SEGUIDOR_DIREITO, INPUT);
  pinMode(IR_SEGUIDOR_ESQUERDO, INPUT);

  pinMode(PWM_MOTOR_DIREITO, OUTPUT);
  pinMode(PWM_MOTOR_ESQUERDO, OUTPUT);
  pinMode(MOTOR_DIREITO_IN_0, OUTPUT);
  pinMode(MOTOR_DIREITO_IN_1, OUTPUT);
  pinMode(MOTOR_ESQUERDO_IN_0, OUTPUT);
  pinMode(MOTOR_ESQUERDO_IN_1, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(IR_OBSTACULO), obstaculo, CHANGE);
  attachInterrupt(digitalPinToInterrupt(IR_CONTADOR_LINHA), contador_linha, RISING);
  Serial.begin(9600);
}

void loop() {
  switch (estado) {
    case SEGUIR_LINHA:
      seguidor_de_linha();
      break;
    case CURVA:
      realizar_a_curva();
      break;
    case PARADA:
      parar();
      break;
  }
}

void seguidor_de_linha() {
  int direita = digitalRead(IR_SEGUIDOR_DIREITO);
  int esquerda = digitalRead(IR_SEGUIDOR_ESQUERDO);

  if (direita == HIGH && esquerda == LOW) {
    moverMotores(0, 220);
  } else if (direita == LOW && esquerda == HIGH) {
    moverMotores(220, 0);
  } else if (direita == LOW && esquerda == LOW) {
    moverMotores(100, -100); 
  } else if (direita == HIGH && esquerda == HIGH) {
    moverMotores(220, 220);
  }
}

void realizar_a_curva() {
  moverMotores(-255, 255);
  delay(1000);
  estado = SEGUIR_LINHA;
}

void parar() {
  digitalWrite(MOTOR_DIREITO_IN_0, LOW);
  digitalWrite(MOTOR_DIREITO_IN_1, LOW);
  digitalWrite(MOTOR_ESQUERDO_IN_0, LOW);
  digitalWrite(MOTOR_ESQUERDO_IN_1, LOW);
}

void contador_linha() {
  unsigned long currentTime = millis();

  if ((currentTime - lastDebounceTime) > debounceDelay) {
    iCount++;
    lastDebounceTime = currentTime;

    if (iCount == 2) {
      estado = CURVA;
    }
  }
}

void obstaculo() {
  int sensorObstaculo = digitalRead(IR_OBSTACULO);

  if (sensorObstaculo == LOW) {
  estado = PARADA;
  } else {
  estado = SEGUIR_LINHA;
  }
}

void moverMotores(int velDireito, int velEsquerdo) {
  if (velDireito > 0) {
    analogWrite(PWM_MOTOR_DIREITO, velDireito);
    digitalWrite(MOTOR_DIREITO_IN_0, HIGH);
    digitalWrite(MOTOR_DIREITO_IN_1, LOW);
  } else if (velDireito < 0) {
    analogWrite(PWM_MOTOR_DIREITO, -velDireito);
    digitalWrite(MOTOR_DIREITO_IN_0, LOW);
    digitalWrite(MOTOR_DIREITO_IN_1, HIGH);
  } else {
    digitalWrite(MOTOR_DIREITO_IN_0, LOW);
    digitalWrite(MOTOR_DIREITO_IN_1, LOW);
  }

  if (velEsquerdo > 0) {
    analogWrite(PWM_MOTOR_ESQUERDO, velEsquerdo);
    digitalWrite(MOTOR_ESQUERDO_IN_0, HIGH);
    digitalWrite(MOTOR_ESQUERDO_IN_1, LOW);
  } else if (velEsquerdo < 0) {
    analogWrite(PWM_MOTOR_ESQUERDO, -velEsquerdo);
    digitalWrite(MOTOR_ESQUERDO_IN_0, LOW);
    digitalWrite(MOTOR_ESQUERDO_IN_1, HIGH);
  } else {
    digitalWrite(MOTOR_ESQUERDO_IN_0, LOW);
    digitalWrite(MOTOR_ESQUERDO_IN_1, LOW);
  }
}
