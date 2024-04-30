#include <LiquidCrystal_I2C.h>
#include <EtherCard.h>

static byte myip[] = { 192, 168, 2, 2 };  // Arduino's static IP address
static byte gwip[] = { 192, 168, 2, 1 };  // Gateway IP address

// MAC address of the Arduino
static byte mymac[] = { 0x72, 0x19, 0x69, 0x2D, 0x30, 0x38 };

// website is localhost on port 80
const byte serverIp[] = { 192, 168, 2, 1 };
const char website[] PROGMEM = "localhost";

// subdomain to connect to is /verification
const char url[] PROGMEM = "verification";
static byte hisip[] = { 192, 168, 2, 1 };

// Ethernet buffer size
byte Ethernet::buffer[700];

LiquidCrystal_I2C lcd(0x20, 16, 2);  // run ic2_scanner sketch and get the IC2 address, which is 0x3f in my case, it could be 0x3f in many cases

const int redLedPin = 9;
const int greenLedPin = 8;
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

struct UserInfo {
  String name;
  String id;
  bool access = false;
  String accessTimeDate;
  unsigned long int accessTime = 0;
};

UserInfo userInfo;

bool isDoorOpen = false;

int count = 0;

Stash stash;
static byte session;

void setup() {
  lcd.init();

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

  // setup LCD
  lcd.backlight();
  lcd.print("  Scan");
  lcd.setCursor(0, 1);
  lcd.print(" Your ID");
  // delay(10000);

  Serial.begin(9600);


  // Begin Ethernet communication with buffer size and MAC address
  if (ether.begin(sizeof Ethernet::buffer, mymac, SS) == 0) {
    // logTerminal.println("Failed to initialize ethernet card.");
    Serial.println("Failed to initialize ethernet card.");
  }
  ether.staticSetup(myip, gwip);

  ether.copyIp(ether.hisip, serverIp);
  ether.persistTcpConnection(true);
}

String inputRfid = "";

void loop() {
  // put your main code here, to run repeatedly:
  readInput();

  word pos = ether.packetLoop(ether.packetReceive());

  if (inputRfid.length() > RFID_LEN || accessStatus == DENY) {
    inputRfid = "";
    Serial.flush();
  }
  if (inputRfid.length() == RFID_LEN) {  // if input is entered on terminal
    Serial.println(inputRfid);
    sendRFIDtoServer(inputRfid);  // set access status to grant or deny
    inputRfid = "";
  }

  recieveServerResponse();

  if (accessStatus == GRANT) {
    if (userInfo.accessTime == 0) {
      grantAccess();
    } else if (millis() - userInfo.accessTime >= VALID_RFID_TIME) {
      closeDoor();
      idealMode();
    }
  } else if (accessStatus == DENY) {
    denyAccess();
    delay(3000);
    idealMode();
  }

  // delay(100);  // Delay to avoid rapid toggling
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


void sendRFIDtoServer(String rfid) {
  byte sd = stash.create();
  stash.print("{\"rfid\":\"");
  stash.print(inputRfid);
  stash.print("\"}");
  stash.save();

  Stash::prepare(
    PSTR(
      "POST http://$F/$F HTTP/1.1"
      "\r\n"
      "Host: $F"
      "\r\n"
      "Content-Length: $D"
      "\r\n"
      "Content-Type: application/json"
      "\r\n"
      "\r\n"
      "$H"),
    website, url, website, stash.size(), sd);

  session = ether.tcpSend();
}

void recieveServerResponse() {
  char* response = ether.tcpReply(session);
  if (response != nullptr) {
    userInfo = parseUserInfoResponse(response);
    if (userInfo.access == true) {
      accessStatus = GRANT;
    } else {
      accessStatus = DENY;
    }
    // Serial.println("Response: ");
    // Serial.println(response);
  }
}

UserInfo parseUserInfoResponse(char* response) {
  /* response is json format like :
  {
    "date": "04/30/2024",
    "rfid_tag": "1234567899",
    "time": "00:07"
  }
  and with statusCode = ok or Unauthorized
  */
  UserInfo user;
  String res = String(response);
  int start = res.indexOf("{");
  int end = res.indexOf("}");
  String json = res.substring(start, end + 1);
  Serial.println(json);
  int dateStart = json.indexOf("\"date\":") + 8;
  int dateEnd = json.indexOf(",", dateStart);
  user.accessTimeDate = json.substring(dateStart, dateEnd);
  int rfidStart = json.indexOf("\"rfid_tag\":") + 12;
  int rfidEnd = json.indexOf(",", rfidStart);
  user.id = json.substring(rfidStart, rfidEnd);
  int timeStart = json.indexOf("\"time\":") + 8;
  int timeEnd = json.indexOf("}", timeStart);
  user.accessTimeDate += " " + json.substring(timeStart, timeEnd);

  if (res.indexOf("Unauthorized") != -1) {
    user.access = false;
  } else {
    user.access = true;
  }

  return user;
}

void grantAccess() {
  userInfo.accessTime = millis();
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

void closeDoor() {
  userInfo.accessTime = 0;
  isDoorOpen = false;

  shiftMotorLeft();
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
    lcd.print(" Date:" + user.accessTimeDate + " Id:" + user.id);
  }
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
