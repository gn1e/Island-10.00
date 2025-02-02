#pragma once
#include <windows.h>
#include <iostream>
#include <vector>
#include <memory>
#include <thread>
#include <Minhook.h>
#include "SDK/SDK.hpp"
using namespace std;
using namespace SDK;

namespace Utils
{
	inline void VirtualHook(void** VFT, int Index, LPVOID Hook, void** OG = nullptr)
	{
		if (OG)
			*OG = VFT[Index];

		DWORD Protect;
		VirtualProtect(VFT[Index], sizeof(uintptr_t), PAGE_EXECUTE_READWRITE, &Protect);
		VFT[Index] = Hook;
		VirtualProtect(VFT[Index], sizeof(uintptr_t), Protect, &Protect);
	}
}