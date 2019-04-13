#include <iostream>
#include <cstdlib>
#include <thread>

#include "stego_storage.h"
#include "fuse/dokan_service.h"
#include "logging/logger.h"

int main(int argc, char *argv[])
{
	Logger::SetVerbosityLevel(std::string("DEBUG"), std::string("cout"));

	LOG_INFO("Start...");

	auto stego_storage = std::make_unique<stego_disk::StegoStorage>();
	stego_storage->Configure("C:\\Users\\Peter\\Desktop\\diplomovka\\config.json");
	stego_storage->Open("C:\\Users\\Peter\\Desktop\\diplomovka\\test_dir", "heslo");
	stego_storage->Load();

	std::cout << "Size: " << stego_storage->GetSize() << std::endl;

	stego_disk::DokanService::Init(stego_storage.get(), "M:\\");
	stego_disk::DokanService::Mount();

	std::cout << "Mounting..." << std::endl;
	stego_disk::DokanService::IsMounted();
	std::cout << "Mounted..." << std::endl;

	std::this_thread::sleep_for(std::chrono::seconds(50));

	//std::cout << "Unmounting...";
	//stego_disk::DokanService::Unmount();

//	std::cout << "Mounting virtual disc..." << std::endl;
//	system("powershell.exe Mount-DiskImage -ImagePath 'M:\\virtualdisc.iso'");
	return 0;
}