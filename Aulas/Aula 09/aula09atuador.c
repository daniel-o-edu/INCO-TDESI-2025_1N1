// 1. Constantes do Bloco 2
const int PINO_TRIG = 8;
const int PINO_ECHO = 7;

// 2. Funcoes do Bloco 2
float condicionarTemperatura(int leituraBruta) {
    // Converte a leitura do ADC (0-1023) em tensao (0-5V)
    float tensao = leituraBruta * (5.0 / 1023.0);
    // Aplica o offset de 0.5V do TMP36 e converte para Celsius
    return (tensao - 0.5) * 100.0;
}

int condicionarLuminosidade(int leituraBruta) {
    return (int)((leituraBruta / 1023.0) * 100.0);
}

long lerDistanciaUltrassonica() {
    digitalWrite(PINO_TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(PINO_TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(PINO_TRIG, LOW);
    
    long duracaoDoPulso = pulseIn(PINO_ECHO, HIGH); // Captura a largura do pulso
    return duracaoDoPulso / 58; // Converte para centimetros
}

// 3. O Motor de Execucao
void setup() {
    Serial.begin(9600); // Habilita o Monitor Serial
    pinMode(PINO_TRIG, OUTPUT); // Configura o pino de disparo
    pinMode(PINO_ECHO, INPUT);  // Configura o pino de escuta
}

void loop() {
    // Simulando leituras brutas (apenas para validar a conversao matematica)
    int simulacaoLuzBruta = 512; // Valor ficticio (metade de 1023)
    int simulacaoTempBruta = 153; // Valor ficticio para TMP36 (equivale a aprox. 24.8 °C)

    // Chamando as funcoes e armazenando os retornos
    int luzReal = condicionarLuminosidade(simulacaoLuzBruta);
    float tempReal = condicionarTemperatura(simulacaoTempBruta);
    long distanciaReal = lerDistanciaUltrassonica();

    // Imprimindo os resultados no console
    Serial.print("Luz Simulada: "); Serial.print(luzReal); Serial.println("%");
    Serial.print("Temp Simulada: "); Serial.print(tempReal); Serial.println(" C");
    Serial.print("Distancia Real (Sensor): "); Serial.print(distanciaReal); Serial.println(" cm");
    Serial.println("-------------------------");
    
    delay(1500); // Pausa para facilitar a visualizacao[cite: 2]
}