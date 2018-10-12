#ifndef __Tasks_Talk__
#define __Tasks_Talk__

#include <painlessMesh.h>
#include <functional>

// Forward declarations.
namespace Facilities { class MeshNetwork; }

namespace Tasks {

class Talk : public Task
{
public:
   explicit Talk(Facilities::MeshNetwork& mesh);
   ~Talk() {};
   // Disallow copy-ing
   Talk(const Talk& other) = delete;
   Talk(Talk&& other) = delete;
   Talk& operator=(const Talk& other) = delete;

   void execute();

private:
   Facilities::MeshNetwork& m_mesh;
};

} // namespace Tasks

#endif //  __Tasks_Talk__
