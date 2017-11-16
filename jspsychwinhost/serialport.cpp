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

			if (!SetCommState(handler, &dcbSerialParameters))
			{
				ChromeClient::sendErrorMess("serial", "ALERT: could not set Serial port parameters\n");
			}
			else {
				this->connected = true;
				PurgeComm(this->handler, PURGE_RXCLEAR | PURGE_TXCLEAR);
				ChromeClient::sendStrToExt("{\"code\":\"serial\",\"message\":\"OK\"}");
			}
		}
	}
}


int SerialPort::readSerialPort(char *buffer, unsigned int buf_size)
{
	DWORD bytesRead;
	unsigned int toRead;

	ClearCommError(this->handler, &this->errors, &this->status);

	if (this->status.cbInQue > 0) {
		if (this->status.cbInQue > buf_size) {
			toRead = buf_size;
		}
		else toRead = this->status.cbInQue;
	}

	if (ReadFile(this->handler, buffer, toRead, &bytesRead, NULL)) return bytesRead;

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
		}
		else if (action == "send") {
			if (serialConnection != nullptr && serialConnection->isConnected()) {
				if (message["payload"].IsString()) {
					const char* signal = message["payload"].GetString();
					serialConnection->writeSerialPort(signal, strlen(signal));
				}
				else {
					ChromeClient::sendErrorMess("serial", "Can only send String to serial port");
				}
				
			}
			else {
				ChromeClient::sendErrorMess("serial", "connection was not initialized");
			}
		}
	}
}