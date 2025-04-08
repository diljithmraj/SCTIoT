# Scecure Communication Through IoT device (SCTIoT)

## Abstract
 This project is to create a secure, simple, and cost-effective communication system using ESP32 devices based on IoT. The system enables the transmission of data, such as Morse code or sensor data, over a network while maintaining the principles of CIA. It uses AES-128 encryption for secure data transmission and SSL/TLS for MQTT communication, ensuring data security and reliability. The project demonstrates the use of ESP32 devices for duplex communication, where one device acts as a Sender and the other as a Receiver, even when connected to different networks. This system is versatile and can be adapted for various applications, such as securely sending sensor data to a database or implementing custom IoT-based communication systems.
 
## implementation
 The project is implemented as a secure communication system using two ESP32 devices (Sender and Receiver) that exchange Morse code messages via an MQTT broker. The implementation involves hardware and software integration, encryption, and real-time communication. Below is a detailed description of the system implementation:
  ### WiFiCredential
   To connect to the network, Wi-Fi credentials need to be added during the ESP32 programming. In the case of a user-friendly interface or when changing locations, modifying Wi-Fi credentials is not easy, so we added an access point interface. If the ESP32 is unable to connect to the network within 15 seconds, it will automatically switch to access point mode.
 ### Data Reading
 In order to show a secure data transmission we choose to read the morse code using push buttons connected to the ESP32 micro-controller. Each button represents a specific Morse code input, such as a dot (.), dash (-), space (/), backspace, and send. The ESP32 processes these button inputs to construct a Morse code message, which can then be encrypted and transmitted securely.
### AESEncryption Standard
 AES is a symmetric encryption algorithm used in this project to secure Morse code messages before transmitting them over the MQTT protocol. The Sender device encrypts the Morse code message using AES-128 encryption, ensuring that the data remains confidential during transmission. The Receiver device decrypts the message to retrieve the original Morse code.
### EMQX(ErlangMQTTBroker)
 EMQX(Erlang MQTT Broker) is a high-performance, scalable, and open-source MQTTbroker designed to handle lightweight, real-time messaging for IoT applications. It implements the MQTT protocol, which is widely used for communication between IoT devices. EMQX acts as a central hub that facilitates the exchange of messages between publishers (e.g., sensors, devices) and subscribers(e.g., applications, other devices) in a secure and efficient manner.
### Post-Decryption Processing
 The decrypted Morse code message is processed to provide feedback using a buzzer.The buzzer will emit sound patterns corresponding to the Morse code symbols.
