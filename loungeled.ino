#include <GPNBadge.hpp>
#include <FS.h>
#include "rboot.h"
#include "rboot-api.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "url-encode.h"
const static int bufferSize = 12;
char packetBuffer[bufferSize];
unsigned int localPort = 8888;
WiFiUDP UDP;
boolean udpConnected = false;

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Badge badge;
void setup() {
  badge.init();
  badge.setBacklight(true);
  pixels.setPixelColor(1, 0, 255, 0);
  pixels.show();
  delay(100);
  pixels.show();
  Serial.begin(115200);


  rboot_config rboot_config = rboot_get_config();
  SPIFFS.begin();
  File f = SPIFFS.open("/rom"+String(rboot_config.current_rom),"w");
  f.println("LOUNGELED\n");
  setup_wifi();
  UDP.begin(localPort);
  
  

}

void loop() {
int packetSize = UDP.parsePacket();
if(packetSize)
{
  tft.fillScreen(BLACK);
  tft.setCursor(0,0);
  
  if(packetSize == bufferSize){
    tft.print("Packet!");
    UDP.read(packetBuffer, bufferSize);
    for (int i = 0; i < 12; i = i+3){
      unsigned char r = packetBuffer[i];
      unsigned char g = packetBuffer[i+1];
      unsigned char b = packetBuffer[i+2];
      pixels.setPixelColor(i, pixels.Color(r, g, b));
      tft.printf("%d, %d, %d\n", r, g, b);
     
    }
    pixels.show();
    tft.writeFramebuffer();
 
    }

}
}


void setup_wifi() {
    int ledVal = 0;
    bool up = true;
    tft.setFont();
    tft.setTextSize(1);
    tft.setTextColor(RED);
    tft.setCursor(0, 2);
    tft.println("Reading wifi.conf...");
    tft.writeFramebuffer();

    tft.setTextColor(WHITE);

    File wifiConf = SPIFFS.open("/wifi.conf", "r");

    if (!wifiConf) {
      tft.println("File not found!");
      tft.writeFramebuffer();
      delay(5000);
    }
    String configString;
    while (wifiConf.available()) {
      configString += char(wifiConf.read());
    }
    wifiConf.close();
    UrlDecode confParse(configString.c_str());
    Serial.println(configString);
    configString = String();
    char* ssid = confParse.getKey("ssid");
    char* pw = confParse.getKey("pw");
    ssid = "loungeled";
    pw = "thisiswhythewifiisslow";
    Serial.printf("Connecting to wifi '%s' with password '%s'...\n", ssid, pw);

    tft.println("Connecting to SSID:");
    tft.println(ssid);
    tft.writeFramebuffer();

    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    delay(400);
    WiFi.mode(WIFI_AP);
    delay(400);
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    delay(400);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pw);

    unsigned long startTime = millis();
    //delete[] pw;
    while (WiFi.status() != WL_CONNECTED) {
      pixels.setPixelColor(1, pixels.Color(0, 0, ledVal));
      pixels.setPixelColor(2, pixels.Color(0, 0, ledVal));
      pixels.setPixelColor(3, pixels.Color(0, 0, ledVal));
      pixels.setPixelColor(0, pixels.Color(0, 0, ledVal));
      pixels.show();
      if (ledVal == 100) {
        up = false;
      }
      if (ledVal == 0) {
        up = true;
      }
      if (up) {
        //ui->root->setSub("Connecting...");
        //ui->draw();
        ledVal++;
      } else {

        ledVal--;
      }
      if (millis() - startTime > 30 * 1000) {

        pixels.setPixelColor(1, pixels.Color(50, 0, 0));
        pixels.setPixelColor(2, pixels.Color(50, 0, 0));
        pixels.setPixelColor(3, pixels.Color(50, 0, 0));
        pixels.setPixelColor(0, pixels.Color(50, 0, 0));
        pixels.show();
        tft.setTextColor(RED);
        tft.println("");
        tft.println("WiFi connect failed!");
        delay(2000);

        ESP.reset();
      }
      delay(10);
    }

    pixels.setPixelColor(1, pixels.Color(0, 0, 0));
    pixels.setPixelColor(2, pixels.Color(0, 0, 0));
    pixels.setPixelColor(3, pixels.Color(0, 0, 0));
    pixels.setPixelColor(0, pixels.Color(0, 0, 0));
    pixels.show();
    
    //delete[] ssid;
    tft.setTextColor(GREEN);
    tft.println("");
    tft.println("WiFi connected!");
    tft.setTextColor(WHITE);
    tft.println(WiFi.localIP().toString());
    tft.writeFramebuffer();
    Serial.println("WiFi connected");
    Serial.println("");
    Serial.println(WiFi.localIP());

    pixels.show();

    delay(2000);

    pixels.setPixelColor(1, pixels.Color(0, 0, 0));
    pixels.setPixelColor(2, pixels.Color(0, 0, 0));
    pixels.setPixelColor(3, pixels.Color(0, 0, 0));
    pixels.setPixelColor(0, pixels.Color(0, 0, 0));
    pixels.show();

      tft.fillScreen(BLACK);
  tft.setCursor(0, 0);
    tft.writeFramebuffer();
  }
