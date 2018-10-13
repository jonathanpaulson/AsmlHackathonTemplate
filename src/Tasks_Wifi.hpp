#ifndef __Tasks_Wifi__
#define __Tasks_Wifi__

#include "Facilities_MeshNetwork.hpp"
#include <painlessMesh.h>
#include <functional>
#include <set>

#ifdef ESP8266
#include "Hash.h"
#include <ESPAsyncTCP.h>
#else
#include <AsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>


// Forward declarations.
namespace Facilities { class MeshNetwork; }

namespace Tasks {

class Wifi : public Task
{
public:
   explicit Wifi(Facilities::MeshNetwork& mesh);
   ~Wifi() {};
   // Disallow copy-ing
   Wifi(const Wifi& other) = delete;
   Wifi(Wifi&& other) = delete;
   Wifi& operator=(const Wifi& other) = delete;

   void execute();

private:
   Facilities::MeshNetwork& m_mesh;
   std::set<uint32_t> m_nodes;
   String m_history;
};

} // namespace Tasks

#endif //  __Tasks_Wifi__
