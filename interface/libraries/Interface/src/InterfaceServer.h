#ifndef RASPBERRY_INTERFACE_SERVER_H
#define RASPBERRY_INTERFACE_SERVER_H

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncDNSServer.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncUDP.h>

class InterfaceConfigValues
{
    public:
        static const char* wifi_ssid;
        static const char* wifi_password;
        static const char* server_hostname;
        static const int dns_port;
};

class InterfaceServer
{
    public:
        InterfaceServer();

    private:
        AsyncWebServer *server;
        AsyncDNSServer *dns_server;
        IPAddress my_ip;

        void handle_root(AsyncWebServerRequest *request);
};


#endif
