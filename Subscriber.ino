#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Galaxy A126BE3";
const char* password = "jgqq7023";
const char* mqtt_server = "192.168.77.237";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

int analogPinH = A0;   // pour le capteur d'humidité de sol
int digitalPinR = 4;   // pour le relais

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    msg[i] = (char)payload[i];
  }
  msg[length] = '\0';  // Null-terminate the string
  Serial.println(msg);

  // Convert the received value to an integer
  int receivedValue = atoi(msg);

  // Check if the received value is greater than 800
  if (receivedValue < 950) {
    // Activate the relay and LED
    digitalWrite(digitalPinR, HIGH);
    digitalWrite(BUILTIN_LED, HIGH);
  } else {
    // Deactivate the relay and LED
    digitalWrite(digitalPinR, LOW);
    digitalWrite(BUILTIN_LED, LOW);
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.publish("outTopic", "hello world");
      client.subscribe("humidity2");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(digitalPinR, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 192);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    client.subscribe("humidity2");
    client.publish("outTopic", "hello world");
  }
}
