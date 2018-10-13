//
//! \file
//
// Created by Sander van Woensel / Umut Uyumaz
// Copyright (c) 2018 ASML Netherlands B.V. All rights reserved.
//
//! Example Task

#ifndef __Tasks_Example__
#define __Tasks_Example__

#include "Facilities_MeshNetwork.hpp"
#include "Debug.hpp"

#include <painlessMesh.h>

#include <functional>
#include <vector>
#include <set>

// Forward declarations.
namespace Facilities { class MeshNetwork; }


namespace Tasks {

class Example : public Task
{
public:
   explicit Example(Facilities::MeshNetwork& mesh);
   ~Example() {};

   // Disallow copy-ing
	Example(const Example& other) = delete;
	Example(Example&& other) = delete;
	Example& operator=(const Example& other) = delete;

   void execute();

private:
   static const int SQ;
   static const unsigned long POLL_DELAY_MS;

   Facilities::MeshNetwork& m_mesh;

   vector<vector<vector<int>>> m_pattern;

   void receivedCb(Facilities::MeshNetwork::NodeId nodeId, String& msg);
   void changedConnections();
};

} // namespace Tasks

#endif //  __Tasks_Example__
