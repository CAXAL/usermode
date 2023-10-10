#include "Kernel.hpp"
#include "ModuleMapper.hpp"
#include "MemoryUtils.hpp"
#include <filesystem>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <TlHelp32.h>
#include "xor.hpp"

uint64_t  game_assembly;
uint64_t  unity_player;
MemoryUtils* memoryUtils = nullptr;
void GetCamera()
{
	while (true)
	{
		const auto camera_manager_offset = memoryUtils->Read<UINT64>(unity_player + 0x1A74500);
		std::cout << "camera_manager_offset: \t" << "0x" << hex << camera_manager_offset << std::endl;
		const auto camera_manager = memoryUtils->Read<UINT64>(camera_manager_offset);
		const auto camera = memoryUtils->Read<UINT64>(camera_manager);
		const auto camera_object = memoryUtils->Read<UINT64>(camera + 0x30);
		const auto object_class = memoryUtils->Read<UINT64>(camera_object + 0x30);
		const auto entity = memoryUtils->Read<UINT64>(object_class + 0x18);
	}
	
}

int main()
{
	

	auto functionName = xorstr("NtUserCreateDesktopEx");

	OperationCallback operation = Communication::Init(functionName.crypt_get());

	functionName.crypt();

	if (!operation)
	{
		printf(xorstr_("\n[IO] Failed."));
		Exit;
	}

	bool status = Communication::IsConnected(operation);

	if (!status)
	{
		printf(xorstr_("\n[IO] Loading vdm...\n"));

		Sleep(2000);

		std::vector<uint8_t> driverImage(sizeof(Kernel_sys));
		memcpy(driverImage.data(), Kernel_sys, sizeof(Kernel_sys));

		if (!Mapper::MapDriver(driverImage))
		{
			printf(xorstr_("\n\n[IO] Loading failed."));
			Exit;
		}

		printf(xorstr_("\n[IO] Success!\n\n"));
	}
	else
	{
		printf(xorstr_("[IO] Success!\n\n"));
	}
	Sleep(1000);
	DWORD processId = ProcessUtils::GetProcessID(xorstr_("RustClient.exe"));

	if (!processId)
	{
		printf(xorstr_("\n[IO] Game not found."));
		Exit;
	}
	memoryUtils = new MemoryUtils(operation, processId);
	ModuleMapper::memoryInstance = memoryUtils;
	game_assembly = memoryUtils->GetModuleBase(L"GameAssembly.dll");
	printf("base: 0x%p\n", game_assembly);
	unity_player = memoryUtils->GetModuleBase(L"UnityPlayer.dll");
	printf("base: 0x%p\n", unity_player);

	GetCamera();
	std::cin.get();
	return EXIT_SUCCESS;
}
