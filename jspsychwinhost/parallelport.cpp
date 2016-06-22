#include "stdafx.h"
#include "parallelport.h"


namespace WinParallelPort {

	PORT_INFO_1 ports[3];

	bool sendTrig(int trigger, HANDLE port ) {
		return false;
	}
	
	std::vector<PORT_INFO_2W> enumPorts() {
		
		DWORD bufferSize;
		DWORD nbrOfPorts;
		PORT_INFO_2W* portInfos;
		//find out how large the buffer that stores the PORT_INFO objects should be
		EnumPorts(NULL, 2, NULL, 0, &bufferSize, &nbrOfPorts);
		portInfos = (PORT_INFO_2W*)malloc(bufferSize); // then allocate enough memory for it
		BOOL success = EnumPorts(NULL, 2, (LPBYTE)portInfos, bufferSize, &bufferSize, &nbrOfPorts); //portInfos is now a pointer to contiguous PORT_INFO_2 structs

		//transfer the structs to the more friendly vector
		std::vector<PORT_INFO_2W> ports; 
		for (int i = 0; i < nbrOfPorts; i++) {
			if (portInfos[i].pPortName != nullptr) {
				ports.push_back(portInfos[i]);
			}
		}
		return ports;
	}
}