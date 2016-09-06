#pragma once

#include "stdafx.h"
#include "ChromeClient.h"

using namespace rapidjson;
using std::string;

/*
* Function encapsulation for interfacing the Chrome native messaging API
* stdin and stdout streams are assumend to already be in binary mode (if not, trouble I guess??)
*
*/

namespace ChromeClient 
{
	
	unsigned int readMessLength() {
		//allocate some space to receive our message length
		unsigned int messLength = 0;
		//read 4 bytes from stdin
		for (int i = 0; i < 4; i++)
		{
			unsigned int read_char = getchar();
			if (read_char == 0xFFFFFFFF) {
				//the stdin stream is closed/meaningless, so return a zero-message length
				return 0;
			}
			messLength = messLength | (read_char << i * 8);
		}
		return messLength;
	}

	Document fetchMessage(unsigned int length) {
		string msg = "";
		if (length == 0) {
			msg = "{\"target\":\"STOP\"}";
		}
		else {
			for (unsigned int i = 0; i < length; i++)
			{
				msg += getchar();
			}
		}
		
		Document jsonMessage;
		jsonMessage.Parse<0>(msg.c_str());
		return jsonMessage;
	}

	//main listening function, waits for and returns a message from the Chrome extension, as a RapidJSON Document
	Document receive() {
		return fetchMessage(readMessLength());
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

		const char* c_str = strbuffer.GetString();
		if (c_str != NULL) {
			return sendStrToExt(std::string(c_str));
		}
		else {
			return false;
		}
		
	}

	bool sendStrToExt(std::string message) {
		uint32_t messLength = message.length();
		std::cout << char(messLength >> 0)
			<< char(messLength >> 8)
			<< char(messLength >> 16)
			<< char(messLength >> 24);

		//Now we send the actual string

		std::cout << message << std::flush;
		return true;
	}

	//TODO: need to check that both arguments are strings without JSON characters like "}", "{", 
	bool sendErrorMess(std::string code, std::string error) {
		std::string errMess = "{";
		errMess = errMess + "\"code\":\"" + code + "\",";
		errMess = errMess + "\"error\":\"" + error + "\"}";
		return sendStrToExt(errMess);
	}

}