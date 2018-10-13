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

   std::list<uint32_t> nodes_list = m_mesh.getNodeList(); 
   std::set<uint32_t> nodes(nodes_list.begin(), nodes_list.end());
   nodes.insert(m_mesh.getMyNodeId());

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

void read_pattern(vector<vector<bool>>& pat, String& pattern) {
  assert(pat.size() > 0);
  assert(pattern.length() == pat.size() * pat[0].size());
  unsigned pi = 0;
  for(unsigned r=0; r<pat.size(); r++) {
    assert(pat[r].size() == pat[0].size());
    for(unsigned c=0; c<pat[r].size(); c++) {
      assert(pi < pattern.length());
      pat[r][c] = (pattern[pi] == '1');
      pi++;
    }
  }
}

void Example::receivedCb(Facilities::MeshNetwork::NodeId nodeId, String& msg)
{
   MY_DEBUG_PRINTF("Data received. me=%u sender=%u msg=%s\n", m_mesh.getMyNodeId(), nodeId, msg.c_str());

   String prefix_a("PATTERNA ");
   if(has_prefix(msg, prefix_a)) { 
     String pattern = remove_prefix(msg, prefix_a);
     read_pattern(m_pattern[0], pattern);
   }
   String prefix_b("PATTERNB ");
   if(has_prefix(msg, prefix_b)) {
     String pattern = remove_prefix(msg, prefix_b);
     read_pattern(m_pattern[1], pattern);
   }
}

} // namespace Tasks

