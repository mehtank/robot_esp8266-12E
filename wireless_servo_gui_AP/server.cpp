#include <Arduino.h>
#include "debug.h"

#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>

#include <ESP8266mDNS.h>

#define STA_MAXTRIES 10

ESP8266WebServer httpServer = ESP8266WebServer(80);
WebSocketsServer wsServer = WebSocketsServer(81);

//
// Setup //
//

void setupSTA(char* ssid, char* password) {
    debug("Connecting to STA");
    WiFi.begin(ssid, password);

    int tries = 0;
    while (WiFi.status() != WL_CONNECTED) {
        if (tries++ > STA_MAXTRIES) {
            debug("  giving up.");
            return;
        }
        delay(500);
        debug("  ... waiting");
    }

    IPAddress myIP = WiFi.localIP();
    debug("STA IP address: ");
    debug(myIP);
}

void setupAP(char* ssid, char* password) {
    WiFi.softAP(ssid, password);

    IPAddress myIP = WiFi.softAPIP();
    debug("AP IP address: ");
    debug(myIP);
}

inline void registerPage(const char* url, const char* type, String content) {
    server.on(url,  []() { server.send(200, type, content); }); 
}

void setupHTTP() {
    httpServer.begin();
}

void setupWS(ws_callback_t callback) {
    // start webSocket server
    webSocket.begin();
    webSocket.onEvent(callback);
}

void setupMDNS(char* name) {
    if(MDNS.begin(name)) {
        // Add services to mDNS
        MDNS.addService("http", "tcp", 80);
        MDNS.addService("ws", "tcp", 81);
        debug("mDNS responder started");
    } else {
        debug("mDNS failed\n");
    }
}

void httpLoop() {
	httpServer.handleClient();                                                       
}

void wsLoop() {
	wsServer.loop();
}
