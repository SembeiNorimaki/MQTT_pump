#include "secrets.h"
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// feeds
Adafruit_MQTT_Subscribe pump_subscriber = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME FEED_NAME, MQTT_QOS_1);
Adafruit_MQTT_Publish pump_publisher = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME FEED_NAME);

//void pumpcallback(char *data, uint16_t len) {
void pumpcallback(double seconds) {
  if (seconds > 0) {
    Serial.print("Pump ON for ");
    Serial.print(seconds);
    Serial.println(" seconds");
    digitalWrite(PUMP_PIN, HIGH);
    delay(1000 * seconds);
    pump_publisher.publish(0);
    digitalWrite(PUMP_PIN, LOW);
    Serial.println("Pump OFF");
  }
}

void setup() {
  pinMode(PUMP_PIN, OUTPUT);
  Serial.begin(115200);
  delay(10);
  WIFI_connect();
  pump_subscriber.setCallback(pumpcallback);
  mqtt.subscribe(&pump_subscriber);
}

void loop() {
  MQTT_connect();
  mqtt.processPackets(10000);
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
}

void WIFI_connect() {
  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());
}

void MQTT_connect() {
  int8_t ret;
  if (mqtt.connected())
    return;

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 10 seconds...");
       mqtt.disconnect();
       delay(10000);  // wait 10 seconds
       retries--;
       if (retries == 0)
         while (1);
  }
  Serial.println("MQTT Connected!");
}
