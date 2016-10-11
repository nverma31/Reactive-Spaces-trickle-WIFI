// Arduino Code:
// Below, 'character' types are defined:  They hold 1 byte of data, 256 values.
// A char can be interpreted as a small number (0-255) or as a member of the
// ASCII set (which is what we deal with below).  Characters expressed as
// ASCII are surrounded in single-quotes, like '5'.
// Thus each char has a corresponding numeric value can thus be tested against.

int ledPin = 13;   // select the pin for the LED
int val = 0;       // variable to store the data from the serial port

void setup() {
 pinMode(ledPin,OUTPUT);    // declare the LED's pin as output
 Serial.begin(9600);        // connect to the serial port
}

void loop () {
  if (Serial.available()) {
    // For the below examples, let's pretend that the passed-in serial 
    // value is character '5'.
    // Since the declared variable val is an int, it converts the char 
    // value passed in into an int.
    // If char val = '5', the numeric representation is 53.
    val = Serial.read();      // read the serial port

    // If the stored value is a single-digit number, blink the LED 
    // that number of times.
    // Here we compare the int value of val against the int values 
    // of the string.
    // Characters '0' and '9' are equivalent to integer 48 and 57.
    
      Serial.println(val);
      // Convert from char to int:
      // From above, int conversion of val, which is char '5', is 53.
      // int conversion of char '0' is 48.
      // 53-48 = 5  : blink that # of times
      val = val - '0';  
      for(int i=0; i<10; i++) {
        Serial.println("blink!");
        digitalWrite(ledPin,HIGH);
        delay(150);
        digitalWrite(ledPin, LOW);
        delay(150);
      }
    
  }
}
