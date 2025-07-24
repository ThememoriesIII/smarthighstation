#include <Arduino.h>
#include <Wire.h>

#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <EEPROM.h>
// include wifi section
#include <DNSServer.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <ESP_Google_Sheet_Client.h>
#include <GS_SDHelper.h>

#include <HX711.h>
//#include <HX711_ADC.h>
#include "ultrasonic.h"
#include "time.h"

WiFiManager wifimanager;
#define MAX_MSG 512
char msg[MAX_MSG];

#define PROJECT_ID "1afLHPQHn5fNF0im8jPzYAW3lXwNND1Cxzb2AwZ8Sk8ACCdsdOJ_Xl8p5"

// Service Account's client email
#define CLIENT_EMAIL "iotdevicetedsaban2@highmeter.iam.gserviceaccount.com"

const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----\nMIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQCqg/VNCEw684yU\niT21BVx3QC6pi09oUZ5cTPdtOWAFmtGeWkDc8NB9eiNVXqaT9G4iL7B3e4UnGaGB\nb1ZxcNCgHPp9Y6/MILSuFVRcceAPCsg8l6PYaQbJ2/SKQIzkFduYHu+nzcMtMAFI\n1Y/fonuwEAb+aBB8e4w5Ai/TFAmiA4kdUOE78fF4hkTY/bLSEw46qPLEeXUHL0R5\nrqPWw5jLxkzzUhECTgYGvPtZfnRpBwcyh6G0De2AIJEkFfx1xihSa9JAUvsikLhl\nMu97wxcvN6NaRrxMwxMRGxBsP5eXcVpngEJ43xu0y6dgLwhu3W06EpoWfMjWz/EW\npXYKpx+HAgMBAAECggEAHVU5+5hz1KhVBLBfRya282S9Ijlu3Zk8ztAGuGloVU5+\nxOjtw5RBqXQ7Y9I2gblIwOTHTKaZD1Lfb67Bhh5MJV8W0D833Lu7mT7eFlEYpD3M\nmG/VpcLbghouhSH5mrsP3alkvM8uxjU941OpvmTrNixrp5QrUd7HDn+Lzpnh1lbR\nTtCrGVNi/lgVLb/G30QcxLQxh+svD3EP7N+NIPRUFenrMy7IFkUJL3KkO9yRT9nA\nVoxOpPnq6VWhKlsDw8vepPH4QLnH1n6EkGrJwGym5tB8Aq7VCkBVDO19mtK2dov+\nRHwEcZs48IEWsgzIm4/qivKit90C0B3qwTeebDEhkQKBgQDf++FzOHySyerxNFcB\n1SKk+pzIMnpg60MUYRiqZ1IqeGuuq/FTLM0u1P/1+an5wrJ4I2h0V7rwYq6tmFSI\n6n0cxJ9Ladlnfai2l+t84iCJN67kRb4IjBKrJmwtrsGYb01xYVw412XwEFdHgDZE\nb8oub1oLCFMtV0B8Cv6FS4uaJQKBgQDC44lFuzGDhORdF9hn5JMhKMYKmJxTZv1S\ny0Hd/l+9v+TVej4EKCG1zckbwItKRioL/J+ggIGtVtYPsFr+fkJ+15VGmzVlrcEE\ngWgYvkZspR8MfCZum43zzzH8B1+4vHPpYF9T2J0gS4hrnH9+pBk80cdfFildb9Sa\ncOrXxeJlOwKBgQCYQPrmvL4JdhIrdh2y1DPhm63ZdF615RZLExmWM/epgQQJzz2T\nAYBUVlvL07OUYKG6gq8tbrlXUG1W+ZZ1H5h+ZrW1qLUs4UE/JZPg50GgpOZQq4va\n+YRzLQIaJ5HX0cj9ymXLCbh0cFcz9lZtPORdnjnCCB2qU4Y1C7Ap4cl6tQKBgDU1\ndFQigKk8zjCQgxgScw0ZmcBo3D11H/LL5cbbXX5r9ulF/8qYvmNptN+r2RvFPlOZ\nfGeQzlbqH3WXG7Dh/futlmb5l0GUh99YCXi/8nBVQL3i44dNVxta1XXO+ZZxdWAh\nXMWCnZuko7Flv/VujilpyPowDckh8DTeYW/Z6i7jAoGAH+aY+pg53UWI+DtCArSE\nSp4GJhWG3P4XnagOdjWMdAJao0eJ8sAr3uoDy/0nogpfn9XwIqcPQ7dPwordI0mY\nEr3d1UFWbbqWpAcL4P+bBSBR+rbaQbH9YCFA53HrS/isEmudLT5raNZ2+2u7TKVd\nbnIevyiVv6BdZsj08PW06Xg=\n-----END PRIVATE KEY-----\n";

// The ID of the spreadsheet where you'll publish the data
const char spreadsheetId[] = "13J91O66o90Y1YAwqQL7amD3Fb7r2CXVPejTfjx5GP3Q";

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;

// Token Callback function
void tokenStatusCallback(TokenInfo info);

LiquidCrystal_I2C lcd(0x27, 16, 2);

