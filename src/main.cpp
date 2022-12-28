#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <config.h>

#define RED_LED 32
#define GREEN_LED 33

WiFiClient espClient;
PubSubClient client(espClient);

void setup_leds()
{
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
}

void display_led()
{
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, HIGH);
}

void setup_wifi()
{
  Serial.println("\n");
  WiFi.begin(SSID, PASSWORD);
  Serial.println("Attente de connexion...");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(100);
  }

  Serial.println("\n");
  Serial.println("Connexion établie!");
  Serial.print("Adresse IP:");
  Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *message, unsigned int length)
{
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  if (String(topic) == "lock1")
  {
    Serial.print('"');
    Serial.print(messageTemp);
    Serial.println('"');
    if (messageTemp == "true")
    {
      Serial.print("ici");
      digitalWrite(RED_LED, LOW);
      digitalWrite(GREEN_LED, HIGH);
    }
    else
    {
      digitalWrite(RED_LED, HIGH);
      digitalWrite(GREEN_LED, LOW);
    }
  }
}

void setup_mqtt()
{
  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(callback);
}

void setup()
{
  setup_leds();
  display_led();

  Serial.begin(9600);
  delay(1000);

  setup_wifi();
  setup_mqtt();
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Tentative de connexion au serveur MQTT...");
    if (client.connect("ESP32Lock1"))
    {
      Serial.println("connecté");
      client.subscribe("lock1");
    }
    else
    {
      Serial.print("échec, rc=");
      Serial.print(client.state());
      Serial.println(" nouvel essai dans 5 secondes");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop()
{

  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
}
