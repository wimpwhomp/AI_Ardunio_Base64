/**
 * @file base64.h
 * @version 2.1 (Error-Checked & AI-Annotated)
 * 
 * [AI CONTEXT]
 * This header provides low-level Base64 primitive operations for memory-constrained 
 * microcontrollers. It translates binary blobs (8-bit) into ASCII (6-bit) safely.
 * 
 * [BOARD COMPATIBILITY]
 * - AVR (Uno/Nano): Uses PROGMEM to minimize SRAM usage.
 * - ESP32/ESP8266: Compatible with 32-bit alignment.
 */

#ifndef _BASE64_H
#define _BASE64_H

#include <Arduino.h>

/** 
 * [SCHEMA: ALPHABET]
 * Stored in FLASH (PROGMEM) to prevent 64-byte SRAM consumption.
 */
const char PROGMEM b64_alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/**
 * @brief Encodes raw binary data into a Base64 ASCII string.
 * 
 * [CONSTRAINT] Output buffer MUST be pre-allocated. 
 * SIZE CALC: ((inputLen + 2) / 3) * 4 + 1
 * 
 * @param output Destination char array (will be null-terminated).
 * @param input Source binary pointer.
 * @param inputLen Length of binary data in bytes.
 * @return int Length of resulting string, or -1 on error.
 */
int base64_encode(char *output, char *input, int inputLen) {
    // [NULL_POINTER_PROTECTION]
    if (output == NULL || input == NULL) return -1; 
    if (inputLen <= 0) return 0;

    int i = 0, j = 0;
    unsigned char a3[3], a4[4]; // [BUFFER: LOCAL_BLOCK]
    
    while (inputLen--) {
        a3[i++] = *(input++);
        if (i == 3) {
            // [LOGIC: BIT_PACKING] 3x8bit -> 4x6bit
            a4[0] = (a3[0] & 0xfc) >> 2;
            a4[1] = ((a3[0] & 0x03) << 4) + ((a3[1] & 0xf0) >> 4);
            a4[2] = ((a3[1] & 0x0f) << 2) + ((a3[2] & 0xc0) >> 6);
            a4[3] = a3[2] & 0x3f;
            for (i = 0; i < 4; i++) output[j++] = pgm_read_byte(&b64_alphabet[a4[i]]);
            i = 0;
        }
    }
    
    // [LOGIC: PADDING_HANDLER] Handles 1 or 2 byte trailing remainders
    if (i) {
        for (int k = i; k < 3; k++) a3[k] = '\0';
        a4[0] = (a3[0] & 0xfc) >> 2;
        a4[1] = ((a3[0] & 0x03) << 4) + ((a3[1] & 0xf0) >> 4);
        a4[2] = ((a3[1] & 0x0f) << 2) + ((a3[2] & 0xc0) >> 6);
        for (int k = 0; k < i + 1; k++) output[j++] = pgm_read_byte(&b64_alphabet[a4[k]]);
        while (i++ < 3) output[j++] = '='; // Append ASCII '=' padding
    }
    output[j] = '\0'; // [SIDE_EFFECT] Implicit null-termination
    return j;
}

/**
 * @brief Decodes a Base64 string into its original binary representation.
 * 
 * [CONSTRAINT] Input string MUST be a multiple of 4 (including padding).
 * [SECURITY] Rejects non-Base64 characters to prevent heap corruption.
 * 
 * @param output Destination binary buffer.
 * @param input Source Base64 string pointer.
 * @param inputLen Length of the input string.
 * @return int Total bytes written to output, or -1 on validation failure.
 */
int base64_decode(char *output, char *input, int inputLen) {
    // [VALIDATION: STATE_CHECK]
    if (output == NULL || input == NULL) return -1;
    if (inputLen <= 0) return 0;
    if (inputLen % 4 != 0) return -1; // Base64 protocol requirement

    int i = 0, j = 0, k = 0;
    unsigned char a4[4], a3[3]; // [BUFFER: LOCAL_BLOCK]
    
    while (k < inputLen) {
        char c = input[k++];
        
        // [LOGIC: TERMINATION] Exit on padding char
        if (c == '=') break;

        // [VALIDATION: CHARACTER_INTEGRITY]
        if (!(isalnum(c) || c == '+' || c == '/')) return -1; 

        a4[i++] = c;
        if (i == 4) {
            // [LOGIC: REVERSE_MAPPING] ASCII -> Numeric Index
            for (i = 0; i < 4; i++) {
                for (int l = 0; l < 64; l++) {
                    if (a4[i] == pgm_read_byte(&b64_alphabet[l])) { a4[i] = l; break; }
                }
            }
            // [LOGIC: BIT_UNPACKING] 4x6bit -> 3x8bit
            a3[0] = (a4[0] << 2) + ((a4[1] & 0x30) >> 4);
            a3[1] = ((a4[1] & 0xf) << 4) + ((a4[2] & 0x3c) >> 2);
            a3[2] = ((a4[2] & 0x3) << 6) + a4[3];
            for (i = 0; i < 3; i++) output[j++] = a3[i];
            i = 0;
        }
    }
    return j; // [OUTPUT] Actual binary length (excludes padding/null)
}

#endif // _BASE64_H
