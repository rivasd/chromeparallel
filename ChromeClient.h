#pragma once
#include <string>

namespace ChromeClient 
{
	typedef struct {
		std::string target;
		unsigned int trigger;
	} chromeMess;

	unsigned int readMessLength();

	chromeMess fetchMessage(unsigned int length);
}