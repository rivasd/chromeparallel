#pragma once
#include "stdafx.h"

namespace WinParallelPort {

	bool sendTrig(int trigger, HANDLE port);
	
	std::vector<PORT_INFO_2W> enumPorts();
}