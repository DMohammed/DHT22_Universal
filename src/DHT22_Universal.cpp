#include "DHT22_Universal.h"

DHT22_Universal::DHT22_Universal(uint8_t pin)
    : _pin(pin), _temperature(NAN), _humidity(NAN) {
    memset(&_debug, 0, sizeof(_debug));
}

void DHT22_Universal::begin() {
    pinMode(_pin, INPUT_PULLUP);
}

int32_t DHT22_Universal::expectPulse(uint8_t state, uint32_t timeoutUs) {
    uint32_t start = micros();
    while (digitalRead(_pin) == state) {
        if ((micros() - start) > timeoutUs) {
            return -1;
        }
    }
    return (int32_t)(micros() - start);
}

DHT22_Status DHT22_Universal::read() {
    uint8_t data[5] = {0, 0, 0, 0, 0};
    memset(&_debug, 0, sizeof(_debug));

    // 1. Pull LOW for 2ms (safe for both original and clone sensors)
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
    delayMicroseconds(2000);

    // 2. Release bus immediately to INPUT_PULLUP so fast clones aren't clipped
    pinMode(_pin, INPUT_PULLUP);

    // Critical timing window (~4ms)
    noInterrupts();

    // 3. Wait for sensor to pull LOW
    _debug.waitLowUs = expectPulse(HIGH, 200);
    if (_debug.waitLowUs < 0) {
        interrupts();
        return DHT22_ERR_NO_RESPONSE;
    }

    // 4. Measure ACK LOW and ACK HIGH
    _debug.ackLowUs  = expectPulse(LOW, 200);
    _debug.ackHighUs = expectPulse(HIGH, 200);

    if (_debug.ackLowUs < 0 || _debug.ackHighUs < 0) {
        interrupts();
        return DHT22_ERR_ACK_TIMEOUT;
    }

    // Relaxed ACK bounds for fast-responding clones (ACK LOW: 15-120us, ACK HIGH: 50-120us)
    if (_debug.ackLowUs < 15 || _debug.ackLowUs > 120 ||
        _debug.ackHighUs < 50 || _debug.ackHighUs > 120) {
        interrupts();
        return DHT22_ERR_INVALID_ACK;
    }

    // 5. Read 40 data bits
    for (uint8_t i = 0; i < 40; i++) {
        int32_t lowTime  = expectPulse(LOW, 150);
        int32_t highTime = expectPulse(HIGH, 150);

        if (lowTime < 0 || highTime < 0) {
            interrupts();
            return DHT22_ERR_BIT_TIMEOUT;
        }

        data[i / 8] <<= 1;
        if (highTime > 50) {
            data[i / 8] |= 1;
        }
    }

    interrupts();

    memcpy(_debug.raw, data, 5);
    _debug.calcCRC = (uint8_t)((data[0] + data[1] + data[2] + data[3]) & 0xFF);

    if (_debug.calcCRC != data[4]) {
        return DHT22_ERR_CRC_MISMATCH;
    }

    // 6. Decode Humidity
    _humidity = ((uint16_t)(data[0] << 8) | data[1]) * 0.1f;

    // 7. Decode Temperature (Original Sign-Bit & Clone Two's Complement)
    uint16_t rawTemp = ((uint16_t)data[2] << 8) | data[3];
    if (data[2] == 0xFF) {
        _temperature = (int16_t)rawTemp * 0.1f;
        _debug.isCloneEncoding = true;
    } else if (rawTemp & 0x8000) {
        _temperature = -((rawTemp & 0x7FFF) * 0.1f);
        _debug.isCloneEncoding = false;
    } else {
        _temperature = rawTemp * 0.1f;
        _debug.isCloneEncoding = false;
    }

    return DHT22_OK;
}

const char* DHT22_Universal::statusToString(DHT22_Status status) {
    switch (status) {
        case DHT22_OK:               return "OK";
        case DHT22_ERR_NO_RESPONSE:  return "NO_RESPONSE";
        case DHT22_ERR_ACK_TIMEOUT:  return "ACK_TIMEOUT";
        case DHT22_ERR_INVALID_ACK:  return "INVALID_ACK";
        case DHT22_ERR_BIT_TIMEOUT:  return "BIT_TIMEOUT";
        case DHT22_ERR_CRC_MISMATCH: return "CRC_MISMATCH";
        default:                     return "UNKNOWN";
    }
}
