#include "stdafx.h"
#include "parallelport.h"
#include "inpout32.h"
#include "windows.h"

namespace WinParallelPort {

	bool sendTrig(int trigger) {
		if (trigger < 0 || trigger > 255) {
			return false;
		}
		else {
			Inp32(trigger);
			Sleep(1);
			Inp32(0);
			return true;
		}
	}

}