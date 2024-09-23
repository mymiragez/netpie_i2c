#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#define PCF8591_1 0x90>>1
const char* ssid     = "xxxxx";        // ชื่อ ssid
const char* password = "xxxxx";    // รหัสผ่าน wifi
const char* mqtt_server = "broker.netpie.io";
const int mqtt_port = 1883;
const char* mqtt_client = "ac30fb63a-a1da-43b3-96f8-22eb1b43deab";
const char* mqtt_username = "sQgFfycNETcdAr9RaMsjhQaKhcCWo8M5J";
const char* mqtt_password = "diZNUegEGqZ71HQs2abXR9ChvbLVo9R3P";

WiFiClient espClient;
PubSubClient client(espClient);
int analog_vr;
char msg[100];

void reconnect() 
{
  while (!client.connected()) 
  {
    Serial.print("Attempting MQTT connection…");
    if (client.connect(mqtt_client, mqtt_username, mqtt_password)) 
    {
      Serial.println("Connected");
      client.subscribe("@msg/analog_vr");
    }
    else 
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("Try again in 5 seconds...");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) 
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  
  String msg;
  for (int i = 0; i < length; i++) 
  {
    msg += (char)payload[i];
  }
  
  if (String(topic) == "@msg/analog_vr")
  {
    analog_vr = msg.toInt();
    Serial.print("analog_vr = ");
    Serial.println(analog_vr);
    project();  // เรียกใช้งานฟังก์ชัน project() เมื่อได้รับค่า analog_vr
  }
  
  Serial.println(msg);
}

void project()
{
  Wire.beginTransmission(PCF8591_1);  // ตั้งค่าที่อยู่ของ PCF8591 ผ่าน I2C
  Wire.write(B01000000);  // ส่งค่าควบคุมไปยัง PCF8591 โดยเริ่มต้นที่ช่อง 0
  Wire.write(analog_vr);  // ส่งค่า analog_vr ไปยัง PCF8591
  Wire.endTransmission();
}

void setup() 
{
  Wire.begin();
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);  
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); 
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

}

void loop() 
{
  if (!client.connected()) 
  {
    reconnect();
  }

  client.loop();
}
