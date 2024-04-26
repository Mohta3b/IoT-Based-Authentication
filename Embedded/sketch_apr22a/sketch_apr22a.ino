#include <LiquidCrystal_I2C.h>
#include <EtherCard.h>

static byte myip[] = {192, 168, 2, 2}; // Arduino's static IP address
static byte gwip[] = {192, 168, 2, 1}; // Gateway IP address

// MAC address of the Arduino
static byte mymac[] = {0x72, 0x19, 0x69, 0x2D, 0x30, 0x38};

byte Ethernet::buffer[700];

// Welcome page HTML content (stored in PROGMEM)
const char welcomePage[] PROGMEM =
    "<!DOCTYPE html>"
    "<html lang='en'>"
    "<head>"
    "<meta charset='UTF-8'>"
    "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
    "<title>RFID Scanner</title>"
    "<style>"
    "body {"
    "font-family: Arial, sans-serif;"
    "background-color: #f0f0f0;"
    "text-align: center;"
    "padding: 20px;"
    "}"
    "h1 {"
    "color: #333333;"
    "}"
    "p {"
    "color: #666666;"
    "}"
    "</style>"
    "</head>"
    "<body>"
    "<h1>First PCP Course Project</h1>"
    "<p>Scan your RFID and if your id was valid, your access will be granted! </p>"
    "</body>"
    "</html>";

// Ethernet buffer size

LiquidCrystal_I2C lcd(0x20, 16, 2); // run ic2_scanner sketch and get the IC2 address, which is 0x3f in my case, it could be 0x3f in many cases

const int redLedPin = 9;
const int greenLedPin = 8;
const int dcMotorPin1 = 7;
const int dcMotorPin2 = 6;
const int terminalPin = 1;

#define VALID_RFID_TIME 10000
#define RFID_LEN 10

enum AccessStatus
{
  IDEAL,
  GRANT,
  DENY
};

AccessStatus accessStatus = IDEAL;

const int MAX_RFIDS = 3;
String rfidList[MAX_RFIDS] = {
    "1234567890",
    "1111111111",
    "0000000000"};

struct UserInfo
{
  String name;
  String id;
  bool access = false;
  // accessTimeDate
  String accessTimeDate;
  unsigned long int accessTime = 0;
};

UserInfo userInfo;

bool isDoorOpen = false;

void setup()
{
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

  // setup LCD
  lcd.backlight();
  lcd.print("  Scan");
  lcd.setCursor(0, 1);
  lcd.print(" Your ID");
  // delay(10000);

  Serial.begin(9600);

  // Begin Ethernet communication with buffer size and MAC address
  ether.begin(sizeof Ethernet::buffer, mymac, SS);

  // Configure static IP and gateway IP
  ether.staticSetup(myip, gwip);
}

String inputRfid = "";

void loop()
{
  // put your main code here, to run repeatedly:
  readInput();
  
  if (inputRfid.length() > RFID_LEN || accessStatus == DENY)
  {
    inputRfid = "";
    Serial.flush();
  }
  if (inputRfid.length() == RFID_LEN)
  {                       // if input is entered on terminal
    Serial.println(inputRfid);
    checkRFID(inputRfid); // set access status to grant or deny
    inputRfid = "";
  }
  if (accessStatus == GRANT)
  {
    if (userInfo.accessTime == 0)
    {
      grantAccess();
    }
    else if (millis() - userInfo.accessTime >= VALID_RFID_TIME)
    {
      closeDoor();
      idealMode();
    }
  }
  else if (accessStatus == DENY)
  {
    denyAccess();
    delay(3000);
    idealMode();
  }

  // delay(100);  // Delay to avoid rapid toggling
}

void wifi()
{
  // Handle incoming Ethernet packets and get the position of the data
  word pos = ether.packetLoop(ether.packetReceive());

  // If data is received
  if (pos)
  {
    // Extract data from Ethernet buffer
    char *data = (char *)Ethernet::buffer + pos;

    // Copy the welcome page content to the Ethernet buffer
    memcpy_P(ether.tcpOffset(), welcomePage, sizeof welcomePage);

    // Send HTTP response with the welcome page to the client
    ether.httpServerReply(sizeof welcomePage - 1);
  }
}

void idealMode()
{
  accessStatus = IDEAL;
  Serial.flush();
  inputRfid = "";
  lcd.clear();
  lcd.print(" Scan");
  lcd.setCursor(0, 1);
  lcd.print(" Your ID");
  digitalWrite(redLedPin, LOW);   // Turn off red LED
  digitalWrite(greenLedPin, LOW); // Turn off green LED
}

