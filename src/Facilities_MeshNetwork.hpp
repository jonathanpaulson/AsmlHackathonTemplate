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
   set<NodeId> getNodes();

   void onReceive(receivedCallback_t receivedCallback);
   void onChangedConnections();



private:
   static const uint16_t PORT;
   set<uint32_t> m_nodes;
   String m_history;

   painlessMesh       m_mesh;

   void receivedCb(NodeId transmitterNodeId, String& msg);


};

} // namespace Facilities

#endif // __Facilities_MeshNetwork__
