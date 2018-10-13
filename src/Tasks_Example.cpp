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
   Task(50 /*ms*/, TASK_FOREVER, std::bind(&Example::execute, this)),
   m_mesh(mesh),
   m_lmd(LEDMATRIX_SEGMENTS, LEDMATRIX_CS_PIN)
{
   m_lmd.setEnabled(true);
   m_lmd.setIntensity(LEDMATRIX_INTENSITY);

   m_mesh.onReceive(std::bind(&Example::receivedCb, this, std::placeholders::_1, std::placeholders::_2));
   //m_mesh.onChangedConnections(std::bind(&Example::changedConnections, this));
   m_pattern = vector<vector<vector<bool>>>{};
   for(int t=0; t<2; t++) { 
     m_pattern.push_back(vector<vector<bool>>{});
     for(int r=0; r<SQ; r++) {
       m_pattern[t].push_back(vector<bool>{});
       for(int c=0; c<SQ; c++) {
         m_pattern[t][r].push_back((r+c)%2 == t);
       }
     }
   }
 
   m_intensities = vector<int>{};
   for(int i=1; i<5; i++) {
     m_intensities.push_back(i);
   }
   for(int i=0; i<5; i++) {
     m_intensities.push_back(5);
   }
   for(int i=4; i>=1; i--) {
     m_intensities.push_back(i);
   }
   for(int i=0; i<5; i++) {
     m_intensities.push_back(0);
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

   int i_sz = static_cast<int>(m_intensities.size());

   uint32_t now = m_mesh.getNodeTime();
   int frame = (now / (1000*10)) % (2*i_sz);
   assert(frame < 2*i_sz);
   int intensity = m_intensities[frame % i_sz];


   m_lmd.clear();
   m_lmd.setIntensity(intensity);
   int which_pattern = (frame / i_sz);

   for(int r=0; r<SQ*n; r++) {
     for(int c=0; c<SQ*n; c++) {
       if(r/SQ == my_idx) {
         m_lmd.setPixel(fix_col(c+(16-4*n)), r%8, intensity == 0 ? false : m_pattern[which_pattern][r/n][c/n]);
       }
     }
   }
   m_lmd.display();

   m_t++;
   if(m_t % 100 == 0) {
     m_t = 0;     
     String pat;
     for(int r=0; r<SQ*n; r++) {
       for(int c=0; c<SQ*n; c++) {
         pat += (m_pattern[which_pattern][r/n][c/n] ? "1" : "0");
       }
     }
     MY_DEBUG_PRINTF("Execute now=%u me=%u n=%d my_idx=%d pattern=%s\n", now, m_mesh.getMyNodeId(), n, my_idx, pat.c_str());
   }
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

   m_mesh.receivedCb(nodeId, msg);
}

} // namespace Tasks

