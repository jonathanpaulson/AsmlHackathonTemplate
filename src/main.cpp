#include <Arduino.h>

#include "Debug.hpp"
#include "painlessMesh.h"
#include "Facilities_MeshNetwork.hpp"
#include "Tasks_Example.hpp"
#include "Tasks_Talk.hpp"

// Translation unit local variables
namespace {

Scheduler                  taskScheduler;

Facilities::MeshNetwork    meshNetwork;
Tasks::Example  example(meshNetwork);
Tasks::Talk talk(meshNetwork);
}

//! Called once at board startup.
void setup()
{
   MY_DEBUG_BEGIN(115200);

   // Create MeshNetwork
   meshNetwork.initialize(F("TeamOrange"), F("Mesh-Secret-Password"), taskScheduler);

   // Create and add tasks.
   taskScheduler.addTask(example);
   taskScheduler.addTask(talk);
   example.enable();
   talk.enable();
   MY_DEBUG_PRINTLN(F("Setup completed"));
}

//! Called repeatedly after setup().
void loop()
{
   taskScheduler.execute();
   meshNetwork.update();
}
