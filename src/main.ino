
#include "conexao.h"

// Display das unidades
const int pinoA = 22;
const int pinoB = 23;
const int pinoC = 14;
const int pinoD = 2;
const int pinoE = 18;
const int pinoF = 21;
const int pinoG = 19;

// Display das dezenas
const int pinoA1 = 13;
const int pinoB1 = 4;
const int pinoC1 = 25;
const int pinoD1 = 26;
const int pinoE1 = 12;
const int pinoF1 = 32;
const int pinoG1 = 33;

// Buzzer
const int buzzer = 27;

// Pinos agrupados
const int pinosUnidade[7] = {
  pinoA, pinoB, pinoC, pinoD, pinoE, pinoF, pinoG
};

const int pinosDezena[7] = {
  pinoA1, pinoB1, pinoC1, pinoD1, pinoE1, pinoF1, pinoG1
};

// Tabela de segmentos: A, B, C, D, E, F, G
// Mantém a lógica do código enviado.
const byte digitos[10][7] = {
  {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, LOW},  // 0
  {LOW, HIGH, HIGH, LOW, LOW, LOW, LOW},      // 1
  {HIGH, HIGH, LOW, HIGH, HIGH, LOW, HIGH},   // 2
  {HIGH, HIGH, HIGH, HIGH, LOW, LOW, HIGH},   // 3
  {LOW, HIGH, HIGH, LOW, LOW, HIGH, HIGH},    // 4
  {HIGH, LOW, HIGH, HIGH, LOW, HIGH, HIGH},   // 5
  {HIGH, LOW, HIGH, HIGH, HIGH, HIGH, HIGH},  // 6
  {HIGH, HIGH, HIGH, LOW, LOW, LOW, LOW},     // 7
  {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH}, // 8
  {HIGH, HIGH, HIGH, HIGH, LOW, HIGH, HIGH}   // 9
};

int tempoRestante = 0;
bool contando = false;
bool finalizado = false;
unsigned long ultimoSegundo = 0;

void mostrarUnidade(int numero) {
  for (int i = 0; i < 7; i++) {
    digitalWrite(pinosUnidade[i], digitos[numero][i]);
  }
}

void mostrarDezena(int numero) {
  for (int i = 0; i < 7; i++) {
    digitalWrite(pinosDezena[i], digitos[numero][i]);
  }
}

void mostrarTempo(int tempo) {
  mostrarDezena(tempo / 10);
  mostrarUnidade(tempo % 10);
}

// Recebe comandos enviados pelo Flask
void receberMensagem(char* topic, byte* payload,
                     unsigned int tamanho) {
  if (String(topic) != topicComando) {
    return;
  }

  String mensagem = "";

  for (unsigned int i = 0; i < tamanho; i++) {
    mensagem += (char)payload[i];
  }

  int novoTempo = mensagem.toInt();

  // Aceita tempos de 1 a 99 segundos
  if (novoTempo >= 1 && novoTempo <= 99 && !contando) {
    tempoRestante = novoTempo;
    contando = true;
    finalizado = false;
    ultimoSegundo = millis();

    mostrarTempo(tempoRestante);

    client.publish(topicStatus, "CONTANDO");

    Serial.print("Tempo recebido do site: ");
    Serial.println(tempoRestante);
  } else {
    Serial.println("Comando ignorado. Verifique o tempo ou o estado.");
  }
}

void conectarWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, senha);

  Serial.print("Conectando ao Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void conectarMQTT() {
  while (!client.connected()) {
    Serial.println("Tentando conectar ao MQTT...");

    String id = "MicroondasESP32-";
    id += String((uint32_t)ESP.getEfuseMac(), HEX);
    id += String(random(0xffff), HEX);

    if (client.connect(id.c_str())) {
      Serial.println("MQTT conectado!");

      client.subscribe(topicComando);
      client.publish(topicStatus, "PRONTO");
    } else {
      Serial.print("Falha MQTT. Código: ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  randomSeed(micros());

  for (int i = 0; i < 7; i++) {
    pinMode(pinosUnidade[i], OUTPUT);
    pinMode(pinosDezena[i], OUTPUT);
  }

  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);

  mostrarTempo(0);

  conectarWiFi();

  client.setServer(mqttServidor, mqttPorta);
  client.setCallback(receberMensagem);
  client.setBufferSize(256);

  conectarMQTT();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }

  if (!client.connected()) {
    conectarMQTT();
    client.subscribe(topicComando);
  }

  client.loop();

  if (contando && millis() - ultimoSegundo >= 1000) {
    ultimoSegundo += 1000;

    if (tempoRestante > 0) {
      tempoRestante--;
      mostrarTempo(tempoRestante);

      String tempo = String(tempoRestante);
      client.publish(topicTempo, tempo.c_str());

      Serial.print("Tempo restante: ");
      Serial.println(tempoRestante);
    }

    if (tempoRestante == 0) {
      contando = false;
      finalizado = true;

      client.publish(topicStatus, "FINALIZADO");

      tone(buzzer, 4000);
      delay(3000);
      noTone(buzzer);

      Serial.println("Tempo finalizado!");
    }
  }
}
