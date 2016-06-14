#pragma once

#include "stdafx.h"
#include "ChromeClient.h"

using json = nlohmann::json;
using std::string;

/*
* Function encapsulation for interfacing the Chrome native messaging functions
*
*/

namespace ChromeClient 
{
	//Type to hold a message from our jspsych Chrome extension
	

	unsigned int readMessLength() {
		//allocate some space to receive our message length
		unsigned int messLength;
		//read 4 bytes from stdin
		for (int i = 0; i < 4; i++)
		{
			unsigned int read_char = getchar();
			messLength = messLength | (read_char << i * 8);
		}

		return messLength;
	}

	chromeMess fetchMessage(unsigned int length) {
		string msg = "";
		for (int i = 0; i < length; i++)
		{
			msg += getchar();
		}
		auto jsonmess = json::parse(msg);

		chromeMess message;
		message.trigger = jsonmess["payload"];

		return message;
	}

}