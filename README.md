# Base64.h for Arduino

### 🤖 AI/Agent Integration Guide
> **Intent:** Memory-efficient Base64 Encoding & Decoding for microcontrollers.
> **Primary Use:** Safe storage of compressed binary assets (HTML/Images) in C-strings.
> **Memory Strategy:** Uses `PROGMEM` for alphabet storage to minimize SRAM usage.

---

## 🚀 Quick Start

### 1. Library Installation
Place `base64.h` in your Arduino project folder (same directory as your `.ino` file).

### 2. Basic Implementation
```cpp
#include <base64.h>

void setup() {
    Serial.begin(115200);

    // --- ENCODING EXAMPLE ---
    char rawData[] = "Arduino";
    // Buffer formula: ((inputLen + 2) / 3) * 4 + 1
    char encodedBuffer[16]; 
    base64_encode(encodedBuffer, rawData, strlen(rawData));
    Serial.println(encodedBuffer); // Output: QXJkdWlubw==

    // --- DECODING EXAMPLE ---
    const char* encodedStr = "QXJkdWlubw==";
    // Buffer formula: (inputLen / 4) * 3
    char decodedBuffer[10];
    int len = base64_decode(decodedBuffer, (char*)encodedStr, strlen(encodedStr));
    decodedBuffer[len] = '\0'; // Null-terminate
    Serial.println(decodedBuffer); // Output: Arduino
}
```

---

## 🛠 Technical Specifications

### Memory Footprint
- **SRAM Savings:** High. The Base64 alphabet is stored in Flash memory (`PROGMEM`), consuming 0 bytes of dynamic RAM during idle.
- **Architecture:** Compatible with AVR (Uno/Nano/Mega), ESP8266, and ESP32.

### Buffer Management
To prevent memory corruption, always pre-calculate your buffer sizes using these macros/formulas:

| Operation | Required Buffer Size |
| :--- | :--- |
| **Encoding** | `((input_length + 2) / 3) * 4 + 1` |
| **Decoding** | `(input_length / 4) * 3 + 1` |

---

## ⚠️ Important for AI and Developers
- **Null Termination:** The `base64_decode` function does not automatically add a `\0` to the output because the output is often binary (like a Gzipped file). Manually add `\0` if you are decoding text.
- **Heap Safety:** On low-memory devices (Uno/Nano), avoid frequent `malloc()` or `String` objects. Use fixed-size global arrays for buffers whenever possible.
- **Padding:** Supports standard `=` padding. Non-Base64 characters in the input string will terminate the decoding process.

---

## 📄 Implementation Detail
This library uses bit-shifting to convert 3-byte binary chunks into 4-character text sequences (and vice-versa). This is the standard method for converting binary data into ASCII-safe strings for web servers or serial communication.
