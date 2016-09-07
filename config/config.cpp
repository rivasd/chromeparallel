// config.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"

using namespace std;


int main(int numberOfParameters, char *parametersArray[])
{
	const string HOOK = "@@path";
	string currentPath;
	string wholeFile;
	string currentLine;
	const char *wholeFileChar;
	size_t fileLength;

	currentPath = parametersArray[0];

	ifstream jsonFile;
	jsonFile.open("com.cogcommtl.hardware.host.json");
	ofstream newJsonFile;

	while (!jsonFile.eof()) {
		getline(jsonFile, currentLine);
		if (currentLine == HOOK) {
			wholeFile += currentPath;

		}
		else
			wholeFile += currentLine;
	}

	jsonFile.close();
	newJsonFile.open("com.cogcommtl.hardware.host.json");

	wholeFileChar = wholeFile.c_str();
	fileLength = wholeFile.length();

	newJsonFile.write(wholeFileChar, fileLength);
	
	newJsonFile.close();
	
    return 0;
}

