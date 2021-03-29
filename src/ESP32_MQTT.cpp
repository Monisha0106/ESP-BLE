#include "ESP32_MQTT.h"

WiFiClient esp32client;
PubSubClient client(esp32client);
IPAddress staticIP;

ESP32_MQTT::ESP32_MQTT(/* args */)
{
    this->mode = "STA";
}

void ESP32_MQTT::setup_wifi() 
{
    if(this->mode == "STA")
    {
        delay(10);
        // We start by connecting to a WiFi network
        Serial.println();
        Serial.print("Connecting to ");
        Serial.println(ssid);

        WiFi.begin(ssid, password);

        while (WiFi.status() != WL_CONNECTED) 
        {
            delay(500);
            Serial.print(".");
        }

        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
    }

    else if(this->mode == "AP")
    {
        Serial.println("Setting Access Point...");
        WiFi.softAP(ap_ssid, ap_password);
    }
}

void ESP32_MQTT::get_WiFi_SSID(char* SSID)
{
    this->ssid = SSID;
}

void ESP32_MQTT::get_WiFi_Password(char* Password)
{
    this->password = Password;
}

void ESP32_MQTT::get_AP_SSID(char* SSID)
{
    this->ap_ssid = SSID;
}

void ESP32_MQTT::get_AP_Password(char* Password)
{
    this->ap_password = Password;
}

void ESP32_MQTT::get_Static_IP(String IP)
{
    int current_point_pos = 0;
    int prev_point_pos = 0;
    int octates[4] = {0, 0, 0, 0};
    int count = 0;
    for(int i = 0; i < IP.length(); i++)
    {
        if(IP[i] == '.')
        {
            current_point_pos = i;
            octates[count] = IP.substring(prev_point_pos, current_point_pos).toInt();
            // Serial.println(IP.substring(prev_point_pos, current_point_pos));
            prev_point_pos = current_point_pos + 1;
            Serial.println(octates[count]);
            count++;
        }
    }
    octates[count] = IP.substring(prev_point_pos).toInt();
    Serial.println(octates[count]);
    staticIP = IPAddress(octates[0], octates[1], octates[2], octates[3]);
}

void ESP32_MQTT::get_WiFi_Credentials(char* SSID, char* Password, String Static_IP, String Mode)
{
    this->mode = Mode;
    if(Mode == "AP")
    {
        WiFi.mode(WIFI_AP);
        this->ap_ssid = SSID;
        this->ap_password = Password;
    }
    else if(Mode == "STA")
    {
        WiFi.mode(WIFI_STA);
        this->get_WiFi_SSID(SSID);
        this->get_WiFi_Password(Password);
        this->get_Static_IP(Static_IP);
    }
    
}


void ESP32_MQTT::get_Broker_Address(char *broker)
{
    this->mqtt_broker = broker;
}

void ESP32_MQTT::get_Broker_Port(int Port)
{
    this->mqtt_port = Port;
}

void ESP32_MQTT::get_Sub_Topic(const char *Topic)
{
    this->sub_topic = Topic;
}
void ESP32_MQTT::get_Pub_Topic(const char *Topic)
{
    this->pub_topic = Topic;
}

void Callback(char* topic, byte* message, unsigned int length) 
{
    Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(". Message: ");
    String messageTemp;
    
    for (int i = 0; i < length; i++) 
    {
        Serial.print((char)message[i]);
        messageTemp += (char)message[i];
    }
}

void ESP32_MQTT::set_Server_And_Callback()
{
    client.setServer(this->mqtt_broker, this->mqtt_port);
    client.setCallback(Callback);
}

bool ESP32_MQTT::is_MQTT_Connected()
{
    return client.connected();
}

void ESP32_MQTT::MQTT_Reconnect()
{
      // Loop until we're reconnected
    while (!client.connected()) 
    {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect("DMA_ESP32Client")) 
        {
            Serial.println("connected");
            // Subscribe
            client.subscribe(this->sub_topic);
        } 
        
        else 
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
  }
}

void ESP32_MQTT::Subscribe(const char *Topic)
{
    client.subscribe(Topic);
}

void ESP32_MQTT::Publish(const char *Topic, char *Payload)
{
    client.publish(Topic, Payload);
}

void ESP32_MQTT::loop()
{
    client.loop();
}