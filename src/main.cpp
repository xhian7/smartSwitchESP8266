#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h>

const char* ssid = "broker";
const char* psk = "MQTT-mosquitto";
const char* mqttBroker = "zero.local";

WiFiClient espClient;
PubSubClient mqttClient(espClient);

bool isLightOn = false;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String message = "";
  for (int i = 0; i < length; i++) {
    message += String((char)payload[i]);
  }
  Serial.println(message);

  // Encender o apagar led
  if (isLightOn && message.equals("turnOff"))
  {
    digitalWrite(2, LOW);
    isLightOn = false;
  }

  if (!isLightOn && message == "turnOn")
  {
    digitalWrite(2, HIGH);
    isLightOn = true;
  }
  
}

void reconnect() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client";
    //clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("connected");
      // ... and resubscribe
      mqttClient.subscribe("test");
      // Once connected, publish an announcement...
      //mqttClient.publish("test", "on");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {

  // Iniciar el monitor
  Serial.begin(115200);
  Serial.println("\nBienvenido");

  // Declarar pines de entrada y salida
  pinMode(0, INPUT);
  pinMode(2, OUTPUT);

  // Inicar conexión WiFi
  WiFi.begin(ssid, psk);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Conexion exitosa");
  Serial.println("Direccion IP: ");
  Serial.println(WiFi.localIP());

  // Iniciar mDNS a direccion esp8266.local
   if (!MDNS.begin("esp8266")) 
   {             
     Serial.println("Error iniciando mDNS");
   }
   Serial.println("mDNS iniciado");

   // Inicar conexión con el broker
   mqttClient.setServer(mqttBroker, 1883);
   mqttClient.setCallback(callback);
}

void loop() {
  if (!mqttClient.connected()) {
    reconnect();
  }

  // Código para opto-triac
  // if (digitalRead(0))
  // {
  //   isLightOn = true;
  // }
  // else
  // {
  //   isLightOn = false;
  // }
  
  // Código de prueba
  if (!digitalRead(0))
  {
    if (isLightOn)
    {
      mqttClient.publish("test", "turnOff");
    }
    else
    {
      mqttClient.publish("test", "turnOn");
    }
    delay(200);
  }

  mqttClient.loop();
}