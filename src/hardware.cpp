#include "hardware.hpp"
#include <fstream>
#include <vector>

namespace { struct Device { std::string type,name,capabilities; bool connected=false; bool timeout=false; std::string telemetry() const { return timeout ? "timeout after 500 ms; disconnected safely; retry permitted" : "SIMULATED telemetry: voltage=3.30 V, status=healthy"; } }; }
bool export_hardware_demo(const std::string& directory,std::string& report) {
 std::vector<Device> devices={{"Microcontroller","STM32-style development board","USB serial, GPIO, ADC, SWD"},{"FPGA","Artix-7-style evaluation board","USB JTAG, bitstream configuration, GPIO"},{"JTAG debugger","CMSIS-DAP-style adapter","SWD/JTAG debug transport only"}}; devices[1].timeout=true;
 std::ofstream log(directory+"/hardware_demo.txt"); if(!log){report="Cannot create hardware demo log.";return false;} log<<"SIMULATION ONLY - no physical devices are controlled.\nDISCOVERY\n"; for(auto& d:devices){log<<d.type<<": "<<d.name<<" | capabilities: "<<d.capabilities<<"\nconnect: success\ntelemetry: "<<d.telemetry()<<"\ndisconnect: success\n";} report="Simulated discovery, capability reports, connect/disconnect, telemetry and recoverable FPGA timeout exported.";return true;
}
