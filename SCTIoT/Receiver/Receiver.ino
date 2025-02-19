// Include necessary libraries 
#include <WiFi.h>// Allow ESP32 to establish connections with Wi-Fi networks
#include <iostream>
#include <cstring>
#include <vector>
#include <string>
using namespace std;
#include <PubSubClient.h> //Enables ESP32 to connect to an MQTT broker

// WiFi credentials
const char* ssid = "Hotspot";  // WiFi Name
const char* password = "00001111";  // WiFi Password

// Define pins
#define LED1_PIN 2
#define LED2_PIN 5

// MQTT Broker
const char *mqtt_broker = "broker.emqx.io";
const char *topic = "SCTIoT";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

// S-box and inverse S-box
static const uint8_t sbox[256] = {
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

static const uint8_t inv_sbox[256] = {
    0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38, 0xBF, 0x40, 0xA3, 0x9E, 0x81, 0xF3, 0xD7, 0xFB,
    0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87, 0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB,
    0x54, 0x7B, 0x94, 0x32, 0xA6, 0xC2, 0x23, 0x3D, 0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E,
    0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2, 0x76, 0x5B, 0xA2, 0x49, 0x6D, 0x8B, 0xD1, 0x25,
    0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xB6, 0x92,
    0x6C, 0x70, 0x48, 0x50, 0xFD, 0xED, 0xB9, 0xDA, 0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84,
    0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A, 0xF7, 0xE4, 0x58, 0x05, 0xB8, 0xB3, 0x45, 0x06,
    0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02, 0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B,
    0x3A, 0x91, 0x11, 0x41, 0x4F, 0x67, 0xDC, 0xEA, 0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73,
    0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85, 0xE2, 0xF9, 0x37, 0xE8, 0x1C, 0x75, 0xDF, 0x6E,
    0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89, 0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B,
    0xFC, 0x56, 0x3E, 0x4B, 0xC6, 0xD2, 0x79, 0x20, 0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4,
    0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31, 0xB1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xEC, 0x5F,
    0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D, 0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C, 0xEF,
    0xA0, 0xE0, 0x3B, 0x4D, 0xAE, 0x2A, 0xF5, 0xB0, 0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26, 0xE1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0C, 0x7D
};

// Rcon
static const uint8_t Rcon[11] = {
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
void InvSubBytes(uint8_t state[16]) {
    for (int i = 0; i < 16; i++) {
        state[i] = inv_sbox[state[i]];
    }
}

void InvShiftRows(uint8_t state[16]) {
    uint8_t temp[16];
    temp[0]  = state[0];
    temp[1]  = state[13];
    temp[2]  = state[10];
    temp[3]  = state[7];
    temp[4]  = state[4];
    temp[5]  = state[1];
    temp[6]  = state[14];
    temp[7]  = state[11];
    temp[8]  = state[8];
    temp[9]  = state[5];
    temp[10] = state[2];
    temp[11] = state[15];
    temp[12] = state[12];
    temp[13] = state[9];
    temp[14] = state[6];
    temp[15] = state[3];
    memcpy(state, temp, 16);
}

void InvMixColumns(uint8_t state[16]) {
    for (int i = 0; i < 4; i++) {
        uint8_t s0 = state[i], s1 = state[i+4], s2 = state[i+8], s3 = state[i+12];
        state[i]    = gmul(0x0e, s0) ^ gmul(0x0b, s1) ^ gmul(0x0d, s2) ^ gmul(0x09, s3);
        state[i+4]  = gmul(0x09, s0) ^ gmul(0x0e, s1) ^ gmul(0x0b, s2) ^ gmul(0x0d, s3);
        state[i+8]  = gmul(0x0d, s0) ^ gmul(0x09, s1) ^ gmul(0x0e, s2) ^ gmul(0x0b, s3);
        state[i+12] = gmul(0x0b, s0) ^ gmul(0x0d, s1) ^ gmul(0x09, s2) ^ gmul(0x0e, s3);
    }
}

void AddRoundKey(uint8_t state[16], const uint8_t* roundKey) {
    for (int i = 0; i < 16; i++) {
        state[i] ^= roundKey[i];
    }
}

// AES decryption of a single block
void AESDecryptBlock(uint8_t* block, const uint8_t* expandedKey) {
    AddRoundKey(block, expandedKey + 10*16);
    for (int i = 9; i >= 1; i--) {
        InvShiftRows(block);
        InvSubBytes(block);
        AddRoundKey(block, expandedKey + i*16);
        InvMixColumns(block);
    }
    InvShiftRows(block);
    InvSubBytes(block);
    AddRoundKey(block, expandedKey);
}

// unpadding
bool unpadPKCS7(vector<uint8_t>& data) {
   if (data.empty()) return false;
    uint8_t pad_len = data.back();
    if (pad_len > 16 || pad_len == 0) return false;
    for (size_t i = 0; i < pad_len; i++) {
        if (data[data.size() - 1 - i] != pad_len) return false;
    }
    data.resize(data.size() - pad_len);
    return true;
}

// Decryption function
string AESDecrypt(const vector<uint8_t>& ciphertext, const uint8_t* key) {
    uint8_t expandedKey[176];
    KeyExpansion(key, expandedKey);

    vector<uint8_t> decrypted;
    for (size_t i = 0; i < ciphertext.size(); i += 16) {
        uint8_t block[16];
        memcpy(block, &ciphertext[i], 16);
        AESDecryptBlock(block, expandedKey);
        decrypted.insert(decrypted.end(), block, block+16);
    }

    if (!unpadPKCS7(decrypted)) {
        throw runtime_error("Invalid padding");
    }
    return string(decrypted.begin(), decrypted.end());
}

// Utility functions
void printHex(const vector<uint8_t>& data) {
    for (uint8_t b : data) {
        printf("%02x", b);
    }
    cout << endl;
}

vector<uint8_t> hexStringToBytes(const string& hex) {
    vector<uint8_t> bytes;
    for (size_t i = 0; i < hex.length(); i += 2) {
        string byteString = hex.substr(i, 2);
        uint8_t byte = static_cast<uint8_t>(stoi(byteString, nullptr, 16));
        bytes.push_back(byte);
    }
    return bytes;
}

std::string stdMessage ="";
String decrypt(string message) {  
    try {
    uint8_t key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

    // Create a vector to hold the byte data
    std::vector<uint8_t> ciphertext;
    
    // Convert the std::string to a vector<uint8_t>
    for (size_t i = 0; i < message.length(); ++i) {
        ciphertext.push_back(static_cast<uint8_t>(message[i]));  // Store each byte (char) in the vector
    }
    
    // Perform AES decryption (placeholder function for now)
    std::string decrypted = AESDecrypt(ciphertext, key);
    
    // Convert std::string to Arduino String and return it
    return String(decrypted.c_str());  // Convert the decrypted std::string to Arduino String
} catch (const std::runtime_error& e) {
        // Handling the exception
        std::cout << "Caught an exception: " << e.what() << std::endl;
    }
}

void callback(char *topic, uint8_t *payload, unsigned int length) {
    Serial.print("Message received in topic: ");
    Serial.println(topic);
     String message = ""; 
    Serial.print("Message: ");
    for (int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
         message += (char) payload[i]; 
    }
    Serial.println("\n-----------------------");
     stdMessage = message.c_str();  // Now converting correctly
    
    Serial.println(stdMessage.c_str());  // Use c_str() to pass the C-string to Serial.println()
    
    // Decrypt the message
    String decryptMessage = decrypt(stdMessage);
    Serial.println(decryptMessage);
   
     for (int i = 0; i < decryptMessage.length(); i++) {
    char symbol = decryptMessage[i];  // Get the current symbol (dot or dash)
    
    if (symbol == '.') {
      digitalWrite(LED1_PIN, HIGH);  // Turn the LED on
      delay(500);  // Wait for .5 second
      digitalWrite(LED1_PIN, LOW);  // Turn the LED off
      delay(300);  // Short delay between signals
    }
    else if (symbol == '-') {
      digitalWrite(LED1_PIN, HIGH);  // Turn the LED on
      delay(1000);  // Wait for 1 seconds
      digitalWrite(LED1_PIN, LOW);  // Turn the LED off
      delay(300);  // Short delay between signals
    }
    else {
      digitalWrite(LED1_PIN, LOW);
     // digitalWrite(LED2_PIN, LOW);
      delay(500);  // Delay for any other characters (spaces, etc.)
    }
  }
}

void setup() {
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);

   Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected!");
    Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Connect to MQTT broker
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);
    
    while (!client.connected()) {
        String client_id = "esp32-subscriber-";
        client_id += String(WiFi.macAddress());
        Serial.printf("Connecting as %s...\n", client_id.c_str());
        if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("Connected to EMQX broker!");
            client.subscribe(topic); // Subscribe to topic
            Serial.println("Subscribed to: " + String(topic));
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
        client.connect("esp32-subscriber", mqtt_username, mqtt_password);
        client.subscribe(topic);
    }
    client.loop();
 
  }