#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x20, 16, 2); // run ic2_scanner sketch and get the IC2 address, which is 0x3f in my case, it could be 0x3f in many cases

const int redLedPin = 13;
const int greenLedPin = 12;
const int dcMotorPin1 = 7;
const int dcMotorPin2 = 6;
const int terminalPin = 1;
#define VALID_RFID_TIME 30000
enum AccessStatus {
  IDEAL,
  GRANT,
  DENY
};

AccessStatus accessStatus = IDEAL;

const int MAX_RFIDS = 3;
String rfidList[MAX_RFIDS] = {
  "1234567890",
  "1111111111",
  "0000000000"
};

void setup() {
  lcd.init();
  // put your setup code here, to run once:
  // Initialize pins
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(dcMotorPin1, OUTPUT);
  pinMode(dcMotorPin2, OUTPUT);
  pinMode(terminalPin, INPUT);

  // Initially turn off LEDs and stop the motor
  digitalWrite(redLedPin, LOW);
  digitalWrite(greenLedPin, LOW);
  stopMotor();

  Serial.begin(9600);

  // setup LCD
  lcd.backlight();
  lcd.print("  Scan");
  lcd.setCursor(0, 1);
  lcd.print(" Your ID");
  // delay(10000);
}

int accessTime = 0;
String userInfo;

void loop() {
  // put your main code here, to run repeatedly:
  String inputRfid = readInput();
  if (inputRfid != "false") // if input is entered on terminal
    checkRFID(inputRfid); // set access status to grant or deny

  if (accessStatus == GRANT){ 
    if (accessTime - millis() >= VALID_RFID_TIME) {
      closeDoor();
    }
    else if (accessTime == 0){
      grantAccess();
    }
  }
  else if (accessStatus == DENY){
    denyAccess();
  }




  // Read input from terminal
  // int input = digitalRead(terminalPin);

  // // if (input == HIGH) { // If input is 1
  //   digitalWrite(greenLedPin, HIGH); // Turn on green LED
  //   digitalWrite(redLedPin, LOW); // Turn off red LED
  //   shiftRight(); // Shift motor right 90 degrees
  // // } else { // If input is 0
  //   delay(5000);
  //   digitalWrite(redLedPin, HIGH); // Turn on red LED
  //   digitalWrite(greenLedPin, LOW); // Turn off green LED
  //   shiftLeft(); // Shift motor left 90 degrees
  // // }

  delay(1000); // Delay to avoid rapid toggling
}

void checkRFID(String inputRfid){
  for (int i = 0; i < MAX_RFIDS; i++) {
    if (inputRfid == rfidList[i]){
      accessStatus = GRANT;
      userInfo = "ID: " + rfidList[i] + "Name:";
    }
  }
}

void closeDoor(){
  accessStatus = IDEAL;
  accessTime = 0;

  // TODO: shift dc motor

}

void grantAccess(){
  accessTime = millis();
  // turn on green LED

  // open door (turn right the DC motor 90 degrees)

  // display the information of the person on the LCD

}

void denyAccess(){
  // turn on red LED

  // close door (turn left DC motor 90 degrees)

  // display "ACCESS DENIED" on the LCD


  delay(4000)
  accessStatus = IDEAL;
}

String readInput() {
  String inputString = "";
  boolean stringComplete = false;

  while (Serial.available()) {
    // Read the incoming byte
    char inChar = (char)Serial.read();

    // If the incoming character is a newline, set stringComplete flag to true
    if (inChar == '\n') {
      stringComplete = true;
    } else {
      // Add the incoming character to the inputString
      inputString += inChar;
    }
  }

  // If a complete string has been received
  if (stringComplete) {
    // Print the received string
    // Serial.println("Received: " + inputString);

    // Clear the inputString and reset stringComplete flag
    inputString = "";
    stringComplete = false;

    return inputString;
  }
  return "false";
}

void shiftRight() {
  digitalWrite(dcMotorPin1, HIGH);
  digitalWrite(dcMotorPin2, LOW);
  delay(1000); // Adjust this delay to control motor speed and angle
  stopMotor();
}

void shiftLeft() {
  digitalWrite(dcMotorPin1, LOW);
  digitalWrite(dcMotorPin2, HIGH);
  delay(1000); // Adjust this delay to control motor speed and angle
  stopMotor();
}

void stopMotor() {
  digitalWrite(dcMotorPin1, LOW);
  digitalWrite(dcMotorPin2, LOW);
}
