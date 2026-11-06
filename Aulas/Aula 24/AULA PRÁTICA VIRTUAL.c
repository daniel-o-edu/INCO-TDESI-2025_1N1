/*
 * AULA PRÁTICA VIRTUAL: Redes Industriais e Robótica Aplicada em IoT
 * Funcionalidades:
 * 1. Parametrização Estática de Rede (MAC, IP Fixo, Gateway e Máscara)
 * 2. Servidor de Borda HTTP na Porta 80 com escuta contínua (client.connected)
 * 3. Aquisição de Dados Ambientais com Sensor LDR
 * 4. Parametrização Proporcional de Atuador Robótico (Servomotor 0° a 180°)
 */

#include <WiFi.h>
#include <ESP32Servo.h> // Parte do pacote de abstração "IoT - Lib completa"

// ============================================================================
// 1. DEFINIÇÃO DE PINOS DE HARDWARE (COMPUTAÇÃO FÍSICA)
// ============================================================================
const int PINO_LDR = 3;   // Entrada analógica (ADC) para leitura luminosa
const int PINO_SERVO = 4; // Saída PWM para controle angular do servomotor

// ============================================================================
// 2. CONFIGURAÇÃO DE CONECTIVIDADE ESTÁTICA (SEM DEPENDER DE DHCP)
// ============================================================================
const char* ssid = "Wokwi-GUEST"; // Rede virtual padrão do simulador
const char* password = "";

// Matriz de endereço físico (MAC Address) de 6 bytes
uint8_t enderecoMAC[6] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x01};

// Endereçamento Lógico Estático (Altere o último octeto conforme seu Nº de Matrícula!)
IPAddress ipFixo(10, 10, 0, 150);
IPAddress gateway(10, 10, 0, 1);
IPAddress mascaraSubrede(255, 255, 255, 0);

// ============================================================================
// 3. INSTANCIAÇÃO DO SERVIDOR DE BORDA (PORTA 80) E ATUADOR ROBÓTICO
// ============================================================================
WiFiServer servidor(80); // Servidor HTTP aguardando requisições na porta 80
Servo atuadorRobotico;   // Objeto de controle do servomotor

int anguloAtual = 0;     // Armazena a posição angular atual (0° a 180°)
int leituraBrutaLDR = 0; // Valor do conversor ADC (0 a 4095 no ESP32)
int luminosidadePct = 0; // Intensidade luminosa convertida (0% a 100%)

