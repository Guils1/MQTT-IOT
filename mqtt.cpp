#include <WiFi.h>
#include <SPI.h>
#include <MFRC522.h>
#include <PubSubClient.h>

const char* ssid = "BDAG";
const char* password = "bdag2018";
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;
const char* mqtt_topic = "toinfatec157"; // Adicionando o tópico "toinfatec157"

#define SS_PIN 21
#define RST_PIN 22
#define GREEN_LED_PIN 12
#define RED_LED_PIN 32

MFRC522 mfrc522(SS_PIN, RST_PIN);

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  delay(100);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(2500);
    Serial.println("Conectando ao WiFi...");
  }
  Serial.println("Conectado à rede WiFi");

  client.setServer(mqtt_server, mqtt_port);

  SPI.begin();
  mfrc522.PCD_Init();
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando se conectar ao MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("Conectado ao MQTT");
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    unsigned long intID = 0;
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      intID = (intID << 8) | mfrc522.uid.uidByte[i];
    }

    Serial.print("ID do Cartão como Inteiro: ");
    Serial.println(intID);

    if (intID == 2274889353) {
      digitalWrite(GREEN_LED_PIN, HIGH);
      delay(1000);
      digitalWrite(GREEN_LED_PIN, LOW);
      Serial.println("AAAA");
      client.publish(mqtt_topic, "Acesso Liberado"); // Publica no tópico "toinfatec157"
    } else {
      digitalWrite(RED_LED_PIN, HIGH);
      delay(1000);
      digitalWrite(RED_LED_PIN, LOW);
      client.publish(mqtt_topic, "Acesso Negado"); // Publica no tópico "toinfatec157"
    }
  }
}