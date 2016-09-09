// config.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"

using namespace std;


int main(int numberOfParameters, char *parametersArray[])
{
	const string HOOK = "\"@@path\",";
	string currentPath;
	string wholeFile;
	string currentLine;
	const char *wholeFileChar;
	size_t fileLength;

	currentPath = parametersArray[1];

	ifstream jsonFile;
	jsonFile.open(currentPath + "\\com.cogcommtl.hardware.host.json", ios::in);

//	cout << "bad : " << jsonFile.bad() << endl;
//	cout << "fail: " << jsonFile.fail() << endl;


/*	if (!jsonFile) {
		cout << "nope!";
	}*/
	
	jsonFile >> currentLine;

	while (!jsonFile.eof()) {
		if (currentLine == HOOK) {
			//TODO: remove hardcoding of exe file
			wholeFile += ("\"" + currentPath + "chromeparallel.exe\", ");

		}
		else {
			wholeFile += (currentLine+ " ");
		}

		jsonFile >> currentLine;
	}

	jsonFile.clear();
	jsonFile.close();

	ofstream newJsonFile(currentPath + "\\com.cogcommtl.hardware.host.json");

	wholeFile += " }";

	wholeFileChar = wholeFile.c_str();
	fileLength = wholeFile.length();

	newJsonFile.write(wholeFileChar, fileLength);
	
	newJsonFile.close();
	
    return 0;
}

