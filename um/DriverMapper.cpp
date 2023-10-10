#include "DriverMapper.hpp"

#include "VDM_MAP.hpp"
#include "Vulnerable.hpp"

bool Mapper::MapDriver(std::vector<uint8_t> driverImage)
{
	if (VDM::IsLoaded())
	{
		printf(xorstr_("\n[IO] Failed to load vdm. Driver already loaded.\n"));

		return false;
	}

	std::vector<uint8_t> vdm_image(sizeof(ene_sys));
	memcpy(vdm_image.data(), ene_sys, sizeof(ene_sys));

	string driverPath = "";

	auto loadInfo = VDM::Load(vdm_image, driverPath);

	auto hDeviceHadle = loadInfo.first;
	auto driverName = loadInfo.second;

	if (hDeviceHadle == NULL || driverName.empty())
	{
		printf(xorstr_("\n[IO] Failed to load vdm.\n"));

		return false;
	}

	if (!VDM::Init(hDeviceHadle))
	{
		printf(xorstr_("\n[IO] Failed to init vdm.\n"));

		return false;
	}

	if (!VDM::InitPageTableBase())
	{
		printf(xorstr_("\n[IO] Failed to init page tables.\n"));

		VDM::Unload(driverName);

		return false;
	}

	printf(xorstr_("[IO] Cleaning...\n"));
	Sleep(1000);

	if (!TraceCleaner::CleanPiDDBCacheTable())
	{
		printf(xorstr_("\n[IO] Failed: #1.\n"));

		VDM::Unload(driverName);

		return false;
	}

	if (!TraceCleaner::CleanKernelHashBucketList(driverName, driverPath))
	{
		printf(xorstr_("\n[IO] Failed: #2.\n"));

		VDM::Unload(driverName);

		return false;
	}

	if (!TraceCleaner::CleanMmUnloadedDrivers(hDeviceHadle))
	{
		printf(xorstr_ ("\n[IO] Failed: #3.\n"));

		VDM::Unload(driverName);

		return false;
	}

	printf(xorstr_("[IO] Mapping...\n"));
	Sleep(1000);

	if (!VDM_MAP::MapKernelModule(driverImage))
	{
		VDM::Unload(driverName);

		return false;
	}

	if (!VDM::Unload(driverName))
		return false;

	return true;
}