char lcd_buffer0[17];
char lcd_buffer1[17];

const int LOADCELL_DOUT_PIN = 5;
const int LOADCELL_SCK_PIN = 4;
const long LOADCELL_OFFSET = 50682624;
const long LOADCELL_DIVIDER = 5895655;

HX711 scale;

const int calVal_eepromAdress = 10;
float calibrationValue = 0;
unsigned long t = 0;
int serialPrintInterval = 1000;
byte d_toggle = 0;
long offset_tare=0;

ULTRASONIC uc(13, 12);
float messure_centemeter = 0;

const byte ROWS = 4; // four rows
const byte COLS = 4; // four columns
int Token_Number = 0;
char hexaKeys[ROWS][COLS] =
    {                          // 26 25 33 32
        {'1', '2', '3', 'A'},  // 14// 1  4  7  *
        {'4', '5', '6', 'B'},  // 27// 2  5  8  0
        {'7', '8', '9', 'C'},  // 18// A  B  C  D
        {'*', '0', '#', 'D'}}; // 19// 3  6  9  #

byte rowPins[ROWS] = {14, 27, 32, 33}; // ROW
byte colPins[COLS] = {16, 17, 18, 19}; // COL

Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

byte enter_mode = false;
byte messure_mode = false;
byte sendtointernet_mode = false;

float higth = 0;
float weight = 0;
String student_id = "";
bool ready = false;
// NTP server to request epoch time
const char *ntpServer = "pool.ntp.org";

// Variable to save current epoch time
unsigned long epochTime;

// Function that gets current epoch time
unsigned long getTime()
{
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    // Serial.println("Failed to obtain time");
    return (0);
  }
  time(&now);
  return now;
}

void configModeCallback(WiFiManager *mywifimanager)
{
  lcd.setCursor(0, 0);
  sprintf(lcd_buffer0, "%-16s", "Enter Config");
  lcd.print(lcd_buffer0);
  lcd.setCursor(0, 1);
  sprintf(lcd_buffer1, "%-16s", mywifimanager->getConfigPortalSSID());
  lcd.print(lcd_buffer1);
}

float messure_kilogram()
{
  float avg_weight=0;

  avg_weight=scale.get_units(10);

  return avg_weight/1000.0f;
}

void calibrate() {
  // Remove any calibration values and clear the scale
  scale.set_scale();
  scale.tare();

  // Prompt the user
  Serial.println("Add your known weight to the scale, enter the weight and press <Enter>");
  int userInput = -123;
  String inputString = "";
  // Loop until we receive an input (which will change the value of userInput
  while (userInput == -123) {
    // Read serial input:
    while (Serial.available() > 0) {
      int inputChar = Serial.read();
      if (isDigit(inputChar)) {
        // convert the incoming byte to a char and add it to the string:
        inputString += (char)inputChar;
      }
      // if you get a newline, print the string, then the string's value:
      if (inputChar == '\n') {
        userInput = inputString.toInt();
      }
    }
  }

  // Now get the reading from the scale
  float calReading = scale.get_units(10);

  Serial.print("Setting the cal to ");
  Serial.println(calReading / userInput);

  scale.set_scale(calReading / userInput);
}

void setup()
{
  uc.begin();
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Start");
  Serial.begin(115200);
  GSheet.printf("ESP Google Sheet Client v%s\n\n", ESP_GOOGLE_SHEET_CLIENT_VERSION);
  wifimanager.setTimeout(30);
  wifimanager.setBreakAfterConfig(true);
  wifimanager.setAPCallback(configModeCallback);
  // wifimanager.autoConnect();
  if (!wifimanager.autoConnect("SmartHeihtStation"))
  {
    lcd.setCursor(0, 1);
    sprintf(lcd_buffer1, "%-16s", "failconnect-wifi");
    lcd.print(lcd_buffer1);
    delay(3000);
    ESP.restart();
    delay(5000);
  }
  lcd.setCursor(0, 1);
  sprintf(lcd_buffer1, "%-16s", "connect-wifiRedy");
  lcd.print(lcd_buffer1);
  // Set the callback for Google API access token generation status (for debug only)
  GSheet.setTokenCallback(tokenStatusCallback);

  // Set the seconds to refresh the auth token before expire (60 to 3540, default is 300 seconds)
  GSheet.setPrerefreshSeconds(10 * 60);

  // Begin the access token generation for Google API authentication
  GSheet.begin(CLIENT_EMAIL, PROJECT_ID, PRIVATE_KEY);

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_gain(128);
  scale.set_scale((float)25.83f);
  scale.tare(); 

  ready = GSheet.ready();
  lastTime = millis();
  configTime(0, 0, ntpServer);
  lcd.setCursor(0, 1);
  sprintf(lcd_buffer1, "%-16s", "                ");
  lcd.print(lcd_buffer1);
}

