#include <WiFi.h>
#include <PubSubClient.h>

// Wi-Fi sozlamalari
const char* ssid = "Mama"; // Wi-Fi nomini kiriting
const char* password = "19962003"; // Wi-Fi parolini kiriting

// MQTT sozlamalari
const char* mqtt_server = "broker.emqx.io";
const char* mqtt_topic = "esp32/buttons";
const char* client_id = "ESP32Client-UniqueID123";

// GPIO pinlari
#define BUZZER_PIN 25
#define LED1 26
#define LED2 27
#define LED3 14
#define LED4 12
#define LED5 13

WiFiClient espClient;
PubSubClient client(espClient);

// Musiqa ohanglari
int melody1[] = {262, 294, 330, 349, 392, 392, 349, 330, 294, 262, 330, 349, 392, 440, 440};
int melody2[] = {392, 440, 494, 523, 587, 587, 523, 494, 440, 392, 494, 523, 587, 659, 659};
int melody3[] = {523, 587, 659, 698, 784, 784, 698, 659, 587, 523, 659, 698, 784, 880, 880};
int noteDurations[] = {500, 500, 500, 500, 500, 250, 250, 500, 500, 500, 250, 250, 500, 500, 1000};

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);

  Serial.begin(115200);

  // Wi-Fi ga ulanish
  Serial.println("Wi-Fi ga ulanmoqda: " + String(ssid));
  WiFi.begin(ssid, password);
  int wifi_attempts = 0;
  while (WiFi.status() != WL_CONNECTED && wifi_attempts < 15) {
    delay(1000);
    Serial.print(".");
    wifi_attempts++;
    // Wi-Fi xato holatini tekshirish
    switch (WiFi.status()) {
      case WL_NO_SSID_AVAIL:
        Serial.println("\nXato: Wi-Fi tarmog‘i topilmadi!");
        break;
      case WL_CONNECT_FAILED:
        Serial.println("\nXato: Wi-Fi paroli noto‘g‘ri yoki ulanish muvaffaqiyatsiz!");
        break;
      case WL_DISCONNECTED:
        Serial.println("\nWi-Fi ulanishi uzildi, qayta urinish...");
        break;
    }
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWi-Fi ga ulandi! IP: " + WiFi.localIP().toString());
  } else {
    Serial.println("\nWi-Fi ga ulanib bo‘lmadi. Quyidagilarni tekshiring:");
    Serial.println("- SSID va parol to‘g‘riligini.");
    Serial.println("- Router 2.4 GHz tarmog‘ida ekanligini.");
    Serial.println("- Signal kuchi yetarli ekanligini.");
    while (true); // To‘xtab qolish
  }

  // MQTT serveriga ulanish
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void reconnect() {
  int maxAttempts = 5;
  int attempt = 0;
  while (!client.connected() && attempt < maxAttempts) {
    Serial.println("MQTT ga ulanmoqda...");
    if (client.connect(client_id)) {
      Serial.println("MQTT ga ulandi!");
      client.subscribe(mqtt_topic);
      Serial.println("Obuna bo‘lindi: " + String(mqtt_topic));
    } else {
      Serial.print("Xato, rc=");
      Serial.print(client.state());
      Serial.println(" 5 sekunddan keyin qayta urinish...");
      delay(5000);
      attempt++;
    }
  }
  if (!client.connected()) {
    Serial.println("MQTT ga ulanib bo‘lmadi. Broker, tarmoq yoki Client IDni tekshiring.");
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println("Xabar keldi: " + message);

  if (message == "button1") {
    playMelody(melody1, sizeof(melody1) / sizeof(melody1[0]), 1);
  } else if (message == "button2") {
    playMelody(melody2, sizeof(melody2) / sizeof(melody2[0]), 2);
  } else if (message == "button3") {
    playMelody(melody3, sizeof(melody3) / sizeof(melody3[0]), 3);
  }
}

void playMelody(int melody[], int size, int pattern) {
  for (int i = 0; i < size; i++) {
    tone(BUZZER_PIN, melody[i], noteDurations[i]);
    if (pattern == 1) {
      digitalWrite(LED1, i % 2 == 0 ? HIGH : LOW);
      digitalWrite(LED2, (i + 1) % 2 == 0 ? HIGH : LOW);
      digitalWrite(LED3, i % 3 == 0 ? HIGH : LOW);
      digitalWrite(LED4, (i + 1) % 3 == 0 ? HIGH : LOW);
      digitalWrite(LED5, i % 4 == 0 ? HIGH : LOW);
    } else if (pattern == 2) {
      digitalWrite(LED5, i % 2 == 0 ? HIGH : LOW);
      digitalWrite(LED4, (i + 1) % 2 == 0 ? HIGH : LOW);
      digitalWrite(LED3, i % 3 == 0 ? HIGH : LOW);
      digitalWrite(LED2, (i + 1) % 3 == 0 ? HIGH : LOW);
      digitalWrite(LED1, i % 4 == 0 ? HIGH : LOW);
    } else if (pattern == 3) {
      digitalWrite(LED1, i % 2 == 0 ? HIGH : LOW);
      digitalWrite(LED2, i % 2 == 0 ? HIGH : LOW);
      digitalWrite(LED3, i % 2 == 0 ? HIGH : LOW);
      digitalWrite(LED4, i % 2 == 0 ? HIGH : LOW);
      digitalWrite(LED5, i % 2 == 0 ? HIGH : LOW);
    }
    delay(noteDurations[i] + 50);
  }
  noTone(BUZZER_PIN);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(LED4, LOW);
  digitalWrite(LED5, LOW);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi ulanishi uzildi. Qayta ulanmoqda...");
    WiFi.reconnect();
    delay(5000);
    return;
  }
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}