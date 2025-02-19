// Include necessary libraries             
#include <WiFi.h>  // Allow ESP32 to establish connections with Wi-Fi networks
#include <iostream>
#include <vector>
#include <string>
using namespace std;
#include <PubSubClient.h> //Enables ESP32 to connect to an MQTT broker

// Define pins
#define LED_PIN 2 // LED
#define DOT_B 4 // DOT
#define DASH_B 5// DASH
#define SPACE_B 18  // BACKSPACE
#define BSPACE_B 19 // SPACE
#define ENTER_B 21 // SEND

// WiFi credentials
const char* ssid PROGMEM = "FreeWiFi"; // WiFi Name
const char* password PROGMEM = "00000000"; // WiFi Password

// MQTT Broker
const char *mqtt_broker PROGMEM = "broker.emqx.io";
const char *topic PROGMEM = "SCTIoT";
const char *mqtt_username PROGMEM = "emqx";
const char *mqtt_password PROGMEM = "public";
const int mqtt_port PROGMEM = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

String M_code = ""; //To store the message
int co = 0;

// S-box 
static const uint8_t sbox[256] PROGMEM = {
    0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
    0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
    0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
    0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
    0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
    0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
    0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
    0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
    0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
    0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
    0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
    0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
    0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
    0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
    0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
    0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
};

// Rcon
static const uint8_t Rcon[11] PROGMEM = {
    0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
};

// Helper functions
uint8_t gmul(uint8_t a, uint8_t b) {
    uint8_t p = 0;
    for (int i = 0; i < 8; i++) {
        if (b & 1) p ^= a;
        bool carry = a & 0x80;
        a <<= 1;
        if (carry) a ^= 0x1b; // XOR with irreducible polynomial x^8 + x^4 + x^3 + x + 1
        b >>= 1;
    }
    return p;
}

// Key expansion
void KeyExpansion(const uint8_t* key, uint8_t* expandedKey) {
    memcpy(expandedKey, key, 16);

    for (int i = 4; i < 44; i++) {
        uint8_t temp[4];
        memcpy(temp, expandedKey + (i-1)*4, 4);

        if (i % 4 == 0) {
            // RotWord
            uint8_t t = temp[0];
            temp[0] = temp[1];
            temp[1] = temp[2];
            temp[2] = temp[3];
            temp[3] = t;

            // SubWord
            for (int j = 0; j < 4; j++) {
                temp[j] = sbox[temp[j]];
            }

            // XOR with Rcon
            temp[0] ^= Rcon[i/4];
        }

        // XOR with the word 4 positions back
        for (int j = 0; j < 4; j++) {
            expandedKey[i*4 + j] = expandedKey[(i-4)*4 + j] ^ temp[j];
        }
    }
}

// AES transformations
void SubBytes(uint8_t state[16]) {
    for (int i = 0; i < 16; i++) {
        state[i] = sbox[state[i]];
    }
}

void ShiftRows(uint8_t state[16]) {
    uint8_t temp[16];
    temp[0]  = state[0];
    temp[1]  = state[5];
    temp[2]  = state[10];
    temp[3]  = state[15];
    temp[4]  = state[4];
    temp[5]  = state[9];
    temp[6]  = state[14];
    temp[7]  = state[3];
    temp[8]  = state[8];
    temp[9]  = state[13];
    temp[10] = state[2];
    temp[11] = state[7];
    temp[12] = state[12];
    temp[13] = state[1];
    temp[14] = state[6];
    temp[15] = state[11];
    memcpy(state, temp, 16);
}

void MixColumns(uint8_t state[16]) {
    for (int i = 0; i < 4; i++) {
        uint8_t s0 = state[i], s1 = state[i+4], s2 = state[i+8], s3 = state[i+12];
        state[i]    = gmul(0x02, s0) ^ gmul(0x03, s1) ^ s2 ^ s3;
        state[i+4]  = s0 ^ gmul(0x02, s1) ^ gmul(0x03, s2) ^ s3;
        state[i+8]  = s0 ^ s1 ^ gmul(0x02, s2) ^ gmul(0x03, s3);
        state[i+12] = gmul(0x03, s0) ^ s1 ^ s2 ^ gmul(0x02, s3);
    }
}