void loop()
{
  char key = keypad.getKey();
  String buffer1 = "";
  lcd.setCursor(0, 0);
  sprintf(lcd_buffer0, "%-16s", "EnterStudentCode");
  lcd.print(lcd_buffer0);
  Serial.print(lcd_buffer0);
  Serial.print(key);
  while (key != 'A')
  {
    key = keypad.getKey();
    Serial.print(key);
  }
  if (key == 'A' || key == 'a')
  {
    enter_mode = true;
  }
  Serial.println("");
  while (enter_mode == true)
  {
    //&& key != 'D' && key != 'd'
    key = keypad.getKey();
    if (key != '\0' && key != 'A' && key != 'a' && key != 'B' && key != 'b' && key != 'C' && key != 'c')
    {
      if (key == 'D' || key == 'd')
      {
        d_toggle = true;
        while (d_toggle)
        {
          key = keypad.getKey();
          if (key == 'D' || key == 'd')
          {
            d_toggle = false;
          }
          messure_kilogram();
          key = 0;
        }
      }
      if (key == '*')
      {
        buffer1 = "";
        lcd.setCursor(0, 1);
        sprintf(lcd_buffer1, "%-16s", "clear data");
        lcd.print(lcd_buffer1);
        Serial.println(lcd_buffer1);
      }
      if (key == '#')
      {
        enter_mode = false;
        // select string
        buffer1[buffer1.length() + 1] = '\0';

        lcd.setCursor(0, 1);
        sprintf(lcd_buffer1, "%-16s", "FinishData");
        lcd.print(lcd_buffer1);
        Serial.println(lcd_buffer1);
        Serial.println(buffer1);
        messure_mode = true;
        enter_mode = false;
        break;
      }
      if (buffer1.length() < 5 && key != '*')
      {
        buffer1 += key;
      }
      lcd.setCursor(0, 1);
      sprintf(lcd_buffer1, "%-16s", &buffer1);
      lcd.print(lcd_buffer1);
      Serial.println(lcd_buffer1);
    }
  }
  if (messure_mode == true)
  {
    messure_centemeter = uc.messure();
    higth = float(200.0) - messure_centemeter;
    lcd.setCursor(0, 0);
    sprintf(lcd_buffer0, "%-16s", "Higth is");
    lcd.print(lcd_buffer0);
    Serial.println(lcd_buffer0);
    lcd.setCursor(0, 1);
    sprintf(lcd_buffer1, "%-16f", higth);
    lcd.print(lcd_buffer1);
    Serial.println(higth);
    delay(5000);
    lcd.setCursor(0, 0);
    sprintf(lcd_buffer0, "%-16s", "Weigth is");
    lcd.print(lcd_buffer0);
    Serial.println(lcd_buffer0);
    weight = messure_kilogram();
    lcd.setCursor(0, 1);
    sprintf(lcd_buffer1, "%-16f", weight);
    lcd.print(lcd_buffer1);
    Serial.println(weight);
    delay(5000);
    messure_mode = false;
    sendtointernet_mode = true;
  }
  if (sendtointernet_mode == true)
  {
    ready = GSheet.ready();
    if (ready)
    {
      while (millis() - lastTime <= timerDelay)
      { /*delay for last time*/
      }
      lastTime = millis();

      FirebaseJson response;

      Serial.println("\nAppend spreadsheet values...");
      Serial.println("----------------------------");

      FirebaseJson valueRange;
      // New BME280 sensor readings

      byte padding0 = 5 - buffer1.length();
      String pad0 = "";
      for (byte i = 0; i < padding0; i++)
      {
        pad0 += "0";
      }
      student_id += pad0;
      student_id += buffer1;
      epochTime = getTime();
      time_t rawtime = epochTime; // ตัวอย่าง epoch
      struct tm *timeinfo = localtime(&rawtime);
      char buffer[50];
      sprintf(buffer, "%d/%d/%d, %d:%d:%d", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
      valueRange.add("majorDimension", "COLUMNS");
      valueRange.set("values/[0]/[0]", buffer);
      valueRange.set("values/[1]/[0]", student_id);
      valueRange.set("values/[2]/[0]", weight);
      valueRange.set("values/[3]/[0]", higth);
      bool success = GSheet.values.append(&response /* returned response */, spreadsheetId /* spreadsheet Id to append */, "raw_data!A1" /* range to append */, &valueRange /* data range to append */);
      if (success)
      {
        response.toString(Serial, true);
        valueRange.clear();
      }
      else
      {
        Serial.println(GSheet.errorReason());
      }
      Serial.println();
      Serial.println(ESP.getFreeHeap());
      student_id = "";
      pad0 = "";
      buffer1 = "";
    }
    lcd.setCursor(0, 1);
    sprintf(lcd_buffer1, "%-16s", "                ");
    lcd.print(lcd_buffer1);
    sendtointernet_mode = false;
  }
}

void tokenStatusCallback(TokenInfo info)
{
  if (info.status == token_status_error)
  {
    GSheet.printf("Token info: type = %s, status = %s\n", GSheet.getTokenType(info).c_str(), GSheet.getTokenStatus(info).c_str());
    GSheet.printf("Token error: %s\n", GSheet.getTokenError(info).c_str());
  }
  else
  {
    GSheet.printf("Token info: type = %s, status = %s\n", GSheet.getTokenType(info).c_str(), GSheet.getTokenStatus(info).c_str());
  }
}