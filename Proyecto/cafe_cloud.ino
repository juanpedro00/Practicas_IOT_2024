/**************************************** 
 * Include Libraries 
 ****************************************/ 
#include <WiFi.h> 
#include <PubSubClient.h> 
#include <Wire.h>
#include <Adafruit_MLX90614.h> 

/**************************************** 
 * Define Constants 
 ****************************************/ 
#define WIFISSID "IoT" // Tu WiFi SSID
#define PASSWORD "1t3s0IoT23" // Tu WiFi Password
#define ADAFRUIT_USERNAME "tu_usuario_adafruit" // Tu nombre de usuario en Adafruit IO
#define ADAFRUIT_KEY "tu_llave_aio" // Tu llave AIO de Adafruit

#define MQTT_CLIENT_NAME "1429183MbRle" // ID del cliente, único y diferente para cada dispositivo
#define DEVICE_LABEL "mqtt-publish-esp32-mar" // Nombre del dispositivo
#define VARIABLE_LABEL_TEMP "temperatura" // Variable para Temperatura

const char* mqttBroker = "io.adafruit.com";
const int mqttPort = 1883;
char payload[200]; // Espacio para el payload
char topic[150]; // Espacio para el nombre del topic

// Espacio para almacenar valores de los sensores
char str_temp[10];

// Crear objeto para el sensor GY-906
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

/**************************************** 
 * Funciones auxiliares 
 ****************************************/ 
WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;
  String message(p);
  Serial.write(payload, length);
  Serial.println(topic);
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (client.connect(MQTT_CLIENT_NAME, ADAFRUIT_USERNAME, ADAFRUIT_KEY)) {
      Serial.println("Connected to Adafruit IO");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      delay(2000);
    }
  }
}

/**************************************** 
 * Main Functions 
 ****************************************/ 
void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFISSID, PASSWORD);

  Serial.println();
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.println("WiFi Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqttBroker, mqttPort);
  client.setCallback(callback);
  
  // Inicializar el sensor GY-906
  if (!mlx.begin()) {
    Serial.println("Error al inicializar el sensor GY-906.");
    while (1);
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  // Leer temperatura del objeto
  float objectTemp = mlx.readObjectTempC();
  dtostrf(objectTemp, 4, 2, str_temp);
  sprintf(topic, "%s/feeds/%s.%s", ADAFRUIT_USERNAME, DEVICE_LABEL, VARIABLE_LABEL_TEMP);
  sprintf(payload, "{\"value\": %s}", str_temp);
  Serial.print("Enviando Temperatura a Adafruit IO: ");
  Serial.println(objectTemp);
  client.publish(topic, payload);

  client.loop();

  delay(12000); // Esperar 12 segundos entre cada envío
}
