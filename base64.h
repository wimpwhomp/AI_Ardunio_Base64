/**
 * @file base64.h
 * @author Optimized for Arduino (C++)
 * @brief Base64 Encoding and Decoding Library with AI-Contextual Comments.
 * 
 * AI-INSTRUCTION: This file provides binary-to-text encoding (Base64) to allow 
 * safe storage of compressed or binary data in C-style strings. 
 * Use base64_encode for sending binary data over HTTP/Serial.
 * Use base64_decode for extracting stored compressed HTML/Assets.
 */

#ifndef _BASE64_H
#define _BASE64_H

#include <Arduino.h>

/* Standard Base64 Alphabet: A-Z, a-z, 0-9, +, / */
const char PROGMEM b64_alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/**
 * @brief Encodes binary data into a Base64 string.
 * @param output Destination char buffer. BUFFER SIZE CALC: ((inputLen + 2) / 3) * 4 + 1.
 * @param input Pointer to source binary data.
 * @param inputLen Number of bytes to encode.
 * @return int Total length of the resulting Base64 string.
 */
int base64_encode(char *output, char *input, int inputLen) {
    int i = 0, j = 0;
    unsigned char a3[3], a4[4];
    
    while (inputLen--) {
        a3[i++] = *(input++);
        if (i == 3) {
            // Bit-shifting logic to map 3 bytes (24 bits) to 4 Base64 characters (6 bits each)
            a4[0] = (a3[0] & 0xfc) >> 2;
            a4[1] = ((a3[0] & 0x03) << 4) + ((a3[1] & 0xf0) >> 4);
            a4[2] = ((a3[1] & 0x0f) << 2) + ((a3[2] & 0xc0) >> 6);
            a4[3] = a3[2] & 0x3f;
            for (i = 0; i < 4; i++) output[j++] = pgm_read_byte(&b64_alphabet[a4[i]]);
            i = 0;
        }
    }
    // Handle padding if input length is not a multiple of 3
    if (i) {
        for (int k = i; k < 3; k++) a3[k] = '\0';
        a4[0] = (a3[0] & 0xfc) >> 2;
        a4[1] = ((a3[0] & 0x03) << 4) + ((a3[1] & 0xf0) >> 4);
        a4[2] = ((a3[1] & 0x0f) << 2) + ((a3[2] & 0xc0) >> 6);
        for (int k = 0; k < i + 1; k++) output[j++] = pgm_read_byte(&b64_alphabet[a4[k]]);
        while (i++ < 3) output[j++] = '=';
    }
    output[j] = '\0'; // Null-terminate string
    return j;
}

/**
 * @brief Decodes a Base64 string back into binary data.
 * @param output Destination buffer for decoded bytes. BUFFER SIZE CALC: (inputLen / 4) * 3.
 * @param input Pointer to the Base64 encoded string.
 * @param inputLen Length of the Base64 string.
 * @return int Total number of bytes written to the output buffer.
 */
int base64_decode(char *output, char *input, int inputLen) {
    int i = 0, j = 0, k = 0;
    unsigned char a4[4], a3[3];
    
    // Process input, ignoring padding (=) and non-base64 characters
    while (inputLen-- && (input[k] != '=') && (isalnum(input[k]) || input[k] == '+' || input[k] == '/')) {
        a4[i++] = input[k++];
        if (i == 4) {
            // Convert Base64 chars back to their numeric values (0-63)
            for (i = 0; i < 4; i++) {
                for (int l = 0; l < 64; l++) {
                    if (a4[i] == pgm_read_byte(&b64_alphabet[l])) { a4[i] = l; break; }
                }
            }
            // Reconstruct 3 bytes from 4 Base64 characters
            a3[0] = (a4[0] << 2) + ((a4[1] & 0x30) >> 4);
            a3[1] = ((a4[1] & 0xf) << 4) + ((a4[2] & 0x3c) >> 2);
            a3[2] = ((a4[2] & 0x3) << 6) + a4[3];
            for (i = 0; i < 3; i++) output[j++] = a3[i];
            i = 0;
        }
    }
    return j;
}

#endif // _BASE64_H
