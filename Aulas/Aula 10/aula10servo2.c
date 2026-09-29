#include <Servo.h>
 
const int PINO_SERVO = 9;
const int PINO_RELE = 4;
const int PINO_MOTOR_PWM = 10;
const float LIMIAR_TEMP = 28.0; // graus Celsius - a partir daqui o motor liga
 
Servo meuServo;
 
// Converte o percentual de luz (0-100) em angulo do servo (0-180)
void moverServoPorLuz(int luzPercentual) {
  int angulo = map(luzPercentual, 0, 100, 0, 180);
  meuServo.write(angulo);
}
 
// Converte temperatura em duty cycle do motor DC, respeitando o limiar
void controlarMotorPorTemperatura(float tempCelsius) {
  int pwm = 0;
  if (tempCelsius > LIMIAR_TEMP) {
    pwm = map((int)tempCelsius, (int)LIMIAR_TEMP, 50, 80, 255);
    pwm = constrain(pwm, 0, 255); // nunca deixa o PWM sair de 0-255
  }
  analogWrite(PINO_MOTOR_PWM, pwm);
}
 
// Liga ou desliga o rele conforme o estado do PIR
void controlarRelePorPresenca(int presenca) {
  digitalWrite(PINO_RELE, presenca == HIGH ? HIGH : LOW);
}
 
void setup() {
  Serial.begin(9600);
  meuServo.attach(PINO_SERVO);
  pinMode(PINO_RELE, OUTPUT);
  pinMode(PINO_MOTOR_PWM, OUTPUT);
}
 
void loop() {
  // Dados ficticios de teste (validando a logica antes de integrar com a Aula 09)
  int luzSimulada = 70;
  float tempSimulada = 32.5;
  int presencaSimulada = HIGH;
 
  moverServoPorLuz(luzSimulada);
  controlarMotorPorTemperatura(tempSimulada);
  controlarRelePorPresenca(presencaSimulada);
 
  Serial.print("Servo: "); Serial.print(map(luzSimulada, 0, 100, 0, 180)); Serial.println(" graus");
  Serial.print("Motor: "); Serial.println(tempSimulada > LIMIAR_TEMP ? "ativo" : "desligado");
  Serial.print("Rele: "); Serial.println(presencaSimulada == HIGH ? "LIGADO" : "DESLIGADO");
  Serial.println("-------------------------");
 
  delay(1500);
}

