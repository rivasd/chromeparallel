#include "stdafx.h"
#include "parallelport.h"


namespace WinParallelPort {

	PORT_INFO_1 ports[3];
	short portAdress;

	bool sendTrig(int trigger, HANDLE port ) {
		return false;
	}
	
	std::vector<PORT_INFO_2> enumPorts() {
		
		DWORD bufferSize;
		DWORD nbrOfPorts;
		PORT_INFO_2* portInfos;
		//find out how large the buffer that stores the PORT_INFO objects should be
		EnumPorts(NULL, 2, NULL, 0, &bufferSize, &nbrOfPorts);
		portInfos = (PORT_INFO_2*)malloc(bufferSize); // then allocate enough memory for it
		BOOL success = EnumPorts(NULL, 2, (LPBYTE)portInfos, bufferSize, &bufferSize, &nbrOfPorts); //portInfos is now a pointer to contiguous PORT_INFO_2 structs

		//transfer the structs to the more friendly vector
		std::vector<PORT_INFO_2> ports; 
		for (int i = 0; i < nbrOfPorts; i++) {
			if (portInfos[i].pPortName != nullptr) {
				ports.push_back(portInfos[i]);
			}
		}
		return ports;
	}

	void setupAddress(std::string address) {
		portAdress = (short)std::stoi(address, nullptr, 16);
	}

	bool process(rapidjson::Document message) {
		if (!message.HasMember("action") || !message["action"].IsString() || !message.HasMember("payload")) {
			return false;
		}
		std::string action = message["action"].GetString();

		if (action == "setup") {
			if (message["payload"].IsString()) {
				try
				{
					setupAddress(message["payload"].GetString());
				}
				catch (const std::exception& ex)
				{

				}
			}
			else {
				return false;
			}
		}
		
	}
}