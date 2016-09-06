// jspsychwinhost.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ChromeClient.h"
#include "parallelport.h"
using namespace rapidjson;

int main()
{
	//we need to reopen the stdin and stdout streams in binary mode to avoid low-level problems
	//s_setmode(_fileno(stdin), _O_BINARY);
	//_setmode(_fileno(stdout), _O_BINARY);

	//will hold the current message from the extension
	Document lastMess;
	//flag indicating the success or failure of the last write operation
	bool writeResult;
	

	//start by sending an "everything is okay" message

	Document welcomeMess;
	welcomeMess.SetObject();
	welcomeMess.AddMember("message", "connected", welcomeMess.GetAllocator());

	ChromeClient::sendMessageToExt(welcomeMess);

	/**********************************TEST CASES *******************************************/

	

	//the main, persistent, loop (listens for messages continuously)
	while (true) {

		//Kindly wait for a message from the Chrome extension
		lastMess = ChromeClient::receive();
		
		if (lastMess.HasMember("target")) {
			if (lastMess["target"] == "STOP") {
				//if we receive (or generate) the action:STOP, exit the program
				break;
			}
			else if (lastMess["target"] == "parallel") {
				WinParallelPort::process(lastMess);
			}
		}
		else {
			ChromeClient::sendErrorMess("1", "message has missing member \"target\"");
		}
		//for now, just echo back the message sent
		ChromeClient::sendMessageToExt(lastMess);
	}
    return 0;
}

