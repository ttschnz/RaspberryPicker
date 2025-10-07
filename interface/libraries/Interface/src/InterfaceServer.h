#ifndef RASPBERRY_INTERFACE_SERVER_H
#define RASPBERRY_INTERFACE_SERVER_H

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncDNSServer.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncUDP.h>

#include "InterfaceSlave.h"

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
        InterfaceServer(InterfaceSlave *interface_slave);

    private:
        AsyncWebServer *server;
        AsyncDNSServer *dns_server;
        IPAddress my_ip;
        InterfaceSlave *interface_slave;
        void handle_root(AsyncWebServerRequest *request);
        void handle_state_get(AsyncWebServerRequest *request);
        void handle_state_post(AsyncWebServerRequest *request);
};


#endif