void AddRoundKey(uint8_t state[16], const uint8_t* roundKey) {
    for (int i = 0; i < 16; i++) {
        state[i] ^= roundKey[i];
    }
}
// AES encryption of a single block
void AESEncryptBlock(uint8_t* block, const uint8_t* expandedKey) {
    AddRoundKey(block, expandedKey);
    for (int i = 1; i <= 10; i++) {
        SubBytes(block);
        ShiftRows(block);
        if (i != 10) MixColumns(block);
        AddRoundKey(block, expandedKey + i*16);
    }
}

// Padding
void padPKCS7(vector<uint8_t>& data) {
    size_t pad_len = 16 - (data.size() % 16);
    if (pad_len == 0) pad_len = 16;
    for (size_t i = 0; i < pad_len; i++) {
        data.push_back(static_cast<uint8_t>(pad_len));
    }
}

// Encryption function
vector<uint8_t> AESEncrypt(const string& plaintext, const uint8_t* key) {
    vector<uint8_t> data(plaintext.begin(), plaintext.end());
    padPKCS7(data);

    uint8_t expandedKey[176];
    KeyExpansion(key, expandedKey);

    vector<uint8_t> ciphertext;
    for (size_t i = 0; i < data.size(); i += 16) {
        uint8_t block[16];
        memcpy(block, &data[i], 16);
        AESEncryptBlock(block, expandedKey);
        ciphertext.insert(ciphertext.end(), block, block+16);
    }
    return ciphertext;
}
// Utility functions
void printHex(const vector<uint8_t>& data) {
    for (uint8_t b : data) {
        printf("%02x", b);
    }
    cout << endl;
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(DOT_B, INPUT_PULLUP); // Enable internal pull-up resistor
  pinMode(DASH_B, INPUT_PULLUP); 
  pinMode(BSPACE_B, INPUT_PULLUP); 
  pinMode(SPACE_B, INPUT_PULLUP);
  pinMode(ENTER_B, INPUT_PULLUP); 
  
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected!");


   // Connect to MQTT broker
    client.setServer(mqtt_broker, mqtt_port);
    while (!client.connected()) {
        String client_id = "esp32-publisher-";
        client_id += String(WiFi.macAddress());
        Serial.printf("Connecting as %s...\n", client_id.c_str());
        if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("Connected to EMQX broker!");
        } else {
            Serial.print("Failed, retrying in 2s. State: ");
            Serial.println(client.state());
            delay(2000);
        }
    }
}

void loop() {
  
  if (!client.connected()) {
        Serial.println("MQTT disconnected! Reconnecting...");
        client.connect("esp32-publisher", mqtt_username, mqtt_password);
    }
// Check if button is pressed
  if (digitalRead(DOT_B) == LOW) { // Dot Button pressed
    digitalWrite(LED_PIN, HIGH);  // Turn on LED
    delay(200);
    M_code += ".";   // "." stored to string
    Serial.print(".");
  }
  else if (digitalRead(DASH_B) == LOW) { // Dash Button pressed 
    digitalWrite(LED_PIN, HIGH);  // Turn on LED
    delay(200);
    M_code += "-";  // "-" stored to string
    Serial.print("-");
  }
  else if (digitalRead(SPACE_B) == LOW) {  // Space Button pressed
    digitalWrite(LED_PIN, HIGH);  // Turn on LED
    delay(200);
    M_code += "/"; // "/" to represent space btw alphabet "//" to represent space btw words
    Serial.print("/");
  }
  else if (digitalRead(BSPACE_B) == LOW) {  // Backspace Button pressed
    delay(200);
    co = M_code.length();
    co = co - 1;
    M_code.remove(co);  //To remove last letter (basically works as backspace)
    Serial.println("");
    Serial.println(M_code);
  }
  else if (digitalRead(ENTER_B) == LOW) {  // Send Button pressed 
    digitalWrite(LED_PIN, HIGH);  // Turn on LED
    delay(200);
    Serial.println("");
    Serial.println(M_code);
  std::string plaintext = M_code.c_str();

  // AES key (should be kept secret and secure)
 uint8_t key[16] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F};

    // Encryption call
    vector<uint8_t> ciphertext = AESEncrypt(plaintext, key);
    cout << "Ciphertext (hex): ";
    printHex(ciphertext);
     String message = "";
    for (size_t i = 0; i < ciphertext.size(); ++i) {
        message += (char)ciphertext[i];  // Append each byte as a character
    }
    
   // Publish a message
    client.publish(topic, message.c_str());
    Serial.println("Message Sent: " + message);

    delay(300);
    M_code = "";

  }
  else {
    digitalWrite(LED_PIN, LOW);   // Turn off LED
  }
}