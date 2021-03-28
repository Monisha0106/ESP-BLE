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