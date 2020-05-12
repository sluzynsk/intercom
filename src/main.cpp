#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const int LED=D8;
const int BTN=D3;

const char* ssid = "private_network_01";
const char* password = "creluchobr";
const char* mqtt_server = "10.0.1.52";

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R2,D1,D2,U8X8_PIN_NONE);

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
 u8g2.clearBuffer();
 u8g2.setFont(u8g2_font_profont17_mf);
 //u8g2.drawUTF8(0,6,topic);
 int row=0;
 String oneLine="";
 for (unsigned int i=0;i<length;i++) {
  yield();//ESP8266            
  if(u8g2.getUTF8Width((oneLine+(char)payload[i]).c_str())>128)
  {
    row+=17;
    oneLine.trim();
    u8g2.drawUTF8(0,row,oneLine.c_str());
    oneLine="";
  }
  oneLine+=(char)payload[i];
 }
 //Last line
 row+=17;
 oneLine.trim();
 u8g2.drawUTF8(0,row,oneLine.c_str());
 u8g2.sendBuffer();
 digitalWrite(LED,length?true:false);
}

void reconnect() {
 // Loop until we're reconnected
 while (!client.connected()) {
  if (client.connect("ESP8266 Client")) {
    // ... and subscribe to topic
    client.subscribe("Ekedal/OLED/1");
  } else {
    u8g2.clearBuffer();
    u8g2.drawUTF8(0,6,"failed, rc=");
    String ts=(String)client.state();
    u8g2.drawUTF8(0,12,ts.c_str());
    u8g2.sendBuffer();
    delay(5000);
  }
 }
}

void setup(void) {
  pinMode(LED,OUTPUT);
  pinMode(BTN,INPUT_PULLUP);
  
  u8g2.begin();
  u8g2.setFlipMode(true);
  u8g2.clearBuffer(); 
  u8g2.setFont(u8g2_font_4x6_mf);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop(void) {
  if (!client.connected()) {
    reconnect();
  }
  if(!digitalRead(BTN))
  {
    digitalWrite(LED,false);
  }
  client.loop();
}



