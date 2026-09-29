// ============================================================================
// AULA 09 - ETAPA 3: O DESAFIO DO PROTAGONISMO
// Sistema Integrado de Telemetria e Monitoramento Ambiental
// ============================================================================

// 1. Mapeamento de Pinos do Hardware
const int PINO_LDR  = A0; // Sensor de Luz (Analógico - Divisor de Tensão)
const int PINO_TEMP = A1; // Sensor de Temperatura TMP36 (Analógico)
const int PINO_PIR  = 2;  // Sensor de Presença PIR (Digital)
const int PINO_TRIG = 8;  // Sensor Ultrassônico HC-SR04 (Disparo/Gatilho)
const int PINO_ECHO = 7;  // Sensor Ultrassônico HC-SR04 (Recepção/Eco)

// ============================================================================
// 2. Funções Modulares de Condicionamento de Sinal
// ============================================================================

/**
 * Converte a leitura bruta do ADC (0 a 1023) do TMP36 para Graus Celsius.
 * Cálculo: Tensão (V) = leituraBruta * (5.0 / 1023.0)
 * Temp (°C) = (Tensão - 0.5V offset) * 100
 */
float condicionarTemperatura(int leituraBruta) {
    float tensao = leituraBruta * (5.0 / 1023.0);
    return (tensao - 0.5) * 100.0;
}

/**
 * Converte a leitura bruta do ADC (0 a 1023) do LDR em Porcentagem (0 a 100%).
 */
int condicionarLuminosidade(int leituraBruta) {
    return (int)((leituraBruta / 1023.0) * 100.0);
}

/**
 * Emite um pulso de 10us no pino TRIG e mede a duração do eco no pino ECHO,
 * convertendo o tempo de viagem do som para distância em centímetros.
 */
long lerDistanciaUltrassonica() {
    digitalWrite(PINO_TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(PINO_TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(PINO_TRIG, LOW);

    long duracaoDoPulso = pulseIn(PINO_ECHO, HIGH);
    return duracaoDoPulso / 58; // Divisão baseada na velocidade do som (cm)
}

// ============================================================================
// 3. Configuração Inicial do Sistema (Setup)
// ============================================================================
void setup() {
    // Inicializa a comunicação serial a 9600 bps para transmissão de telemetria
    Serial.begin(9600);

    // Configuração dos modos dos pinos digitais
    pinMode(PINO_PIR, INPUT);
    pinMode(PINO_TRIG, OUTPUT);
    pinMode(PINO_ECHO, INPUT);
}

// ============================================================================
// 4. Ciclo Principal de Execução (Loop)
// ============================================================================
void loop() {
    // A. Aquisição dos Dados Brutos dos Sensores
    int leituraBrutaLDR  = analogRead(PINO_LDR);
    int leituraBrutaTEMP = analogRead(PINO_TEMP);
    int estadoPIR        = digitalRead(PINO_PIR);

    // B. Processamento e Condicionamento de Sinal (Chama as funções)
    int luzPorcentagem     = condicionarLuminosidade(leituraBrutaLDR);
    float temperaturaCelsius = condicionarTemperatura(leituraBrutaTEMP);
    long distanciaCm       = lerDistanciaUltrassonica();

    // C. Lógica Condicional de Avaliação do PIR
    String statusSeguranca;
    if (estadoPIR == HIGH) {
        statusSeguranca = "ALERTA: Presenca Detectada";
    } else {
        statusSeguranca = "Area Segura";
    }

    // D. Exibição da Telemetria Formatada no Monitor Serial
    Serial.print("Temp: ");
    Serial.print(temperaturaCelsius, 2);
    Serial.print(" C | Luz: ");
    Serial.print(luzPorcentagem);
    Serial.print("% | Dist: ");
    Serial.print(distanciaCm);
    Serial.print(" cm | Status: ");
    Serial.println(statusSeguranca);

    // E. Estabilização do Ciclo de Leitura (1 segundo)
    delay(1000);
}