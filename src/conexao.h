#ifndef CONEXAO_H
#define CONEXAO_H

#include <WiFi.h>
#include <PubSubClient.h>

// Wi-Fi
const char* ssid = "WIFI-IOT";
const char* senha = "ac5ce1ss0@IOT";

// MQTT
const char* mqttServidor = "broker.hivemq.com";
const int mqttPorta = 1883;

// Tópicos do micro-ondas
const char* topicComando = "senai510/microondas/comando";
const char* topicTempo = "senai510/microondas/tempo";
const char* topicStatus = "senai510/microondas/status";

// Cliente Wi-Fi e MQTT
WiFiClient espClient;
PubSubClient client(espClient);

#endif
