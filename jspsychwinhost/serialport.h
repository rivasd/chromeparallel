#pragma once

#define ARDUINO_WAIT_TIME 2000
#define MAX_DATA_LENGTH 255

class SerialPort
{
private:
	HANDLE handler;
	bool connected;
	COMSTAT status;
	DWORD errors;
	void init(const char* portNumber);
public:
	SerialPort(const char *portName);
	SerialPort(int portNmb);
	~SerialPort();

	int readSerialPort(char *buffer, unsigned int buf_size);
	bool writeSerialPort(const char *buffer, unsigned int buf_size);
	bool isConnected();
};

namespace WinSerialPort {

	bool process(rapidjson::Document& message);
}