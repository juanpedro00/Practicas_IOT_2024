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

#define MQTT_CLIENT_NAME "testClient" // ID de cliente único para la conexión de prueba
#define DEVICE_LABEL "test-device" // Dispositivo para prueba
#define VARIABLE_LABEL_TEMP "test-temp" // Variable de prueba para la temperatura

const char* mqttBroker = "io.adafruit.com";
const int mqttPort = 1883;
char payload[100]; // Espacio para el payload
char topic[100]; // Espacio para el topic

// Crear objeto para el sensor GY-906
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

/**************************************** 
 * WiFi y MQTT 
 ****************************************/ 
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  
  // Conectar a WiFi
  Serial.print("Conectando a WiFi...");
  WiFi.begin(WIFISSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println(" Conectado!");
  
  // Configuración de servidor MQTT
  client.setServer(mqttBroker, mqttPort);

  // Inicializar el sensor GY-906
  if (!mlx.begin()) {
    Serial.println("Error al inicializar el sensor GY-906.");
    while (1); // Detener el código si el sensor falla
  }
  Serial.println("Sensor GY-906 inicializado correctamente.");

  // Intentar conexión a Adafruit IO
  Serial.print("Conectando a Adafruit IO...");
  if (client.connect(MQTT_CLIENT_NAME, ADAFRUIT_USERNAME, ADAFRUIT_KEY)) {
    Serial.println(" Conexión exitosa a Adafruit IO!");

    // Leer temperatura y enviar
    float objectTemp = mlx.readObjectTempC();
    Serial.print("Temperatura leída: ");
    Serial.println(objectTemp);

    dtostrf(objectTemp, 4, 2, payload);
    sprintf(topic, "%s/feeds/%s.%s", ADAFRUIT_USERNAME, DEVICE_LABEL, VARIABLE_LABEL_TEMP);
    sprintf(payload, "{\"value\": %s}", payload);

    // Enviar dato a Adafruit IO
    if (client.publish(topic, payload)) {
      Serial.println("Temperatura enviada exitosamente a Adafruit IO!");
    } else {
      Serial.println("Error al enviar la temperatura a Adafruit IO.");
    }
  } else {
    Serial.println("Error al conectar a Adafruit IO.");
  }
}

void loop() {
  // Este código de prueba no necesita un loop continuo
}
