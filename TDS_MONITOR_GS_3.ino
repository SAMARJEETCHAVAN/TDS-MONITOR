#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "Wire.h"
#include "LiquidCrystal_I2C.h"
String readString;
LiquidCrystal_I2C lcd(0x27, 20, 4);
int led = 12;
int led2 = 2;
const char* ssid = "FENET";
const char* password = "12345678";
const char* host = "script.google.com";
const int httpsPort = 443;
int button = 14;
int temp = 0;
int dataval = 0;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
WiFiClientSecure client;
const char* fingerprint = "46 B2 C3 44 9C 59 09 8B 01 B6 F8 BD 4C FB 00 74 91 2F EF F6";
String GAS_ID = "AKfycbwDoXb3r-BFRE_WaZa3-0gnBERXN6xqpky2ttM0FDt_w-EoRT_TvUi78H1PKo1oy-V5ZA";  // Replace by your GAS service id
void Connectwifi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  digitalWrite(led2, HIGH);
  timeClient.begin();
  timeClient.setTimeOffset(19800);
  delay(2000);
}
void setup()
{
  Serial.begin(115200);
  Wire.setClock(10000);
  lcd.begin();
  lcd.backlight();/*
  lcd.setCursor(1, 0);
  lcd.print("");
  lcd.setCursor(1, 1);
  lcd.print("");*/
  delay(1000);
  lcd.setCursor(1, 0);
  lcd.print("WELCOME TO");
  lcd.setCursor(1, 1);
  lcd.print("FRESH EXPRESS");
  delay(2000);
  pinMode(button, INPUT);
  pinMode(led2, OUTPUT);
  pinMode(led, OUTPUT);   // declare LED as output
  digitalWrite(led, LOW);
  digitalWrite(led2, LOW);
  lcd.noBacklight();
  //sendData(" "," "," "," "," "," "," ","tds booted"," "," "," ");  // Send test data
}

void loop()
{
  Wire.setClock(10000);
  temp = digitalRead(button);
  if (temp == HIGH){
    lcd.backlight();
/*    lcd.setCursor(1, 0);
    lcd.print("");
    lcd.setCursor(1, 1);
    lcd.print("");*/
    delay(1000);
    lcd.setCursor(1, 0);
    lcd.print("BUTTON PRESSED");
    lcd.setCursor(1, 1);
    lcd.print("GETTING TDS     ");
    delay(1000);
    digitalWrite(led, HIGH);
    Connectwifi();
    dataval = (analogRead(A0));
    Serial.println(dataval);
/*    lcd.setCursor(1, 0);
    lcd.print("");
    lcd.setCursor(1, 1);
    lcd.print("");*/
    delay(1000);
    lcd.setCursor(1, 0);
    lcd.print("TDS READ VALUE ");
    lcd.setCursor(1, 1);
    lcd.print(String(dataval));
    delay(1000);
    sendData("a", "b", "c", "d", "e", "f", "g", String(dataval), "i", "j", "k");
    delay(2000);
    lcd.noBacklight();
    }
  else {
    //lcd.noBacklight();
    Serial.println("Please press button");
/*    lcd.setCursor(1, 0);
    lcd.print("");
    lcd.setCursor(1, 1);
    lcd.print("");*/
    delay(1000);  
    lcd.setCursor(1, 0);
    lcd.print("STRT FENET WIFI");
    lcd.setCursor(1, 1);
    lcd.print("PRESS RED BTTON");
    delay(1000);
  }
}
void sendData(String a, String b, String c, String d, String e, String f, String g, String h, String i, String j, String k)
{
  Connectwifi();
  client.setInsecure();
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    /*lcd.setCursor(1, 0);
    lcd.print("");
    lcd.setCursor(1, 1);
    lcd.print("");*/
    delay(1000);
    lcd.setCursor(1, 0);
    lcd.print("ConectionFailed");
    lcd.setCursor(1, 1);
    lcd.print("Pls Try Again!");
    return;
  }

  if (client.verify(fingerprint, host)) {
  Serial.println("certificate matches");
  } else {
  Serial.println("certificate doesn't match");
  }
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();
  String formattedTime = timeClient.getFormattedTime();
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  int currentSecond = timeClient.getSeconds();
  struct tm *ptm = gmtime ((time_t *)&epochTime); 
  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon+1;
  int currentYear = ptm->tm_year+1900;
  String currentDate = String(currentYear) + "/" + String(currentMonth) + "/" + String(monthDay)+ "/" + String(currentHour)+ "/" + String(currentMinute);
  String url = "/macros/s/" + GAS_ID + "/exec?Date="+currentDate+"&Time="+"_"+"&DT="+"5"+"&DID="+"1"+"&NSC="+"1"+"&ST="+"6"+"&SID="+"1"+"&VALUE1=" + h + "&VALUE2=" + "_" + "&VALUE3=" + "_" + "&VALUE4=" + "_";
  Serial.print("requesting URL: ");
  Serial.println(url);
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
         "Host: " + host + "\r\n" +
         "User-Agent: BuildFailureDetectorESP8266\r\n" +
         "Connection: close\r\n\r\n");

  Serial.println("request sent");
  while (client.connected()) {
  String line = client.readStringUntil('\n');
  if (line == "\r") {
    Serial.println("headers received");
    break;
  }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
  Serial.println("esp8266/Arduino CI successfull!");
  } else {
  Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.println("reply was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("closing connection");
  WiFi.disconnect();/*
    lcd.setCursor(1, 0);
    lcd.print("");
    lcd.setCursor(1, 1);
    lcd.print("");*/
    delay(1000);
    lcd.setCursor(1, 0);
    lcd.print("Data uploaded ");
    lcd.setCursor(1, 1);
    lcd.print("Successfully!");
  digitalWrite(led, LOW);
  digitalWrite(led2, LOW);
}
