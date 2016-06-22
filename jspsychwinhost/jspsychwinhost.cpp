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
	BOOL writeResult;
	COMMTIMEOUTS comParams = { 0, 0, 0, 100, 5000 };

	//start by sending an "everything is okay" message

	Document welcomeMess;
	welcomeMess.SetObject();
	welcomeMess.AddMember("message", "connected", welcomeMess.GetAllocator());

	ChromeClient::sendMessageToExt(welcomeMess);

	/**********************************TEST CASES *******************************************/

	std::vector<PORT_INFO_2> ports = WinParallelPort::enumPorts();
	for (auto port_inf : ports) {
		std::wcout << port_inf.pPortName << std::endl;
	}

	OVERLAPPED ol = { 0 };
	HANDLE pport = CreateFile("LPT1", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,0 );
	if (pport == INVALID_HANDLE_VALUE) {
		std::wcout << "Oops! error opening parallel port" << GetLastError();
	}
	else {
		SetCommTimeouts(pport, &comParams);
		unsigned char trigger[1] = { 20 };
		std::cout << "Opened the handle to LPT1" << std::endl;
		writeResult = WriteFile(pport, &trigger, 1, NULL, NULL); // this should actually send somehing
		std::cout << writeResult << std::endl << GetLastError();
	}

	return 0;

	//the main, persistent, loop (listens for messages continuously)
	while (true) {

		//Kindly wait for a message from the Chrome extension
		lastMess = ChromeClient::receive();
		
		if (lastMess.HasMember("action")) {
			if (lastMess["action"] == "STOP") {
				//if we receive (or generate) the action:STOP, exit the program
				break;
			}
			else if (lastMess["action"] == "parallel") {
				//WinParallelPort::sendTrig(lastMess["payload"].GetInt());
			}
		}

		//for now, just echo back the message sent
		ChromeClient::sendMessageToExt(lastMess);
	}

    return 0;
}

