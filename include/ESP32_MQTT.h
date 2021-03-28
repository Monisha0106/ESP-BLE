#ifndef ESP32_MQTT_H
#define ESP32_MQTT_H

#include<Arduino.h>
#include<WiFi.h>
#include<PubSubClient.h>
#define MAX_MESSAGE_SIZE 256


class ESP32_MQTT
{
    public:
        long last_message = 0;
        char message[MAX_MESSAGE_SIZE];
        bool is_wifi_connected;
        const char *ssid;
        const char *password;
        const char *ap_ssid;
        const char *ap_password;
        String mode;

        ESP32_MQTT(/* args */);

        void setup_wifi();
        void get_WiFi_SSID(char* SSID);
        void get_WiFi_Password(char* Password);
        void get_AP_SSID(char *SSID);
        void get_AP_Password(char *Password);
        void get_Static_IP(String IP);
        void get_WiFi_Credentials(char* SSID, char* Password, String Static_IP, String Mode);
        // ~ESP32_MQTT();
};




#endif