// Função auxiliar para processar comandos (via Rede HTTP ou Simulador Serial)
void processarParametroRequisicao(String requisicao) {
  // Busca pelo Request Parameter "angulo=" na string recebida (Ex: GET /?angulo=120)
  int indiceParametro = requisicao.indexOf("angulo=");
  
  if (indiceParametro != -1) {
    // Extrai o valor numérico logo após "angulo="
    String valorExtraido = "";
    for (int i = indiceParametro + 7; i < requisicao.length(); i++) {
      if (isDigit(requisicao[i])) {
        valorExtraido += requisicao[i];
      } else {
        break;
      }
    }

    if (valorExtraido.length() > 0) {
      int novoAngulo = valorExtraido.toInt();
      
      // Validação dos limites mecânicos do servomotor (0° a 180°)
      if (novoAngulo >= 0 && novoAngulo <= 180) {
        anguloAtual = novoAngulo;
        atuadorRobotico.write(anguloAtual); // Aciona o servo proporcionalmente
        Serial.println(">> [ATUADOR] Comando aceito! Eixo posicionado em: " + String(anguloAtual) + " graus.");
      } else {
        Serial.println(">> [ERRO] Parametro fora da faixa segura (Permitido: 0 a 180 graus).");
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println("======================================================");
  Serial.println(" INICIANDO NÓ IoT INDUSTRIAL - SERVIDOR DE BORDA HTTP ");
  Serial.println("======================================================");

  // Configura resolução analógica e inicializa o servomotor
  analogReadResolution(12); // 0 a 4095
  atuadorRobotico.attach(PINO_SERVO, 500, 2400);
  atuadorRobotico.write(anguloAtual); // Posição inicial segura (0°)

  // Aplica a configuração de IP Estático antes de conectar
  if (!WiFi.config(ipFixo, gateway, mascaraSubrede)) {
    Serial.println("[FALHA] Erro ao parametrizar IP Estatico!");
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Conectando a rede sem fio");
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println("\n[REDE CONECTADA COM SUCESSO]");

  // Exibe os parâmetros estáticos configurados
  Serial.printf("Endereco Fisico (MAC Definido): %02X:%02X:%02X:%02X:%02X:%02X\n",
                enderecoMAC[0], enderecoMAC[1], enderecoMAC[2],
                enderecoMAC[3], enderecoMAC[4], enderecoMAC[5]);
  Serial.print("IP Estatico Ativo : "); Serial.println(WiFi.localIP());
  Serial.print("Mascara Sub-rede  : "); Serial.println(WiFi.subnetMask());
  Serial.print("Gateway Padrao    : "); Serial.println(WiFi.gatewayIP());

  // Inicia o servidor HTTP na porta 80
  servidor.begin();
  Serial.println("Servidor HTTP instanciado e escutando na Porta 80.");
  Serial.println("------------------------------------------------------");
  Serial.println("DICA DE TESTE NO SIMULADOR:");
  Serial.println("Digite no Monitor Serial uma requisicao HTTP simulada, ex:");
  Serial.println("GET /?angulo=90 HTTP/1.1");
  Serial.println("------------------------------------------------------");
}

void loop() {
  // 1. AQUISIÇÃO DE DADOS AMBIENTAIS (COMPUTAÇÃO FÍSICA)
  leituraBrutaLDR = analogRead(PINO_LDR);
  // Mapeia a leitura do ADC (no módulo LDR do Wokwi, menor valor = mais luz)
  luminosidadePct = map(leituraBrutaLDR, 4063, 32, 0, 100);
  luminosidadePct = constrain(luminosidadePct, 0, 100);

  // 2. ESCUTA DO SERVIDOR DE BORDA (REDE TCP/IP - PORTA 80)
  WiFiClient cliente = servidor.available(); // Verifica se há cliente conectando

  if (cliente) {
    Serial.println("\n[REDE] Novo cliente conectado ao Servidor de Borda!");
    String requisicaoHTTP = "";

    // Laço contínuo enquanto o cliente estiver conectado
    while (cliente.connected()) {
      if (cliente.available()) {
        char c = cliente.read();
        requisicaoHTTP += c;

        // Ao final da linha da requisição HTTP, processa os parâmetros
        if (c == '\n') {
          Serial.print("[PACOTE RECEBIDO]: " + requisicaoHTTP);
          processarParametroRequisicao(requisicaoHTTP);

          // Envia resposta HTTP 200 OK com telemetria em formato JSON/HTML
          cliente.println("HTTP/1.1 200 OK");
          cliente.println("Content-Type: application/json");
          cliente.println("Connection: close");
          cliente.println();
          cliente.print("{\"ip\":\"");
          cliente.print(WiFi.localIP());
          cliente.print("\",\"luminosidade_pct\":");
          cliente.print(luminosidadePct);
          cliente.print(",\"servo_angulo\":");
          cliente.print(anguloAtual);
          cliente.println("}");
          break;
        }
      }
    }
    delay(10);
    cliente.stop(); // Encerra a conexão após responder
    Serial.println("[REDE] Conexao encerrada com o cliente.");
  }

  // 3. INJETOR DE PACOTES HTTP VIA MONITOR SERIAL (PARA VALIDAÇÃO NO WOKWI FREE)
  // Permite testar a mesma rotina de Request Parameters sem precisar de túnel pago
  if (Serial.available() > 0) {
    String pacoteSimulado = Serial.readStringUntil('\n');
    pacoteSimulado.trim();
    if (pacoteSimulado.length() > 0) {
      Serial.println("\n[SIMULADOR DE REDE] Pacote HTTP injetado: " + pacoteSimulado);
      processarParametroRequisicao(pacoteSimulado);
      Serial.print("[TELEMETRIA ATUAL] Luminosidade LDR: ");
      Serial.print(luminosidadePct);
      Serial.print("% (ADC: ");
      Serial.print(leituraBrutaLDR);
      Serial.print(") | Posicao do Servo: ");
      Serial.print(anguloAtual);
      Serial.println(" graus.");
    }
  }

  delay(50); // Intervalo de varredura do sistema embarcado
}

