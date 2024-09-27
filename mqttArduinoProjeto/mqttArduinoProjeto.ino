#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Adafruit_AHT10.h>

const char* ssid = "SENAC";
const char* password = "x1y2z3@snc";

const char* mqtt_server = "zfeemqtt.eastus.cloudapp.azure.com";
const int mqtt_port = 41883;
const char* mqtt_user = "Senac";
const char* mqtt_password  = "Senac";
const char* Client = "Senac10";
const char* topic = "Senac/M2G2E/Saida";
const char* topicSaida = "Senac/M2G2E/Entrada";

WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_AHT10 aht;

bool sendingData = false;  // Variável para controlar o envio de dados
unsigned long lastMsgTime = 0;  // Tempo da última mensagem enviada
const long interval = 5000;  // Intervalo de 5 segundos

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando ao WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Conectado!");
  Serial.println("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida no tópico: ");
  Serial.print(topic);
  Serial.print(". Mensagem: ");
  String message;

  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // Analisa o JSON recebido
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, message);

  if (!error) {
    const char* status = doc["status"];
    if (strcmp(status, "on") == 0) {
      sendingData = true;  // Ativa o envio de dados
      Serial.println("Envio de dados ativado");
    } else if (strcmp(status, "off") == 0) {
      sendingData = false;  // Desativa o envio de dados
      Serial.println("Envio de dados desativado");
    }
  } else {
    Serial.print("Falha ao analisar JSON: ");
    Serial.println(error.c_str());
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    if (client.connect(Client, mqtt_user, mqtt_password)) {
      Serial.println("Conectado!");
      client.subscribe(topicSaida);  // Assina o tópico de entrada
    } else {
      Serial.print("Falha na conexão, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5 segundos...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  if (!aht.begin()) {
    Serial.println("Could not find AHT10? Check wiring");
    while (1) delay(10);
  }
  Serial.println("AHT10 found");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();  // Mantenha esta chamada para processar mensagens recebidas

  // Apenas envia os dados se sendingData for true e se o intervalo de 5 segundos tiver passado
  if (sendingData && (millis() - lastMsgTime > interval)) {
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);

    Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees C");
    Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.println("% rH");

    StaticJsonDocument<200> doc;
    doc["Temperature"] = temp.temperature;
    doc["Humidity"] = humidity.relative_humidity;

    char jsonBuffer[200];
    serializeJson(doc, jsonBuffer);

    // Publica o JSON no tópico MQTT
    client.publish(topic, jsonBuffer);

    // Exibe os dados no Serial Monitor
    Serial.println(jsonBuffer);

    // Atualiza o tempo da última mensagem enviada
    lastMsgTime = millis();
  }
}
