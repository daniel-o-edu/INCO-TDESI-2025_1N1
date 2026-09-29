#include <Servo.h>

// ==========================================
// MAPEAMENTO DE PINOS E CONSTANTES
// ==========================================
// Sensores (Aula 09)
const int PINO_LDR  = A0; // Leitura de Luminosidade
const int PINO_TMP  = A1; // Leitura de Temperatura
const int PINO_PIR  = 2;  // Sensor de Presenca
const int PINO_ECHO = 7;  // Ultrassonico - Receptor
const int PINO_TRIG = 8;  // Ultrassonico - Emissor

// Atuadores (Aula 10)
const int PINO_SERVO     = 9;  // Saida PWM - Servomotor
const int PINO_MOTOR_PWM = 10; // Saida PWM - Base do Transistor (Motor CC)
const int PINO_RELE      = 4;  // Saida Digital - Controle do Rele

// Parametros de Decisao
const float LIMIAR_TEMP = 28.0; // Temperatura limite (°C) para acionamento do motor

// Instancia do Servomotor
Servo meuServo;

// ==========================================
// FUNCOES DE CONDICIONAMENTO DE SENSORES (AULA 09)
// ==========================================

// Converte a leitura do LDR em percentual (0 a 100%)
int condicionarLuminosidade(int leituraBruta) {
  int percentual = map(leituraBruta, 0, 1023, 0, 100);
  return constrain(percentual, 0, 100);
}

// Converte a leitura analogica do TMP36 em graus Celsius
float condicionarTemperatura(int leituraBruta) {
  float tensao = (leituraBruta * 5.0) / 1024.0;
  float tempCelsius = (tensao - 0.5) * 100.0;
  return tempCelsius;
}

// Emite o pulso ultrassónico e calcula a distancia em centimetros
long lerDistanciaUltrassonica(int pinoTrig, int pinoEcho) {
  digitalWrite(pinoTrig, LOW);
  delayMicroseconds(2);
  digitalWrite(pinoTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinoTrig, LOW);

  long duracao = pulseIn(pinoEcho, HIGH);
  long distancia = duracao * 0.034 / 2; // Converte o tempo do eco em cm
  return distancia;
}

// ==========================================
// FUNCOES DE ACTUACAO E DECISAO (AULA 10)
// ==========================================

// Ajusta o angulo do servo proporcionalmente a luz (0% = 0°, 100% = 180°)
void moverServoPorLuz(int luzPercentual) {
  int angulo = map(luzPercentual, 0, 100, 0, 180);
  angulo = constrain(angulo, 0, 180);
  meuServo.write(angulo);
}

// Controla a velocidade do motor CC via PWM acima do limiar de temperatura
void controlarMotorPorTemperatura(float tempCelsius) {
  int pwm = 0;
  if (tempCelsius > LIMIAR_TEMP) {
    pwm = map((int)tempCelsius, (int)LIMIAR_TEMP, 50, 80, 255);
    pwm = constrain(pwm, 0, 255);
  }
  analogWrite(PINO_MOTOR_PWM, pwm);
}

// Liga ou desliga o rele em tempo real conforme deteccao de presenca do PIR
void controlarRelePorPresenca(int presenca) {
  digitalWrite(PINO_RELE, presenca == HIGH ? HIGH : LOW);
}

// ==========================================
// CONFIGURACAO INICIAL (SETUP)
// ==========================================
void setup() {
  Serial.begin(9600);

  // Configuração dos pinos dos sensores
  pinMode(PINO_PIR, INPUT);
  pinMode(PINO_TRIG, OUTPUT);
  pinMode(PINO_ECHO, INPUT);

  // Configuração dos pinos dos atuadores
  meuServo.attach(PINO_SERVO);
  pinMode(PINO_RELE, OUTPUT);
  pinMode(PINO_MOTOR_PWM, OUTPUT);
}

// ==========================================
// LACO PRINCIPAL (LOOP)
// ==========================================
void loop() {
  // 1. SENTIR (Leitura e Condicionamento)
  int luzPercentual  = condicionarLuminosidade(analogRead(PINO_LDR));
  float tempCelsius  = condicionarTemperatura(analogRead(PINO_TMP));
  long distanciaCm   = lerDistanciaUltrassonica(PINO_TRIG, PINO_ECHO);
  int presenca       = digitalRead(PINO_PIR);

  // 2. DECIDIR E AGIR (Comando dos Atuadores)
  moverServoPorLuz(luzPercentual);
  controlarMotorPorTemperatura(tempCelsius);
  controlarRelePorPresenca(presenca);

  // 3. TELEMETRIA (Formatação de Saida no Monitor Serial)
  int anguloAtual = constrain(map(luzPercentual, 0, 100, 0, 180), 0, 180);
  int pwmAtual = 0;
  if (tempCelsius > LIMIAR_TEMP) {
    pwmAtual = constrain(map((int)tempCelsius, (int)LIMIAR_TEMP, 50, 80, 255), 0, 255);
  }

  Serial.print("Temp: "); Serial.print(tempCelsius, 2); Serial.print(" C | ");
  Serial.print("Luz: "); Serial.print(luzPercentual); Serial.print("% | ");
  Serial.print("Dist: "); Serial.print(distanciaCm); Serial.print(" cm | ");
  Serial.print("Servo: "); Serial.print(anguloAtual); Serial.print(" graus | ");
  Serial.print("Motor PWM: "); Serial.print(pwmAtual); Serial.print(" | ");
  Serial.print("Rele: "); Serial.print(presenca == HIGH ? "LIGADO" : "DESLIGADO"); Serial.print(" | ");
  Serial.print("Status: ");
  
  if (presenca == HIGH) {
    Serial.println("ALERTA: Presenca Detectada");
  } else {
    Serial.println("Area Segura");
  }

  // Estabilização de ciclo
  delay(1000);
}