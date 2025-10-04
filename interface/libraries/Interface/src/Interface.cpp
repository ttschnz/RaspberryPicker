#include "Interface.h"

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncDNSServer.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncUDP.h>


const char* InterfaceConfigValues::wifi_ssid = "RaspberryPicker12";
const char* InterfaceConfigValues::wifi_password = "00000000";
const char* InterfaceConfigValues::server_hostname = "raspberrypicker";
const int InterfaceConfigValues::dns_port = 53;


Interface::Interface(InterfacePinout *interface_pinout){
    WiFi.hostname("esp8266device");
    WiFi.softAP(
        InterfaceConfigValues::wifi_ssid,
        InterfaceConfigValues::wifi_password
    );

    this->my_ip = WiFi.softAPIP();

    this->server = new AsyncWebServer(80);
    this->server->onNotFound([](AsyncWebServerRequest *request){
      request->redirect("/");
    });
    this->server->on("/", HTTP_GET,
        [this](AsyncWebServerRequest *request){this->handle_root(request);}
    );
    this->server->begin();

    this->dns_server = new AsyncDNSServer();
    this->dns_server->start(
        InterfaceConfigValues::dns_port,
        InterfaceConfigValues::server_hostname,
        this->my_ip
    );

}

void Interface::handle_root(AsyncWebServerRequest *request){
    request->send(
        200,
        "text/html",
        "<html><body><h1>Welcome</h1></body></html>"
    );
}
