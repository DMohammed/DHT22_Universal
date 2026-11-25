#include <Arduino.h>
#include <DHT22_Universal.h>

// Use pin 14 on ESP32, or pin 2 on Arduino Uno/Nano
#define DHT_PIN 14

DHT22_Universal dht(DHT_PIN);

void setup() {
    Serial.begin(115200);
    dht.begin();
}

void loop() {
    DHT22_Status status = dht.read();

    if (status == DHT22_OK) {
        Serial.print("Temp: ");
        Serial.print(dht.getTemperature(), 1);
        Serial.print(" C | Humidity: ");
        Serial.print(dht.getHumidity(), 1);
        Serial.println(" %");
    } else {
        Serial.print("Read failed: ");
        Serial.println(DHT22_Universal::statusToString(status));
    }

    delay(2500);
}
