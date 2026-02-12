#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

// ===== CONFIGURAÇÕES DE REDE =====
const char* ssid = "NOME DO WIFI";
const char* password = "SENHA DO WIFI";

// ===== CONFIGURAÇÕES DO THINGSBOARD (TLS) =====
const char *mqtt_broker = "mqtt.thingsboard.cloud";
const uint16_t mqtt_port = 8883;                      // TLS
const char *mqtt_topic = "v1/devices/me/telemetry";
const char *mqtt_username = "TOKEN DO DEVICE";   // token do device
const char *mqtt_password = "";                       // vazio
const char *client_id = "apolo-esp01-tls";

WiFiClientSecure secureClient;
PubSubClient mqtt_client(secureClient);

String serialBuffer = "";

String mqttStateToString(int state) {
  switch(state) {
    case -4: return "MQTT_CONNECTION_TIMEOUT";
    case -3: return "MQTT_CONNECTION_LOST";
    case -2: return "MQTT_CONNECT_FAILED";
    case -1: return "MQTT_DISCONNECTED";
    case 0:  return "MQTT_CONNECTED";
    case 1:  return "CONNECTION_REFUSED_TRANSIENT";
    case 2:  return "CONNECTION_REFUSED_BAD_PROTOCOL_VERSION";
    case 3:  return "CONNECTION_REFUSED_IDENTIFIER_REJECTED";
    case 4:  return "CONNECTION_REFUSED_SERVER_UNAVAILABLE";
    case 5:  return "CONNECTION_REFUSED_BAD_USERNAME_PASSWORD";
    case 6:  return "CONNECTION_REFUSED_NOT_AUTHORIZED";
    default: return "UNKNOWN";
  }
}

void connectToWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao WiFi");
  unsigned long t0 = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(400);
    Serial.print(".");
    if (millis() - t0 > 20000) { Serial.println("\nWiFi connect timeout"); break; }
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi OK. IP: " + WiFi.localIP().toString());
  } else {
    Serial.println("\nWiFi NÃO conectado. Confere SSID/senha/boot pins.");
  }
}

void connectToMQTTBroker() {
  // setInsecure para teste rápido (ignora validação do certificado).
  // Em produção substitua por verificação do CA/root fingerprint.
  secureClient.setInsecure();

  mqtt_client.setServer(mqtt_broker, mqtt_port);

  unsigned long start = millis();
  while (!mqtt_client.connected()) {
    Serial.printf("Conectando MQTT TLS como %s ...\n", client_id);
    if (mqtt_client.connect(client_id, mqtt_username, mqtt_password)) {
      Serial.println("Connected to MQTT broker (TLS).");
      break;
    } else {
      int st = mqtt_client.state();
      Serial.print("Failed to connect, rc=");
      Serial.print(st);
      Serial.print(" -> ");
      Serial.println(mqttStateToString(st));
      // Se for problema de credenciais (rc=5) não adianta re-tentar sem corrigir token.
      if (st == 5) {
        Serial.println("TOKEN inválido (rc=5). Verifica o token do device no ThingsBoard.");
        break;
      }
      Serial.println("Tentando de novo em 5s...");
      delay(5000);
      // opcional: timeout global ou fallback para plain/8883 já resolvido com teste anterior
      if (millis() - start > 60000) { Serial.println("Timeout geral MQTT connect."); break; }
    }
  }
}

void mqttCallback(char *topic, byte *payload, unsigned int length) {
  // caso queira processar comandos do ThingsBoard
  Serial.print("MQTT msg on ");
  Serial.println(topic);
  Serial.print("Payload: ");
  for (unsigned int i=0;i<length;i++) Serial.print((char)payload[i]);
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  delay(150);
  Serial.println("\n=== ESP01 TLS MQTT START ===");
  connectToWiFi();
  mqtt_client.setCallback(mqttCallback);
  connectToMQTTBroker();
}

void loop() {
  if (!mqtt_client.connected()) {
    Serial.println("MQTT desconectado, tentando reconectar...");
    connectToMQTTBroker();
  }
  mqtt_client.loop();

  // leitura UART (dados vindos da STM)
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\r') continue;
    if (c == '\n') {
      serialBuffer.trim();
      if (serialBuffer.length() > 0) {
        bool published = false;
        if (serialBuffer.startsWith("{") && serialBuffer.endsWith("}")) {
          published = mqtt_client.publish(mqtt_topic, serialBuffer.c_str());
          Serial.print("Publish JSON -> ");
        } else {
          String msg = "{\"message\":\"" + serialBuffer + "\"}";
          published = mqtt_client.publish(mqtt_topic, msg.c_str());
          Serial.print("Publish texto -> ");
        }
        Serial.println(published ? "OK" : "FALHOU");
        if (!published) {
          int st = mqtt_client.state();
          Serial.print("mqtt_client.state() = ");
          Serial.print(st);
          Serial.print(" -> ");
          Serial.println(mqttStateToString(st));
        } else {
          Serial.print("Publicado: ");
          Serial.println(serialBuffer);
        }
      }
      serialBuffer = "";
    } else {
      serialBuffer += c;
    }
  }
}