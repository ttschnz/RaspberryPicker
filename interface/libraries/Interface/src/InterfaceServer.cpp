#include "InterfaceServer.h"

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncDNSServer.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncUDP.h>

#include "static/index_html.h"

#include "InterfaceSlave.h"

const char* InterfaceConfigValues::wifi_ssid = "RaspberryPicker12";
const char* InterfaceConfigValues::wifi_password = "00000000";
const char* InterfaceConfigValues::server_hostname = "raspberrypicker";
const int InterfaceConfigValues::dns_port = 53;


InterfaceServer::InterfaceServer(InterfaceSlave *interface_slave){
    this->interface_slave = interface_slave;

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
    this->server->on("/api/state", HTTP_GET,
        [this](AsyncWebServerRequest *request){this->handle_state(request);}
    );
    this->server->on("/api/state", HTTP_POST,
        [this](AsyncWebServerRequest *request){this->set_state(request);}
    );

    this->server->begin();

    this->dns_server = new AsyncDNSServer();
    this->dns_server->start(
        InterfaceConfigValues::dns_port,
        InterfaceConfigValues::server_hostname,
        this->my_ip
    );

}

void InterfaceServer::handle_root(AsyncWebServerRequest *request){
    request->send(
        200,
        "text/html",
        index_html
    );
}

void InterfaceServer::handle_state_get(AsyncWebServerRequest *request){
    String current_state = this->interface_slave->get_state_json();
    request->send(
        200,
        "application/json",
        current_state
    );
}

void InterfaceServer::handle_state_post(AsyncWebServerRequest *request){
    if (!request->hasParam("key", true, false)){
        request->send(
            422,
            "application/json",
            R"rawliteral({"error":"missing key"})rawliteral"
        );
        return;
    }
    if (!request->hasParam("value", true, false)){
        request->send(
            422,
            "application/json",
            R"rawliteral({"error":"missing value"})rawliteral"
        );
        return;
    }
    String key = request->getParam("key", true, false);
    String value = request->getParam("value", true, false);


}
