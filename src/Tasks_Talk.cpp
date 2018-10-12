#include "Tasks_Talk.hpp"
#include "Debug.hpp"
#include "Facilities_MeshNetwork.hpp"

#include <functional>
#include <cassert>

namespace Tasks {
  Talk::Talk(Facilities::MeshNetwork& mesh) :
   Task(TASK_SECOND * 10, TASK_FOREVER, std::bind(&Talk::execute, this)),
   m_mesh(mesh)
{
}



void Talk::execute()
{
   String pattern = "PATTERN ";
   for(int r=0; r<8; r++) {
     for(int c=0; c<8; c++) {
       pattern += (r<=1 || r>=6 || c<=1 || c>=6 ? "1" : "0");
     }
   }
   //assert(pattern.length() == 64);
   m_mesh.sendBroadcast(pattern, true);
}

} // namespace Tasks
