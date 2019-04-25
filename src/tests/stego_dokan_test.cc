#include <iostream>
#include <cstdlib>
#include <thread>

#include "stego_storage.h"
#include "fuse/dokan_service.h"
#include "logging/logger.h"

std::unique_ptr<stego_disk::StegoStorage> stego_storage{ nullptr };

void help()
{
	std::cout << "Usage: stego_dokan_test.exe <option(s)>" << std::endl;
	std::cout << "Options:" << std::endl;
	std::cout << "\t-h,--help\t\tShow this help message" << std::endl;
	std::cout << "\t-d,--directory\t\tSpecitfy the source directory" << std::endl;
	std::cout << "\t-p,--password\t\tSpecify if the password should be used" << std::endl;
	std::cout << "\t-c,--config\t\tSpecify the configuration file" << std::endl;
}

void commands_help()
{
	std::cout << "help\t\tShow this help message" << std::endl;
	std::cout << "mount\t\tMount virtualdisc.iso file" << std::endl;
	std::cout << "unmount\t\tUnmount virtualdisc.iso file and exit" << std::endl;
	std::cout << "exit\t\tExit" << std::endl;
} 

void mount_virtualdisc()
{
	system("powershell.exe Mount-DiskImage -ImagePath 'M:\\virtualdisc.iso'");
}

void unmount_virtualdisc()
{
	system("powershell.exe Dismount-DiskImage -ImagePath 'M:\\virtualdisc.iso'");
}

void mount_cmd()
{
	while (true)
	{
		std::string command;
		std::cout << "Command: ";
		std::cin >> command;

		if (command == "help")
		{
			commands_help();
		}
		else if (command == "mount")
		{
			mount_virtualdisc();
		}
		else if (command == "unmount")
		{
			unmount_virtualdisc();
			stego_storage->Save();
			stego_disk::DokanService::Unmount();
			exit(0);
		}
		else if (command == "exit")
		{
			stego_storage->Save();
			stego_disk::DokanService::Unmount();
			exit(0);
		}
		else
		{
			std::cout << "Unrecognized command!" << std::endl;
		}
	}
}

void mount()
{
	std::cout << "Mounting..." << std::endl;
	stego_disk::DokanService::Init(stego_storage.get(), "M:\\");
	stego_disk::DokanService::Mount();
	stego_disk::DokanService::IsMounted();
	std::cout << "Mounted..." << std::endl;
}

void init_stego_storage(const std::string &directory, const std::string &password, const std::string &config)
{
	stego_storage = std::make_unique<stego_disk::StegoStorage>();
	stego_storage->Configure(config);
	stego_storage->Open(directory, password);
	stego_storage->Load();

	mount();
	mount_cmd();
}

int main(int argc, char *argv[])
{
	Logger::SetVerbosityLevel(std::string("INFO"), std::string("cout"));

	std::string directory{ "" }, password{ "" }, config{ "" };

	for (auto i = 1u; i < argc; ++i)
	{
		if (auto arg = std::string(argv[i]); arg == "-h" || arg == "--help")
		{
			help();
			return 0;
		}
		else if (arg == "-d" || arg == "--directory")
		{
			if (++i < argc)
			{
				directory = argv[i];
			}
			else
			{
				std::cout << "--directory option requires one argument" << std::endl;
				return -1;
			}
		}
		else if (arg == "-p" || arg == "--password")
		{
			if (++i < argc)
			{
				password = argv[i];
			}
			else
			{
				std::cout << "--password option requires one argument" << std::endl;
				return -1;
			}
		}
		else if (arg == "-c" || arg == "--config")
		{
			if (++i < argc)
			{
				config = argv[i];
			}
			else
			{
				std::cout << "--config option requires one argument" << std::endl;
			}
		}
	}

	if (!directory.empty() && !config.empty())
	{
		init_stego_storage(directory, password, config);
	}
	else
	{
		std::cout << "No directory or configuration file specified" << std::endl;
		return -1;
	}

	return 0;
}