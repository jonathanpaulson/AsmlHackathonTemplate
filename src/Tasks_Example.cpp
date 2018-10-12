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

#include <LEDMatrixDriver.hpp>

#include <functional>
#include <cassert>

namespace Tasks {


const int Example::PAT_WIDTH = 8;
const int Example::PAT_HEIGHT = 8;

const int Example::LEDMATRIX_WIDTH = 31;
const int Example::LEDMATRIX_HEIGHT = 7;
const int Example::LEDMATRIX_SEGMENTS = 4;
const int Example::LEDMATRIX_INTENSITY = 5;
const int Example::LEDMATRIX_CS_PIN = 16;
const unsigned long Example::POLL_DELAY_MS = 100;

//! Initializes the LED Matrix display.
Example::Example(Facilities::MeshNetwork& mesh) :
   Task(TASK_SECOND * 1, TASK_FOREVER, std::bind(&Example::execute, this)),
   m_mesh(mesh),
   m_lmd(LEDMATRIX_SEGMENTS, LEDMATRIX_CS_PIN)
{
   m_lmd.setEnabled(true);
   m_lmd.setIntensity(LEDMATRIX_INTENSITY);

   m_mesh.onReceive(std::bind(&Example::receivedCb, this, std::placeholders::_1, std::placeholders::_2));
   //m_mesh.onChangedConnections(std::bind(&Example::changedConnections, this));
   m_pattern = vector<vector<int>>(PAT_HEIGHT, vector<int>(PAT_WIDTH, false));
   m_pattern[0][0] = true;
   m_pattern[0][1] = true;
}

// 24-31 16-23 8-15 0-7
int fix_col(int c) {
  if(0<=c && c<8) {
    return c+24;
  } else if(8<=c && c<16) {
    return 16+(c-8);
  } else if(16<=c && c<24) {
    return 8+(c-16);
  } else {
    return (c-24);
  }
}

//! Update display
void Example::execute()
{

   int n = m_nodes.size();
   int my_idx = 0;
   for(auto& node : m_nodes) {
     if(node == m_mesh.getMyNodeId()) {
       break;
     }
     my_idx++;
   }
   String pat;
   for(int r=0; r<PAT_HEIGHT; r++) {
     for(int c=0; c<PAT_WIDTH; c++) {
       pat += (m_pattern[r][c] ? "1" : "0");
     }
   }
   MY_DEBUG_PRINTF("Execute me=%u n=%d my_idx=%d pattern=%s\n", m_mesh.getMyNodeId(), n, my_idx, pat.c_str());

   m_lmd.clear();
   for(int r=0; r<PAT_HEIGHT*n; r++) {
     for(int c=0; c<PAT_WIDTH*n; c++) {
       if(r/8 == my_idx) {
	       m_lmd.setPixel(fix_col(c), r%8, m_pattern[r/n][c/n]);
       }
     }
   }
   m_lmd.display();
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
uint32_t to_int(String& msg) {
  uint32_t ans = 0;
  for(unsigned i=0; i<msg.length(); i++) {
    ans = ans*10 + (msg[i]-'0');
  }
  return ans;
}

void Example::receivedCb(Facilities::MeshNetwork::NodeId nodeId, String& msg)
{
   MY_DEBUG_PRINTF("Data received. me=%u sender=%u msg=%s\n", m_mesh.getMyNodeId(), nodeId, msg.c_str());

   String node_prefix("NODE ");
   if(has_prefix(msg, node_prefix)) {
     String nodeid = remove_prefix(msg, node_prefix);
     m_nodes.insert(to_int(nodeid));
   }

   String prefix("PATTERN ");
   if(has_prefix(msg, prefix)) { 
     String pattern = remove_prefix(msg, prefix);
     assert(pattern.length() == 64);
     for(int r=0; r<8; r++) {
       for(int c=0; c<8; c++) {
         m_pattern[r][c] = (pattern[r*8+c]=='1');
       }
     }
   }
}

} // namespace Tasks

