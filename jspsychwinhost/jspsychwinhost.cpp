// jspsychwinhost.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ChromeClient.h"
using namespace rapidjson;

int main()
{
	//we need to reopen the stdin and stdout streams in binary mode to avoid low-level problems
	_setmode(_fileno(stdin), _O_BINARY);
	_setmode(_fileno(stdout), _O_BINARY);

	//allocate only once a variable to hold the most recent message length, and the most recent message itself
	uint32_t lastMessLength; //uint32_t variable type is virtually assured to actually be 4 bytes long
	ChromeClient::chromeMess lastMessage; //damn, dat json type feels like javascript <3

	//start by sending an "everything is okay" message

	Document welcomeMess;
	welcomeMess.SetObject();
	welcomeMess.AddMember("message", "connected", welcomeMess.GetAllocator());

	ChromeClient::sendMessageToExt(welcomeMess);


	//the main, persistent, loop (listens for messages continuously)
	while (true) {
		
		


		//try to read the 4-byte message length from the chrome port
		//EXECUTION WILL BLOCK HERE UNTIL SOMETHING IS RECEIVED
		lastMessLength = ChromeClient::readMessLength();



	}

    return 0;
}

