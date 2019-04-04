#include "dokan_service.h"
#include "stego_storage.h"
#include "logging/logger.h"

#include <thread>

namespace stego_disk
{
	/**
	* Static variables
	*/
	observer_ptr<StegoStorage> DokanService::stego_storage_ = nullptr;
	uint64 DokanService::capacity_ = 0;
	std::wstring DokanService::file_path_;
	std::wstring DokanService::mount_point_;
	PDOKAN_OPERATIONS DokanService::operations_ = nullptr;
	PDOKAN_OPTIONS DokanService::options_ = nullptr;

	DokanService::DokanService()
	{

	}

	void DokanService::Init(observer_ptr<StegoStorage> stego_storage, const std::string &mount_point)
	{
		LOG_INFO("Initializing Dokan");

		const std::string path = "virtualdisc.iso";
		DokanService::file_path_ = std::wstring(path.begin(), path.end());

		if (stego_storage)
		{
			capacity_ = stego_storage->GetSize();
			stego_storage_ = stego_storage;
		}

		mount_point_ = StringToWString(mount_point);

		operations_ = static_cast<PDOKAN_OPERATIONS>(malloc(sizeof(PDOKAN_OPERATIONS)));
		options_ = static_cast<PDOKAN_OPTIONS>(malloc(sizeof(PDOKAN_OPTIONS)));

		ZeroMemory(DokanService::options_, sizeof(DOKAN_OPTIONS));
		options_->Version = DOKAN_VERSION;
		options_->MountPoint = mount_point_.c_str();

		ZeroMemory(DokanService::operations_, sizeof(DOKAN_OPERATIONS));
		operations_->ZwCreateFile = stego_disk::SFSCreateFile;
		operations_->FindFiles = stego_disk::SFSFindFiles;
		operations_->Mounted = stego_disk::SFSMounted;
		operations_->Unmounted = stego_disk::SFSUnmounted;
		operations_->GetVolumeInformation = stego_disk::SFSGetVolumeInformation;
		operations_->ReadFile = stego_disk::SFSReadFile;
		operations_->WriteFile = stego_disk::SFSWriteFile;
	}

	void DokanService::Mount()
	{
		LOG_INFO("Mounting Dokan");

		//std::thread th([]() {
			DokanMain(DokanService::options_, DokanService::operations_);
		//});
		
		//th.detach();
	}

	std::wstring StringToWString(const std::string &str)
	{
		std::wstring temp = std::wstring(str.begin(), str.end());
		return temp;
	}

	FILETIME GetCurrentFileTime()
	{
		FILETIME ft;
		SYSTEMTIME st;

		GetSystemTime(&st);
		SystemTimeToFileTime(&st, &ft);

		return ft;
	}

	std::string LPCWSTRToString(LPCWSTR str)
	{
		std::wstring ws(str);
		return std::string(ws.begin(), ws.end());
	}

	NTSTATUS DOKAN_CALLBACK SFSCreateFile(LPCWSTR FileName, PDOKAN_IO_SECURITY_CONTEXT SecurityContext, ACCESS_MASK DesiredAccess, ULONG FileAttributes, ULONG ShareAccess, ULONG CreateDisposition, ULONG CreateOptions, PDOKAN_FILE_INFO DokanFileInfo)
	{
		return STATUS_SUCCESS;
	}

	NTSTATUS DOKAN_CALLBACK SFSFindFiles(LPCWSTR FileName, PFillFindData FillFindData, PDOKAN_FILE_INFO DokanFileInfo)
	{
		WIN32_FIND_DATAW file_info;
		auto file_time = GetCurrentFileTime();

		file_info.nFileSizeLow = DokanService::capacity_;
		file_info.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
		file_info.ftCreationTime = file_time;
		file_info.ftLastAccessTime = file_time;
		file_info.ftLastWriteTime = file_time;

		auto i{ 0u };
		for (i = 0u; i < DokanService::file_path_.size(); i++)
		{
			file_info.cFileName[i] = DokanService::file_path_[i];
		}
		file_info.cFileName[i] = '\0';

		FillFindData(&file_info, DokanFileInfo);

		return STATUS_SUCCESS;
	}

	NTSTATUS DOKAN_CALLBACK SFSMounted(PDOKAN_FILE_INFO DokanFileInfo)
	{
		return STATUS_SUCCESS;
	}

	NTSTATUS DOKAN_CALLBACK SFSUnmounted(PDOKAN_FILE_INFO DokanFileInfo)
	{
		return STATUS_SUCCESS;
	}

	NTSTATUS DOKAN_CALLBACK SFSGetVolumeInformation(LPWSTR VolumeNameBuffer, DWORD VolumeNameSize, LPDWORD VolumeSerialNumber, LPDWORD MaximumComponentLength, LPDWORD FileSystemFlags, LPWSTR FileSystemNameBuffer, DWORD FileSystemNameSize, PDOKAN_FILE_INFO DokanFileInfo)
	{
		// TODO:

		return STATUS_SUCCESS;
	}

	NTSTATUS DOKAN_CALLBACK SFSReadFile(LPCWSTR FileName, LPVOID Buffer, DWORD BufferLength, LPDWORD ReadLength, LONGLONG Offset, PDOKAN_FILE_INFO DokanFileInfo)
	{
		if (wcscmp(FileName, DokanService::file_path_.c_str()) != 0)
		{
			return STATUS_NO_SUCH_FILE;
		}

		uint64 offset64 = Offset;
		uint64 size64 = BufferLength;

		if (offset64 >= DokanService::capacity_)
		{
			// log...
			// error
			return STATUS_END_OF_FILE;
		}

		if (offset64 + size64 > DokanService::capacity_)
		{
			// log...
			size64 = DokanService::capacity_ - offset64;
		}

		// try catch?
		DokanService::stego_storage_->Read(Buffer, offset64, size64);

		return STATUS_SUCCESS;
	}

	NTSTATUS DOKAN_CALLBACK SFSWriteFile(LPCWSTR FileName, LPCVOID Buffer, DWORD NumberOfBytesToWrite, LPDWORD NumberOfBytesWritten, LONGLONG Offset, PDOKAN_FILE_INFO DokanFileInfo)
	{
		if (wcscmp(FileName, DokanService::file_path_.c_str()) != 0)
		{
			return STATUS_NO_SUCH_FILE;
		}

		uint64 Offset64 = Offset;
		uint64 size64 = NumberOfBytesToWrite;

		if (Offset64 >= DokanService::capacity_)
		{
			// log...
			// error
			return STATUS_END_OF_FILE;
		}

		if (Offset64 + size64 > DokanService::capacity_)
		{
			// log...
			size64 = DokanService::capacity_ - Offset64;
		}

		// try catch?
		DokanService::stego_storage_->Write(Buffer, Offset64, size64);

		return STATUS_SUCCESS;
	}
}