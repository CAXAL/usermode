#pragma once

#include "ProcessUtils.hpp"

class MemoryUtils
{
public:
	MemoryUtils(OperationCallback operation, uint64_t pid);

	NTSTATUS ReadBuffer(uint64_t address, LPVOID lpBuffer, SIZE_T nSize);
	NTSTATUS WriteMemory(uint64_t address, LPVOID dstAddress, SIZE_T nSize);

	template <typename T>
	T Read(uint64_t address);
	template <typename T>
	T Write(uintptr_t Addr, T Data);
	uint64_t GetModuleBase(wstring moduleName);

	uint64_t AllocateMemory(size_t size, uint32_t allocation_type, uint32_t protect);
	NTSTATUS ProtectMemory(uint64_t address, size_t size, uint32_t protect);
	NTSTATUS FreeMemory(uint64_t address);

	uint64_t AllocateStealthMemory(size_t size);

private:
	OperationCallback operationCallback;

	uint64_t processId = 0;
};

template<typename T>
inline T MemoryUtils::Read(uint64_t address)
{
	T buffer{ };
	ReadBuffer(address, &buffer, sizeof(T));
	return buffer;
}

template<typename T>
inline T MemoryUtils::Write(uintptr_t address, T Data)
{
	T buffer{ };
	WriteMemory(address, &buffer, sizeof(T));
	return buffer;
}


extern MemoryUtils* memoryUtils;