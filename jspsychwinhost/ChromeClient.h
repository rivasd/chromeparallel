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

	rapidjson::Document fetchMessage(unsigned int length);

	rapidjson::Document receive();

	bool sendMessageToExt(const rapidjson::Document& jsonMess);

	bool sendStrToExt(std::string message);

	bool sendErrorMess(std::string code, std::string error);
}