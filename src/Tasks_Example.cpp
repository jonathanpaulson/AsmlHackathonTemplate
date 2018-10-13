//
//! \file
//
// Created by Sander van Woensel / Umut Uyumaz
// Copyright (c) 2018 ASML Netherlands B.V. All rights reserved.
//
//! Example Task to output something to the LED Matrix.
#include "Tasks_Example.hpp"
#include "Debug.hpp"
#include "Facilities_MeshNetwork.hpp"

#include <functional>
#include <cassert>
#include <set>
#include <EEPROM.h>

namespace Tasks {

const int Example::SQ = 8;
const unsigned long Example::POLL_DELAY_MS = 100;

//! Initializes the LED Matrix display.
Example::Example(Facilities::MeshNetwork& mesh) :
   Task(10 /*ms*/, TASK_FOREVER, std::bind(&Example::execute, this)),
   m_mesh(mesh)
{
   m_mesh.onReceive(std::bind(&Example::receivedCb, this, std::placeholders::_1, std::placeholders::_2));
   //m_mesh.onChangedConnections(std::bind(&Example::changedConnections, this));
   EEPROM.begin(512);
}

//! Update display
void Example::execute()
{
   std::set<uint32_t> nodes = m_mesh.getNodes();
   int n = nodes.size();
   int my_idx = 0;
   for(auto& node : nodes) {
     if(node == m_mesh.getMyNodeId()) {
       break;
     }
     my_idx++;
   }
   
   MY_DEBUG_PRINTF("Execute me=%u n=%d my_idx=%d\n", m_mesh.getMyNodeId(), n, my_idx);
}

using pii = pair<int,int>;
pair<int, int> read_int(String& msg, int st) {
  int ans = 0;
  for(int i=st;; i++) {
    if(msg[i] == ' ') {
      return make_pair(ans, i+1);
    } else {
      assert('0' <= msg[i] && msg[i] <= '9');
      ans = ans*10 + (msg[i]-'0');
    }
  }
}

void Example::receivedCb(Facilities::MeshNetwork::NodeId nodeId, String& msg)
{
   MY_DEBUG_PRINTF("Data received. me=%u sender=%u msg=%s\n", m_mesh.getMyNodeId(), nodeId, msg.c_str());
   String prefix_a("PATTERN ");
    
    if(has_prefix(msg, prefix_a)) { 
      String pattern = remove_prefix(msg, prefix_a);
      pii which = read_int(pattern, 0);
      pii c = read_int(pattern, which.second);
      pii r = read_int(pattern, c.second);
      assert(pattern.length() - r.second + 1 == 64);
      unsigned pi = r.second;
      
      for (int row = 0; row < 8; row++) {
       uint8_t byte = 0;

       for (int i = 0; i < 8; i++) {
         uint8_t bit = pattern[8 * row + i] == '1' ? 1 : 0;
         byte = byte | bit << (7 - i);
       }
  
       EEPROM.write(128 * which.first + 32 * r.first + 8 * c.first + row, byte);
      }
      
    }
}

} // namespace Tasks