// void checkRFID(String inputRfid) {
//   userInfo.access = false;
//   accessStatus = DENY;
//   for (int i = 0; i < MAX_RFIDS; i++) {
//     if (inputRfid == rfidList[i]) {
//       accessStatus = GRANT;
//       // userInfo.accessTime = millis();
//       userInfo.access = true;
//       userInfo.id = rfidList[i];
//       userInfo.name = "John";
//       break;
//     }
//   }
// }

void checkRFID(String inputRfid)
{
  // sent the RFID to the server and get the response
  // if the response is "GRANT", set the access status to GRANT
  // if the response is "DENY", set the access status to DENY
  userInfo.access = false;
  accessStatus = DENY;
  UserInfo serverResponse = sendRfidToServer(inputRfid);
  if (serverResponse.access)
  {
    accessStatus = GRANT;
    userInfo = serverResponse;
  }
}

UserInfo sendRfidToServer(String inputRfid) {
  // Assuming 'sent' indicates whether the packet was successfully sent
  bool sent = sendPacketToServer(inputRfid);

  if (!sent) {
    Serial.println("Failed to send packet");
    return UserInfo(); // Return empty UserInfo object if packet not sent
  }

  // Handle incoming Ethernet packets and fill UserInfo based on received data
  word pos = ether.packetLoop(ether.packetReceive());

  if (pos) {
    char *data = (char *)Ethernet::buffer + pos;
    String response = String(data);

    // Response is like "Granted-1234567890-2024-04-22 12:00:00" or "Denied"
    if (response.startsWith("Granted")) {
      return parseGrantedResponse(response);
    } else {
      return UserInfo(); // Return empty UserInfo object if access denied
    }
  }

  return UserInfo(); // Return empty UserInfo object if no response received
}

bool sendPacketToServer(String packet) {
  // Create the packet
  memcpy(Ethernet::buffer + 14, mymac, 6); // Destination MAC address
  memcpy(Ethernet::buffer + 20, myip, 4);    // Source IP address
  memcpy(Ethernet::buffer + 30, gwip, 4); // Destination IP address
  memcpy(Ethernet::buffer + 42, packet.c_str(), packet.length()); // Payload

  // Send the packet
  ether.packetSend(sizeof(packet) - 1 + 42);

  return true;
}

UserInfo parseGrantedResponse(String response) {
  UserInfo user;
  user.access = true;
  user.id = response.substring(8, 18);
  user.accessTimeDate = response.substring(19);
  return user;
}


void grantAccess()
{
  userInfo.accessTime = millis();
  // turn on green LED
  digitalWrite(greenLedPin, HIGH); // Turn on green LED
  digitalWrite(redLedPin, LOW);    // Turn off red LED

  // display the information of the person on the LCD
  lcdShowMessage(userInfo);

  // open door (turn right the DC motor 90 degrees)
  if (!isDoorOpen)
    shiftMotorRight();
  isDoorOpen = true;
}

void denyAccess()
{
  // turn on red LED
  digitalWrite(redLedPin, HIGH);  // Turn on red LED
  digitalWrite(greenLedPin, LOW); // Turn off green LED

  // display "ACCESS DENIED" on the LCD
  lcdShowMessage(userInfo);

  // close door (turn left DC motor 90 degrees)
  if (isDoorOpen)
    closeDoor();
}

void closeDoor()
{
  userInfo.accessTime = 0;
  isDoorOpen = false;

  shiftMotorLeft();
}

void lcdShowMessage(UserInfo user)
{
  lcd.clear();
  if (user.access == false)
  {
    lcd.print(" ACCESS DENIED!");
    lcd.setCursor(0, 1);
    lcd.print("Wait 3 seconds!");
  }
  else
  {
    lcd.print(" ACCESS GRANTED!");
    lcd.setCursor(0, 1);
    lcd.print(" Date:" + user.accessTimeDate + " Id:" + user.id);
  }
}

void readInput()
{
  while (Serial.available() > 0)
  {
    char c = Serial.read();
    inputRfid += c;
  }
}

void shiftMotorRight()
{
  digitalWrite(dcMotorPin1, HIGH);
  digitalWrite(dcMotorPin2, LOW);
  delay(300); // Adjust this delay to control motor speed and angle
  stopMotor();
}

void shiftMotorLeft()
{
  digitalWrite(dcMotorPin1, LOW);
  digitalWrite(dcMotorPin2, HIGH);
  delay(300); // Adjust this delay to control motor speed and angle
  stopMotor();
}

void stopMotor()
{
  digitalWrite(dcMotorPin1, LOW);
  digitalWrite(dcMotorPin2, LOW);
}
