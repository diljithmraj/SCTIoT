#define LED_PIN 2 // LED
#define DOT_B 4  // DOT
#define DASH_B 5 // DASH
#define BSPACE_B 19  // BACKSPACE
#define SPACE_B 18 // SPACE
#define ENTER_B 21 // SEND

String M_code = ""; 
int count = 0;  

void setup() {
  pinMode(LED_PIN, OUTPUT);  
  pinMode(DOT_B, INPUT_PULLUP); // Enable internal pull-up resistor
  pinMode(DASH_B, INPUT_PULLUP); 
  pinMode(BSPACE_B, INPUT_PULLUP); 
  pinMode(SPACE_B, INPUT_PULLUP); 
  pinMode(ENTER_B, INPUT_PULLUP); 
  
  Serial.begin(115200);
}

void loop() {
  // Check if button is pressed
  if (digitalRead(DOT_B) == LOW) {  // Dot Button pressed
    digitalWrite(LED_PIN, HIGH);  // Turn on LED
    delay(200);
    M_code += ".";    // "." stored to string
    Serial.print(".");
  }
  else if (digitalRead(DASH_B) == LOW) {  // Dash Button pressed 
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
    count = M_code.length();
    count = count - 1;
    M_code.remove(count);  //To remove last letter (basically works as backspace)
     Serial.println("");
    Serial.println(M_code);
  }
  else if (digitalRead(ENTER_B) == LOW) {  // Send Button pressed 
    digitalWrite(LED_PIN, HIGH);  // Turn on LED
    delay(200);
    Serial.println(M_code); //print the morse code
  }
  else {
    digitalWrite(LED_PIN, LOW);   // Turn off LED
  }
}