#include <Arduino.h>
#include <DHT22_Universal.h>

#define DHT_PIN 14

DHT22_Universal dht(DHT_PIN);

void printHexByte(uint8_t b) {
    Serial.print("0x");
    if (b < 0x10) Serial.print("0");
    Serial.print(b, HEX);
}

void setup() {
    Serial.begin(115200);
    dht.begin();
    delay(1000);
    Serial.println("DHT22 Universal Diagnostics");
}

void loop() {
    DHT22_Status status = dht.read();
    const DHT22_DebugData& dbg = dht.getDebugData();

    Serial.println("----------------------------------------");
    Serial.print("Status   : ");
    Serial.println(DHT22_Universal::statusToString(status));

    Serial.print("Timings  : Wait=");
    Serial.print(dbg.waitLowUs);
    Serial.print("us | ACK_LOW=");
    Serial.print(dbg.ackLowUs);
    Serial.print("us | ACK_HIGH=");
    Serial.print(dbg.ackHighUs);
    Serial.println("us");

    Serial.print("RAW HEX  : ");
    for (uint8_t i = 0; i < 5; i++) {
        printHexByte(dbg.raw[i]);
        Serial.print(" ");
    }
    Serial.println();

    if (status == DHT22_OK) {
        Serial.print("CRC      : OK (");
        printHexByte(dbg.calcCRC);
        Serial.println(")");

        Serial.print("Result   : ");
        Serial.print(dht.getTemperature(), 1);
        Serial.print(" C | ");
        Serial.print(dht.getHumidity(), 1);
        Serial.println(" %");
    } else if (status == DHT22_ERR_CRC_MISMATCH) {
        Serial.print("CRC      : MISMATCH (Calc: ");
        printHexByte(dbg.calcCRC);
        Serial.print(", Recv: ");
        printHexByte(dbg.raw[4]);
        Serial.println(")");
    }

    delay(2500);
}
