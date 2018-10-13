//
//! \file
//
// Created by Sander van Woensel / Umut Uyumaz
// Copyright (c) 2018 ASML Netherlands B.V. All rights reserved.
//
//! Mesh Network wrapper class to provide a container to add specific
//! mesh network behaviour.

#include "Facilities_MeshNetwork.hpp"

#include "Debug.hpp"
#include "painlessMesh.h"
#include <cassert>

#ifdef ESP8266
#include "Hash.h"
#include <ESPAsyncTCP.h>
#else
#include <AsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include <EEPROM.h>

uint32_t to_int(String& msg) {
  uint32_t ans = 0;
  for(unsigned i=0; i<msg.length(); i++) {
    ans = ans*10 + (msg[i]-'0');
  }
  return ans;
}
String to_string(uint32_t n) {
  if(n == 0) {
    return "";
  }
  String ans = "";
  while(n > 0) {
    ans = ('0' + (n%10)) + ans;
  }
  return ans;
}
bool has_prefix(String& msg, String& prefix) {
  if(msg.length() < prefix.length()) { return false; }
  for(unsigned i=0; i<prefix.length(); i++) {
    if(msg[i] != prefix[i]) {
      return false;
    }
  }
  return true;
}
String remove_prefix(String& msg, String& prefix) {
  assert(has_prefix(msg, prefix));
  String ans = "";
  for(unsigned i=prefix.length(); i<msg.length(); i++) {
    ans += msg[i];
  }
  return ans;
}



namespace Facilities {

const uint16_t MeshNetwork::PORT = 5555;

//! Construct only.
//! \note Does not construct and initialize in one go to be able to initialize after serial debug port has been opened.
MeshNetwork::MeshNetwork()
{
   //m_mesh.onNewConnection(...);
   //m_mesh.onChangedConnections(...);
   //m_mesh.onNodeTimeAdjusted(...);
}

// Initialize mesh network.
void MeshNetwork::initialize(const __FlashStringHelper *prefix, const __FlashStringHelper *password, Scheduler& taskScheduler)
{
   // Set debug messages before init() so that you can see startup messages.
   m_mesh.setDebugMsgTypes( ERROR | STARTUP );  // To enable all: ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE
   m_mesh.init( prefix, password, &taskScheduler, MeshNetwork::PORT );
   EEPROM.begin(512);
   
   server.on("/pattern", HTTP_GET, [&](AsyncWebServerRequest *request){
    int x = atoi(request->arg("x").c_str());
    int y = atoi(request->arg("y").c_str());
    int p = atoi(request->arg("p").c_str());
    String binaryString = request->arg("b");

    for (int row = 0; row < 8; row++) {
     uint8_t byte = 0;

     for (int i = 0; i < 8; i++) {
       uint8_t bit = binaryString[8 * row + i] == '1' ? 1 : 0;
       byte = byte | bit << (7 - i);
     }
  
     EEPROM.write(128 * p + 32 * y + 8 * x + row, byte);
    }
    
    EEPROM.commit();
    String msg = "PATTERN " + request->arg("p") + " " + request->arg("x") + " " + request->arg("y") + " " + binaryString;
    request->send(200, "text/plain", "Yeah, whatever.");
  });

   server.on("/debug", HTTP_GET, [&](AsyncWebServerRequest *request){
     request->send(200, "text/plain", "Frame Rate: 100hz\nMissed Frames: 0\nAnimation time: 10s\nNodes: Many\n");
   });

   //Amount of nodes actively functioning.
   //History record of removed and added nodes since the system became active (i.e. running with at least one node).
   
   server.begin();
}

//! Update mesh; forward call to painlessMesh.
void MeshNetwork::update()
{
   m_mesh.update();
}

void MeshNetwork::sendBroadcast(String &message, bool include_self)
{
   MY_DEBUG_PRINT("Broadcasting message: "); MY_DEBUG_PRINTLN(message);
   m_mesh.sendBroadcast(message, include_self);
}

MeshNetwork::NodeId MeshNetwork::getMyNodeId()
{
  return m_mesh.getNodeId();
}
uint32_t MeshNetwork::getNodeTime() {
  return m_mesh.getNodeTime();
}
std::list<uint32_t> MeshNetwork::getNodeList() {
  return m_mesh.getNodeList();
}

void MeshNetwork::onReceive(receivedCallback_t receivedCallback)
{
   m_mesh.onReceive(receivedCallback);
}

} // namespace Facilities
