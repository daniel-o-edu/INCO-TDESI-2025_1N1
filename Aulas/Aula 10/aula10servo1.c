#include <Servo.h>
 
const int PINO_SERVO = 9;      // Saida PWM - servomotor
const int PINO_RELE = 4;       // Saida digital - rele
const int PINO_MOTOR_PWM = 10; // Saida PWM - base do transistor (via resistor 1k)
 
Servo meuServo;
 
void setup() {
  Serial.begin(9600);
  meuServo.attach(PINO_SERVO);
  pinMode(PINO_RELE, OUTPUT);
  pinMode(PINO_MOTOR_PWM, OUTPUT);
}
 
// Voce deve rodar este laco e observar o movimento dos atuadores no simulador
void loop() {
  // Teste do servo: varre de 0 a 180 graus
  for (int angulo = 0; angulo <= 180; angulo += 30) {
    meuServo.write(angulo);
    Serial.print("Servo: "); Serial.println(angulo);
    delay(500);
  }
 
  // Teste do rele: liga e desliga
  digitalWrite(PINO_RELE, HIGH);
  Serial.println("Rele: LIGADO");
  delay(1000);
  digitalWrite(PINO_RELE, LOW);
  Serial.println("Rele: DESLIGADO");
  delay(1000);
 
  // Teste do motor DC via PWM (rampa de velocidade)
  for (int pwm = 0; pwm <= 255; pwm += 51) {
    analogWrite(PINO_MOTOR_PWM, pwm);
    Serial.print("Motor PWM: "); Serial.println(pwm);
    delay(500);
  }
  analogWrite(PINO_MOTOR_PWM, 0);
}
