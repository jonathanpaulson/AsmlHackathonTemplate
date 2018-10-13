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
#include <set>

namespace Tasks {

const int Example::SQ = 8;

const int Example::LEDMATRIX_WIDTH = 31;
const int Example::LEDMATRIX_HEIGHT = 7;
const int Example::LEDMATRIX_SEGMENTS = 4;
const int Example::LEDMATRIX_INTENSITY = 5;
const int Example::LEDMATRIX_CS_PIN = 16;
const unsigned long Example::POLL_DELAY_MS = 100;

//! Initializes the LED Matrix display.
Example::Example(Facilities::MeshNetwork& mesh) :
   Task(10 /*ms*/, TASK_FOREVER, std::bind(&Example::execute, this)),
   m_mesh(mesh),
   m_lmd(LEDMATRIX_SEGMENTS, LEDMATRIX_CS_PIN)
{
   m_lmd.setEnabled(true);
   m_lmd.setIntensity(LEDMATRIX_INTENSITY);

   m_mesh.onReceive(std::bind(&Example::receivedCb, this, std::placeholders::_1, std::placeholders::_2));
   //m_mesh.onChangedConnections(std::bind(&Example::changedConnections, this));
   m_pattern = vector<vector<vector<int>>>{};
   for(int n=1; n<=4; n++) {
     m_pattern.push_back(vector<vector<int>>{});
     for(int r=0; r<SQ*n; r++) {
       m_pattern[n-1].push_back(vector<int>{});
       for(int c=0; c<SQ*n; c++) {
         m_pattern[n-1][r].push_back(false);
       }
     }
   }
   for(int r=0; r<SQ; r++) {
     for(int c=0; c<SQ; c++) {
       m_pattern[0][r][c] = ((r+c)%2 == 1);
     }
   }
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
   std::set<uint32_t> nodes = m_mesh.getNodes();
   int n = nodes.size();
   int my_idx = 0;
   for(auto& node : nodes) {
     if(node == m_mesh.getMyNodeId()) {
       break;
     }
     my_idx++;
   }

   if(m_t % 100 == 0) {
     String pat;
     for(int r=0; r<SQ*n; r++) {
       for(int c=0; c<SQ*n; c++) {
         pat += (m_pattern[0][r/n][c/n] ? "1" : "0");
       }
     }
     MY_DEBUG_PRINTF("Execute me=%u n=%d my_idx=%d pattern=%s\n", m_mesh.getMyNodeId(), n, my_idx, pat.c_str());
   }

   m_lmd.clear();
   for(int r=0; r<SQ*n; r++) {
     for(int c=0; c<SQ*n; c++) {
       if(r/SQ == my_idx) {
         m_lmd.setPixel(fix_col(c), r%8, m_pattern[0][r/n][c/n]);
       }
     }
   }
   m_lmd.display();
}

void Example::receivedCb(Facilities::MeshNetwork::NodeId nodeId, String& msg)
{
   MY_DEBUG_PRINTF("Data received. me=%u sender=%u msg=%s\n", m_mesh.getMyNodeId(), nodeId, msg.c_str());

   String prefix("PATTERN ");
   if(has_prefix(msg, prefix)) { 
     String pattern = remove_prefix(msg, prefix);
     assert(pattern.length() == 8*8);
     unsigned pi = 0;
     for(int n=1; n>=1; n--) {
        for(int r=0; r<SQ*n; r++) {
          for(int c=0; c<SQ*n; c++) {
            assert(pi < pattern.length());
            m_pattern[n-1][r][c] = (pattern[pi]=='1');
            pi++;
          }
        }
     }
   }
   m_mesh.receivedCb(nodeId, msg);
}

} // namespace Tasks

