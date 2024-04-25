#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x20, 16, 2);  // run ic2_scanner sketch and get the IC2 address, which is 0x3f in my case, it could be 0x3f in many cases

const int redLedPin = 13;
const int greenLedPin = 12;
const int dcMotorPin1 = 7;
const int dcMotorPin2 = 6;
const int terminalPin = 1;

#define VALID_RFID_TIME 10000
#define RFID_LEN 10

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

unsigned long int accessTime = 0;


struct UserInfo {
  String name;
  String id;
  bool access = false;
};

UserInfo userInfo;

bool isDoorOpen = false;

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

String inputRfid = "";

void loop() {
  // put your main code here, to run repeatedly:
  readInput();
  if (inputRfid.length() > RFID_LEN || accessStatus == DENY){
    inputRfid = "";
    Serial.flush();
  }
  if (inputRfid.length() == RFID_LEN) {  // if input is entered on terminal
    checkRFID(inputRfid);                // set access status to grant or deny
    Serial.println(inputRfid);
    inputRfid = "";
  }
  if (accessStatus == GRANT) {
    if (accessTime == 0) {
      grantAccess();
    } else if (millis() - accessTime >= VALID_RFID_TIME) {
      closeDoor();
      idealMode();
    }
  } else if (accessStatus == DENY) {
    denyAccess();
    delay(3000);
    idealMode();
  }

  //delay(100);  // Delay to avoid rapid toggling
}

void idealMode() {
  accessStatus = IDEAL;
  Serial.flush();
  inputRfid = "";
  lcd.clear();
  lcd.print(" Scan");
  lcd.setCursor(0, 1);
  lcd.print(" Your ID");
  digitalWrite(redLedPin, LOW);    // Turn off red LED
  digitalWrite(greenLedPin, LOW);  // Turn off green LED
}

void checkRFID(String inputRfid) {
  userInfo.access = false;
  accessStatus = DENY;
  for (int i = 0; i < MAX_RFIDS; i++) {
    if (inputRfid == rfidList[i]) {
      accessStatus = GRANT;
      // accessTime = millis();
      userInfo.access = true;
      userInfo.id = rfidList[i];
      userInfo.name = "John";
      break;
    }
  }
}

void closeDoor() {
  accessTime = 0;
  isDoorOpen = false;

  // TODO: shift dc motor
  shiftMotorLeft();
}

void grantAccess() {
  accessTime = millis();
  // turn on green LED
  digitalWrite(greenLedPin, HIGH);  // Turn on green LED
  digitalWrite(redLedPin, LOW);     // Turn off red LED

  // display the information of the person on the LCD
  lcdShowMessage(userInfo);

  // open door (turn right the DC motor 90 degrees)
  if (!isDoorOpen)
    shiftMotorRight();
  isDoorOpen = true;
}

void lcdShowMessage(UserInfo user) {
  lcd.clear();
  if (user.access == false) {
    lcd.print(" ACCESS DENIED!");
    lcd.setCursor(0, 1);
    lcd.print("Wait 3 seconds!");
  } else {
    lcd.print(" ACCESS GRANTED!");
    lcd.setCursor(0, 1);
    lcd.print(" " + user.name + " " + user.id);
  }
}

void denyAccess() {
  // turn on red LED
  digitalWrite(redLedPin, HIGH);   // Turn on red LED
  digitalWrite(greenLedPin, LOW);  // Turn off green LED

  // display "ACCESS DENIED" on the LCD
  lcdShowMessage(userInfo);

  // close door (turn left DC motor 90 degrees)
  if (isDoorOpen)
    closeDoor();
}

void readInput() {
  while (Serial.available() > 0) {
    char c = Serial.read();
    inputRfid += c;
  }
}

void shiftMotorRight() {
  digitalWrite(dcMotorPin1, HIGH);
  digitalWrite(dcMotorPin2, LOW);
  delay(300);  // Adjust this delay to control motor speed and angle
  stopMotor();
}

void shiftMotorLeft() {
  digitalWrite(dcMotorPin1, LOW);
  digitalWrite(dcMotorPin2, HIGH);
  delay(300);  // Adjust this delay to control motor speed and angle
  stopMotor();
}

void stopMotor() {
  digitalWrite(dcMotorPin1, LOW);
  digitalWrite(dcMotorPin2, LOW);
}
