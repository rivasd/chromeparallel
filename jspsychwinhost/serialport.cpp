#include "stdafx.h"
#include "serialport.h"
#include "ChromeClient.h"

/**
* RAII encapsulator class representing a Serial Port (COM) connection
* copied and modified from https://github.com/manashmndl/SerialPort
*/
SerialPort::SerialPort(const char *portName)
{
	this->connected = false; 
	this->init(portName);
}

SerialPort::SerialPort(int portNmb) {

	this->connected = false;
	if (portNmb < 0) {
		ChromeClient::sendErrorMess("serial", "ERROR: requested port number must be positive integer");
	}
	else {
		std::string portName = std::to_string(portNmb);
		this->init(portName.c_str());
	}
}

SerialPort::~SerialPort()
{
	if (this->connected) {
		this->connected = false;
		CloseHandle(this->handler);
	}
}

/*
* Initializes the resource held by an instance of this class
*/
void SerialPort::init(const char* portNumber) {

	//we were already initialized, release the previously held port resource
	if (this->connected) {
		this->connected = false;
		CloseHandle(this->handler);
	}

	//now that were are sure that we don't hold any COM port handler, acquire the new one

	std::string portPath = "\\\\.\\COM";
	portPath.append(portNumber);

	this->handler = CreateFileA(static_cast<LPCSTR>(portPath.c_str()),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (this->handler == INVALID_HANDLE_VALUE) {
		if (GetLastError() == ERROR_FILE_NOT_FOUND) {
			ChromeClient::sendErrorMess("serial", "ERROR: Handle was not attached.Reason : %s not available");
		}
		else
		{
			ChromeClient::sendErrorMess("serial", "Unknown error, could not attach to port");
		}
	}
	else {
		DCB dcbSerialParameters = { 0 };

		if (!GetCommState(this->handler, &dcbSerialParameters)) {
			ChromeClient::sendErrorMess("serial", "failed to get current serial parameters");
		}
		else {
			dcbSerialParameters.BaudRate = CBR_9600;
			dcbSerialParameters.ByteSize = 8;
			dcbSerialParameters.StopBits = ONESTOPBIT;
			dcbSerialParameters.Parity = NOPARITY;
			dcbSerialParameters.fDtrControl = DTR_CONTROL_ENABLE;

			//Using strict timeouts and Blocking I/O for now
			//TODO: use multithreaded model to continously listen for data on the port without blocking main thread
			if (!SetCommState(handler, &dcbSerialParameters))
			{
				ChromeClient::sendErrorMess("serial", "ALERT: could not set Serial port parameters\n");
			}
			else {
				COMMTIMEOUTS timeouts = { 0 };

				if (!GetCommTimeouts(this->handler, &timeouts)) {
					ChromeClient::sendErrorMess("serial", "ALERT: could not get Serial port timeouts\n");
				}
				else {

					timeouts.ReadIntervalTimeout = 5;
					timeouts.ReadTotalTimeoutConstant = 10;
					timeouts.ReadTotalTimeoutMultiplier = 5;

					if (!SetCommTimeouts(this->handler, &timeouts)) {
						ChromeClient::sendErrorMess("serial", "ALERT: could not set Serial port timeouts\n");
					}
					else {
						this->connected = true;
						PurgeComm(this->handler, PURGE_RXCLEAR | PURGE_TXCLEAR);
						std::string name = "COM";
						name.append(portNumber);
						ChromeClient::sendStrToExt("{\"code\":\"serial\",\"result\":\"connected\", \"name\":\"" + name + "\"}");
					}
				}
			}
		}
	}
}


int SerialPort::readSerialPort(std::string& buffer, unsigned int buf_size)
{
	DWORD bytesRead;
	unsigned int toRead=1;

	ClearCommError(this->handler, &this->errors, &this->status);

	if (this->status.cbInQue > 0) {
		if (this->status.cbInQue > buf_size) {
			toRead = buf_size;
		}
		else toRead = this->status.cbInQue;
	}
	else {
		return 0;
	}

	char* inputBuffer = new char[toRead+1];

	if (ReadFile(this->handler, inputBuffer, toRead, &bytesRead, NULL)) {
		buffer.empty();
		inputBuffer[toRead] = '\0';
		buffer += inputBuffer;
		delete[] inputBuffer;
		return bytesRead;
	}
	delete[] inputBuffer;
	return 0;
}

bool SerialPort::writeSerialPort(const char *buffer, unsigned int buf_size)
{
	DWORD bytesSend;

	if (!WriteFile(this->handler, (void*)buffer, buf_size, &bytesSend, 0)) {
		ClearCommError(this->handler, &this->errors, &this->status);
		return false;
	}
	else return true;
}

bool SerialPort::isConnected()
{
	return this->connected;
}

/**
* simple namespace for Serial Port communication operations
*/
namespace WinSerialPort {

	std::unique_ptr<SerialPort> serialConnection = nullptr;

	bool process(rapidjson::Document& message) {
		if (!message.HasMember("action") || !message["action"].IsString() || !message.HasMember("payload")) {
			return false;
		}

		std::string action = message["action"].GetString();

		if (action == "connect") {

			if (message["payload"].IsString()) {
				const char* payload = message["payload"].GetString();
				serialConnection.reset(new SerialPort(payload));
			}
			else if (message["payload"].IsInt()) {
				serialConnection.reset(new SerialPort(message["payload"].GetInt()));
			}
			
			//check if our serialConnection is was actually connected successfully
			return serialConnection->isConnected();
		}
		else if (action == "disconnect") {
			serialConnection.reset();
			return ChromeClient::sendStrToExt("{\"code\":\"serial\", \"result\":\"disconnected\"}");
		}
		else if (action == "send") {
			if (serialConnection && serialConnection->isConnected()) {
				if (message["payload"].IsString()) {
					const char* signal = message["payload"].GetString();
					return serialConnection->writeSerialPort(signal, strlen(signal));
				}
				else {
					ChromeClient::sendErrorMess("serial", "Can only send String to serial port");
					return false;
				}

			}
			else {
				ChromeClient::sendErrorMess("serial", "connection was not initialized");
				return false;
			}
		}
		else if (action == "list") {
			std::vector<std::string> ports;
			WinSerialPort::listPorts(ports);

			rapidjson::Document mess;
			mess.SetObject();

			rapidjson::Value portList(rapidjson::kArrayType);

			for (int i = 0; i < ports.size(); i++) {
				portList.PushBack(rapidjson::Value(ports[i].c_str(), mess.GetAllocator()).Move(), mess.GetAllocator());
			}
			mess.AddMember("code", "serial", mess.GetAllocator());
			mess.AddMember("ports", portList, mess.GetAllocator());

			ChromeClient::sendMessageToExt(mess);


		}
		else if (action == "read") {
			if (!serialConnection) {
				ChromeClient::sendErrorMess("serial", "no connection to read on");
				return false;
			}
			if (message["payload"].IsInt()) {
				const unsigned int length = message["payload"].GetInt();
				std::string read;
				if (serialConnection->readSerialPort(read, length)) {
					ChromeClient::sendStrToExt("{\"code\":\"serial\", \"type\":\"read\",\"result\":\""+read+"\"}");
					return true;
				}
				else {
					ChromeClient::sendErrorMess("serial", "failed to read anything");
					return true;
				}
			}
			else {
				ChromeClient::sendErrorMess("serial", "must specify max amount of characters to read in 'payload' member");
				return false;
			}
		}
	}

	void listPorts(std::vector<std::string>& buffer) {

		buffer.clear();

		for (int i = 0; i <= 64; i++) {
			char port[7];
			COMMCONFIG CommConfig;
			DWORD size;

			snprintf(port, sizeof port, "COM%d", i);
			size = sizeof CommConfig;
			if (GetDefaultCommConfig(port, &CommConfig, &size) || size > sizeof CommConfig) {
				buffer.push_back(port);
			}
		}

	}
}