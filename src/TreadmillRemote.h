#ifndef TREADMILLREMOTE_H
#define TREADMILLREMOTE_H

#include <Arduino.h>
#include <cstdint>

// --- Protocol description (scoped inside the class) ---
class TreadmillRemote {
public:
    explicit constexpr TreadmillRemote(uint8_t pin) : txPin(pin) {}

    void begin() const {
        pinMode(txPin, OUTPUT);
        digitalWrite(txPin, LOW);
    }

    // Arduino‑style methods
    void start() const { send(Command::Start); }
    void stop()  const { send(Command::Stop);  }
    void plus()  const { send(Command::Plus);  }
    void minus() const { send(Command::Minus); }
    void mode()  const { send(Command::Mode);  }

private:
    const uint8_t txPin;
    mutable bool lineStateHigh = false;

    // --- Protocol constants ---
    static constexpr uint16_t PulseSync   = 10'000;
    static constexpr uint16_t PulseShort  = 300;
    static constexpr uint16_t PulseLong   = 800;
    static constexpr uint32_t RepeatGap   = 50'000; // 50 ms
    static constexpr uint8_t  RepeatCount = 2;

    static constexpr uint64_t HeaderBits =
        0b101010101010101010101010101010101010101010101010010101ULL;
    static constexpr uint8_t HeaderLen = 54;

    enum class Command : uint32_t {
        Plus  = 0b010110100110101010100101100,
        Minus = 0b011010011010101010010110010,
        Stop  = 0b100110101010101001100101010,
        Start = 0b011010100110101010010101100,
        Mode  = 0b100101010110101001101010100
    };
    static constexpr uint8_t CmdLen = 27;

    // --- Internal helpers ---
    void send(Command cmd) const {
        for (uint8_t r = 0; r < RepeatCount; ++r) {
            sendHeader();
            sendCommand(static_cast<uint32_t>(cmd));
            delayMicroseconds(RepeatGap);
        }
    }

    void sendBit(bool bitVal) const {
        digitalWrite(txPin, lineStateHigh ? HIGH : LOW);
        delayMicroseconds(bitVal ? PulseLong : PulseShort);
        lineStateHigh = !lineStateHigh;
    }

    void sendHeader() const {
        digitalWrite(txPin, LOW);
        delayMicroseconds(PulseSync);
        lineStateHigh = true;
        for (int i = HeaderLen; i-- > 0;) {
            sendBit((HeaderBits >> i) & 1ULL);
        }
    }

    void sendCommand(uint32_t cmd) const {
        for (int i = CmdLen; i-- > 0;) {
            sendBit((cmd >> i) & 1U);
        }
    }
};

#endif // TREADMILLREMOTE_H
