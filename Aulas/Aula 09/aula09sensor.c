// 1. Definição e nomeação dos Pinos
const int PINO_LDR = A0;   // Sensor de luz no pino A0
const int PINO_TEMP = A1;  // Sensor de temperatura TMP36 no pino A1
const int PINO_PIR = 2;    // Sensor de presença no pino Digital 2

void setup() {
  // Inicializa a comunicação serial a 9600 bps para exibir os dados no computador
  Serial.begin(9600);
  
  // Configura o pino digital do PIR como pino de entrada (INPUT)
  pinMode(PINO_PIR, INPUT);
}

void loop() {
  // 2. Leitura dos dados brutos dos sensores
  int valorLDR = analogRead(PINO_LDR);     // Leitura analógica (retorna de 0 a 1023)
  int valorLM35 = analogRead(PINO_TEMP);   // Leitura analógica (retorna de 0 a 1023)
  int valorPIR = digitalRead(PINO_PIR);    // Leitura digital (retorna 0 ou 1)

  // 3. Exibição das leituras no Monitor Serial
  Serial.print("LDR (Luz Bruta): ");
  Serial.print(valorLDR);
  Serial.print(" | LM35 (Temp Bruta): ");
  Serial.print(valorTMP36);
  Serial.print(" | PIR (Presenca): ");
  Serial.println(valorPIR);

  // Pausa de 1 segundo (1000 ms) antes do próximo ciclo de leitura
  delay(1000);
}

