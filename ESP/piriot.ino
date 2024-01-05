#include <WiFi.h>
#include <PubSubClient.h>

// WiFi Credentials
char ssid[] = "your_wifi_ssid";        // Enter your WiFi name
char password[] = "your_wifi_password"; // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "Broker.hivemq.com";
const char *topic = "motion";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.println("Received message:");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup()
{
  Serial.begin(19200);
  Serial.println("Starting...");

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // Connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("Connecting to WiFi...");
    delay(500);
  }
  Serial.println("Connected to WiFi");
  digitalWrite(LED_BUILTIN, LOW);

  // Connecting to an MQTT broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  while (!client.connected())
  {
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("Connecting to MQTT...");
    String client_id = "esp32-client-";
    uint8_t mac[6];
    WiFi.macAddress(mac);
    client_id += String(mac[5], HEX);
    client_id += String(mac[4], HEX);
    client_id += String(mac[3], HEX);
    client_id += String(mac[2], HEX);
    client_id += String(mac[1], HEX);
    client_id += String(mac[0], HEX);

    if (client.connect(client_id.c_str()))
    {
      Serial.println("MQTT Connected");
      digitalWrite(LED_BUILTIN, LOW);
    }
    else
    {
      Serial.print("MQTT connection failed with state ");
      Serial.println(client.state());
      delay(2000);
    }
  }

  // Subscribe to a MQTT topic
  client.subscribe(topic);
  Serial.println("Subscribed to MQTT topic");
}

void loop()
{
  client.loop();
  delay(2000);
}
