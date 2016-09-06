#include "stdafx.h"
#include "parallelport.h"
#include "inpout32.h"
#include "windows.h"

namespace WinParallelPort {

	const short pport = 0x378;

	bool sendTrig(int trigger) {
		if (trigger < 0 || trigger > 255) {
			return false;
		}
		else {
			Out32(pport, trigger);
			Sleep(1);
			Out32(pport, 0);
			return true;
		}
	}

}