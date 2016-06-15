#pragma once
#include <string>
#include "document.h"

namespace ChromeClient 
{
	typedef struct {
		std::string target;
		unsigned int trigger;
	} chromeMess;

	unsigned int readMessLength();

	chromeMess fetchMessage(unsigned int length);

	bool sendMessageToExt(const rapidjson::Document& jsonMess);
}