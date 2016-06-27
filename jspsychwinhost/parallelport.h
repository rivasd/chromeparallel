#pragma once
#include "stdafx.h"

namespace WinParallelPort {

	bool sendTrig(int trigger);
	
	std::vector<PORT_INFO_2> enumPorts();

	void setupAddress(std::string address);

	bool process(rapidjson::Document& message);
}