# DHT22_Universal

A lightweight, cross-platform Arduino & PlatformIO library for **DHT22 (AM2302)** temperature and humidity sensors, specifically engineered to support both **original** sensors and **fast-responding clones** that fail on strict RMT or standard timing libraries.

## Features
- **Cross-Platform:** Works seamlessly on **ESP32**, **ESP8266**, and **AVR Arduinos** (Uno, Nano, Mega) with zero platform-specific dependencies (`Serial.printf` is not used).
- **Clone-Friendly ACK Timing:** Releases the data bus immediately to `INPUT_PULLUP` and accepts fast-responding clones (`ACK LOW` down to 15 µs) without throwing false `Invalid ACK` errors.
- **Dual Negative Temperature Decoding:** Automatically handles both original **sign-bit** negative temperatures and clone **two's complement** negative temperatures.
- **Built-in Diagnostics:** Exposes raw 5-byte HEX frames, exact `Wait`, `ACK LOW`, and `ACK HIGH` microsecond timings, and calculated vs. received CRC values.

---

## Hardware Wiring
Connect the DHT22 to your microcontroller:
- **VCC** → `3.3V` (ESP32/ESP8266) or `5V` (Arduino Uno/Nano/Mega)
- **GND** → `GND`
- **DATA** → Any digital GPIO pin (e.g., `GPIO 14` on ESP32, `Pin 2` on Arduino)
- **Pull-up Resistor:** Place an external **4.7kΩ – 10kΩ** pull-up resistor between `DATA` and `VCC` for clean rising edges.

---

## Installation

### PlatformIO
Add the repository to your `platformio.ini`:
```ini
lib_deps =
    https://github.com/YOUR_USERNAME/DHT22_Universal.git
```

### Arduino IDE
1. Download this repository as a `.zip` file.
2. Open Arduino IDE and go to **Sketch → Include Library → Add .ZIP Library...**
3. Select `DHT22_Universal.zip`.

---

## Quick Start

```cpp
#include <Arduino.h>
#include <DHT22_Universal.h>

#define DHT_PIN 14  // Use Pin 2 on Arduino Uno/Nano

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

    delay(2500); // DHT22 requires at least 2 seconds between reads
}
```

---

## Diagnostics & Troubleshooting
Use `examples/Diagnostics/Diagnostics.ino` to inspect raw sensor behavior:
- **`NO_RESPONSE`**: Sensor did not pull the line LOW. Check wiring, power, and pull-up resistor.
- **`INVALID_ACK`**: ACK pulse fell outside `15–120 µs` (LOW) or `50–120 µs` (HIGH).
- **`CRC_MISMATCH`**: Checksum failed. Usually caused by noise, long cables, or a missing 4.7kΩ pull-up resistor.

## License
MIT License
