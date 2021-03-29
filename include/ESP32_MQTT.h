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
        bool is_mqtt_connected;
        const char *ssid;
        const char *password;
        const char *ap_ssid;
        const char *ap_password;
        const char *mqtt_broker;
        const char *sub_topic;
        const char *pub_topic;
        int mqtt_port;
        String mode;

        ESP32_MQTT(/* args */);

        void setup_wifi();
        void get_WiFi_SSID(char* SSID);
        void get_WiFi_Password(char* Password);
        void get_AP_SSID(char *SSID);
        void get_AP_Password(char *Password);
        void get_Static_IP(String IP);
        void get_WiFi_Credentials(char* SSID, char* Password, String Static_IP, String Mode);

        void get_Broker_Address(char *broker);
        void get_Broker_Port(int Port);
        void get_Sub_Topic(const char *Topic);
        void get_Pub_Topic(const char *Topic);
        static void callback(char* topic, byte* message, unsigned int length);
        void set_Server_And_Callback();
        bool is_MQTT_Connected();
        void MQTT_Reconnect();
        void Subscribe(const char *Topic);
        void Publish(const char *Topic, char *Payload);
        void loop();
        // ~ESP32_MQTT();
};




#endif