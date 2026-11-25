#ifndef DHT22_UNIVERSAL_H
#define DHT22_UNIVERSAL_H

#include <Arduino.h>

enum DHT22_Status : uint8_t {
    DHT22_OK = 0,
    DHT22_ERR_NO_RESPONSE,
    DHT22_ERR_ACK_TIMEOUT,
    DHT22_ERR_INVALID_ACK,
    DHT22_ERR_BIT_TIMEOUT,
    DHT22_ERR_CRC_MISMATCH
};

struct DHT22_DebugData {
    int32_t waitLowUs;
    int32_t ackLowUs;
    int32_t ackHighUs;
    uint8_t raw[5];
    uint8_t calcCRC;
    bool isCloneEncoding;
};

class DHT22_Universal {
public:
    explicit DHT22_Universal(uint8_t pin);

    void begin();
    DHT22_Status read();

    float getTemperature() const { return _temperature; }
    float getHumidity() const { return _humidity; }
    const DHT22_DebugData& getDebugData() const { return _debug; }

    static const char* statusToString(DHT22_Status status);

private:
    uint8_t _pin;
    float _temperature;
    float _humidity;
    DHT22_DebugData _debug;

    int32_t expectPulse(uint8_t state, uint32_t timeoutUs);
};

#endif
