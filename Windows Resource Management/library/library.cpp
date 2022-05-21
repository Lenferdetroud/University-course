#include "stdafx.h"
#include "library.h"
#define __cpuid __mycpuidex
#define c_cpuid __mycpuidex

// ��������� ���������� � ����� ������ ���� � ���������� �������������� ������ 
LIBRARY_API int GetInfo(library_info* info) {

	info -> funcKeysCount = GetKeyboardType(2); // ���������� �������������� ������

	// ��������, �������������� �� CPUID
	__asm {
		PUSHFD // ���������� �������� EFLAGS � �����
		POP EAX // ���������� �������� EFLAGS � EAX
		MOV EBX, EAX // ���������� �������� � EBX
		XOR EAX, 00200000h // ��������� 21-�� ����
		PUSH EAX // ���������� ������ �������� � �����
		POPFD // ���������� ������ �������� � EFLAGS
		PUSHFD // ����� ���������� EFLAGS � �����
		POP EAX  // �������� EFLAGS ������ � EAX
		cmp EAX, EBX // �������� 21-�� ����
		JZ RET_ARG // ��� �� ��������� - CPUID �� ��������������
	}

	// ��������� ����� ������ ���� (����� AMD)
	__asm {
		MOV EAX, 0x80000006
		cpuid
		and EDX, 0xFF
		MOV EAX, info
		MOV[EAX], EDX
		JMP RET_ARG
	}

	__asm { RET_ARG: }
}

// ����� ����� ������������ ����������
BOOL WINAPI DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved) {
	switch (fdwReason) {
	case DLL_PROCESS_DETACH: // ���������� DLL
		break;
	}
	return TRUE;
}