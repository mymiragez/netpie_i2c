#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid     = "xxxx";        // ชื่อ ssid
const char* password = "xxxxx";    // รหัสผ่าน wifi
const char* mqtt_server = "broker.netpie.io";
const int mqtt_port = 1883;
const char* mqtt_client = "4f6cb0a2-1e28-4af8-a1da-8c389dd98826a";
const char* mqtt_username = "KHiqFiaKKiKtpzVjU9NQTb1aHfjjyXKas";
const char* mqtt_password = "Va3vbcwDSMa5T823zycCPHC8JL41A7Etd";

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
  }
  
  Serial.println(msg);
}

void setup() 
{
  Serial.begin(115200);
  pinMode(A0, INPUT); // ตั้ง A0 เป็น input
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
  
  int DataSlider = analogRead(A0); // อ่านค่า analog input A0
  analog_vr = map(DataSlider, 0, 1023, 0, 255); // แปลงค่า
  String data = String(analog_vr);
  data.toCharArray(msg, (data.length() + 1));
  client.publish("@msg/analog_vr", msg);
  Serial.print("analog_vr = ");
  Serial.println(msg);
  delay(500);
  client.loop();
}
