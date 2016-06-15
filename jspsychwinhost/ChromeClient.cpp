#pragma once

#include "stdafx.h"
#include "ChromeClient.h"

using namespace rapidjson;
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

		Document jsonMessage;
		jsonMessage.Parse<0>(msg.c_str());

		chromeMess message;

		return message;
	}

	/**
	* takes a json-like Document (types from RapidJSON library) and sends the equivalent JSON string to the native app through stdout
	* Returns true on success, false on failure
	*/
	bool sendMessageToExt(const rapidjson::Document& jsonMess) {
		//magic code, comes straight from RapidJSON's github page
		StringBuffer strbuffer;
		Writer<StringBuffer> writer(strbuffer);
		jsonMess.Accept(writer);

		std::string message = strbuffer.GetString();
		uint32_t messLength = message.length();

		//now that we have our String and its length as a 4 byte unsigned int, we can start I/O
		//first with the 4-byte message length
		std::cout << char(messLength >> 0)
			<< char(messLength >> 8)
			<< char(messLength >> 16)
			<< char(messLength >> 24);

		//Now we send the actual string

		std::cout << message << std::flush;
		return true;
	}

}