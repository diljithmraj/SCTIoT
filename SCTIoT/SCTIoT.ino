#define LED_PIN 2
#define DOT_B 4
#define DASH_B 5
#define BSPACE_B 19
#define SPACE_B 18
#define ENTER_B 21

String M_code = ""; 
int count = 0;  

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(DOT_B, INPUT_PULLUP); // Enable internal pull-up resistor
  pinMode(DASH_B, INPUT_PULLUP); // Enable internal pull-up resistor
  pinMode(BSPACE_B, INPUT_PULLUP); // Enable internal pull-up resistor
  pinMode(SPACE_B, INPUT_PULLUP); // Enable internal pull-up resistor
  pinMode(ENTER_B, INPUT_PULLUP); // Enable internal pull-up resistor
  
  Serial.begin(115200);
}

void loop() {
  // Check if button is pressed
  if (digitalRead(DOT_B) == LOW) {  // Button pressed (LOW when pressed due to pull-up)
    digitalWrite(LED_PIN, HIGH);  // Turn on LED
    delay(200);
    M_code += ".";
    Serial.print(".");
  }
  else if (digitalRead(DASH_B) == LOW) {  // Button pressed (LOW when pressed due to pull-up)
    digitalWrite(LED_PIN, HIGH);  // Turn on LED
    delay(200);
    M_code += "-";
    Serial.print("-");
  }
  else if (digitalRead(SPACE_B) == LOW) {  // Button pressed (LOW when pressed due to pull-up)
    digitalWrite(LED_PIN, HIGH);  // Turn on LED
    delay(200);
    M_code += "/";
    Serial.print("/");
  }
  else if (digitalRead(BSPACE_B) == LOW) {  // Button pressed (LOW when pressed due to pull-up)
    delay(200);
    count = M_code.length();
    count = count - 1;
    M_code.remove(count);  //To remove last letter (basically works as backspace)
     Serial.println("");
    Serial.println(M_code);
  }
  else if (digitalRead(ENTER_B) == LOW) {  // Button pressed (LOW when pressed due to pull-up)
    digitalWrite(LED_PIN, HIGH);  // Turn on LED
    delay(200);
    Serial.println(M_code);
  }
  else {
    digitalWrite(LED_PIN, LOW);   // Turn off LED
  }
}