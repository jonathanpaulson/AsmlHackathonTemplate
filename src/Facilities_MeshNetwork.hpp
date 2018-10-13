//
//! \file
//
// Created by Sander van Woensel / Umut Uyumaz
// Copyright (c) 2018 ASML Netherlands B.V. All rights reserved.
//
//! Mesh Network wrapper class.

#ifndef __Facilities_MeshNetwork__
#define __Facilities_MeshNetwork__

#include "painlessMesh.h"
#include <functional>
#include <set>
#include <map>
#include <list>

uint32_t to_int(String& msg);
String to_string(uint32_t n);
bool has_prefix(String& msg, String& prefix);
String remove_prefix(String& msg, String& prefix);

namespace Facilities {

class MeshNetwork
{

public:
   typedef uint32_t NodeId;

   MeshNetwork();
   ~MeshNetwork() {};

   // Disallow copy-ing
  MeshNetwork(const MeshNetwork& other) = delete;
  MeshNetwork(MeshNetwork&& other) = delete;
  MeshNetwork& operator=(const MeshNetwork& other) = delete;

   void update();
   void initialize(const __FlashStringHelper *prefix, const __FlashStringHelper *password, Scheduler& taskScheduler);

   void sendBroadcast(String& message, bool include_self);
   NodeId getMyNodeId();
   std::list<NodeId> getNodeList();
   uint32_t getNodeTime();

   void onReceive(receivedCallback_t receivedCallback);
   void onChangedConnections();

private:
   static const uint16_t PORT;
   painlessMesh       m_mesh;
};

} // namespace Facilities

#endif // __Facilities_MeshNetwork__
