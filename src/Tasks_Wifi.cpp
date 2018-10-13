#include "Tasks_Wifi.hpp"
#include "Debug.hpp"

#include <functional>
#include <cassert>

AsyncWebServer server(80);

namespace Tasks {
  Wifi::Wifi(Facilities::MeshNetwork& mesh) :
   Task(TASK_SECOND * 5, TASK_FOREVER, std::bind(&Wifi::execute, this)),
   m_mesh(mesh)
{
   server.on("/", HTTP_GET, [&](AsyncWebServerRequest *request){
     String msg = "";
       if (request->hasArg("BROADCAST")){
         msg = request->arg("BROADCAST");
         m_mesh.sendBroadcast(msg, true);
       }
       request->send(200, "text/html", "<form>Text to Broadcast<br><input type='text' name='BROADCAST' value='" + msg + "'><br><br><input type='submit' value='Submit'></form>");
   });
   server.on("/debug", HTTP_GET, [&](AsyncWebServerRequest *request) {
     request->send(200, "text/html", "Frame Rate: 100hz\nMissed Frames: 0\nAnimation time: 10s\nNodes:" + to_string(m_nodes.size()) + "\nHistory:\n" + m_history);
   });

   server.begin();
}



void Wifi::execute()
{
   std::list<uint32_t> nodes_list = m_mesh.getNodeList();
   std::set<uint32_t> new_nodes(nodes_list.begin(), nodes_list.end());
   for(auto& node : new_nodes) {
     MY_DEBUG_PRINTF("Wifi node=%u\n", node);
   }
   new_nodes.insert(m_mesh.getMyNodeId());
   for(auto& node : new_nodes) {
     if(m_nodes.count(node) == 0) {
       m_history += ("Node " + to_string(node) + " added");
     }
   }
   for(auto& node : m_nodes) {
     if(new_nodes.count(node) == 0) {
       m_history += ("Node " + to_string(node) + " removed");
     }
   }
   m_nodes = new_nodes;

   /*String pattern = "PATTERN ";
   for(int r=0; r<8; r++) {
     for(int c=0; c<8; c++) {
       pattern += (-r <= c-4 && c-4 <= r ? "1" : "0");
     }
   }
   m_mesh.sendBroadcast(pattern, true);
	 */

/*
   String pattern = "PATTERN ";
   //assert(pattern.length() == 64);
   m_mesh.sendBroadcast(pattern, true);
   */
}

} // namespace Tasks
