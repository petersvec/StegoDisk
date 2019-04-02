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
	const std::string DokanService::file_path_ = "/virtualdisc.iso";
	std::string DokanService::mount_point_ = "";
	PDOKAN_OPERATIONS DokanService::operations_ = nullptr;
	PDOKAN_OPTIONS DokanService::options_ = nullptr;

	DokanService::DokanService()
	{

	}

	void DokanService::Init(observer_ptr<StegoStorage> stego_storage, const std::string &mount_point)
	{
		LOG_INFO("Initializing Dokan");

		if (stego_storage_)
		{
			capacity_ = stego_storage_->GetSize();
		}

		mount_point_ = mount_point;

		operations_ = static_cast<PDOKAN_OPERATIONS>(malloc(sizeof(PDOKAN_OPERATIONS)));
		options_ = static_cast<PDOKAN_OPTIONS>(malloc(sizeof(PDOKAN_OPTIONS)));

		ZeroMemory(options_, sizeof(PDOKAN_OPTIONS));
		options_->Version = DOKAN_VERSION;
		options_->MountPoint = StringToLPCWSTR(mount_point_);

		ZeroMemory(operations_, sizeof(PDOKAN_OPERATIONS));
		operations_->ZwCreateFile = SFSCreateFile;
		operations_->FindFiles = SFSFindFiles;
		operations_->Mounted = SFSMounted;
		operations_->Unmounted = SFSUnmounted;
		operations_->GetVolumeInformation = SFSGetVolumeInformation;
		operations_->ReadFile = SFSReadFile;
		operations_->WriteFile = SFSWriteFile;
	}

	void DokanService::Mount()
	{
		LOG_INFO("Mounting Dokan");

		std::thread th([]() {
			DokanMain(options_, operations_);
		});
		
		th.detach();
	}

	LPCWSTR StringToLPCWSTR(const std::string &str)
	{
		std::wstring temp = std::wstring(str.begin(), str.end());
		return temp.c_str();
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
		CopyMemory(file_info.cFileName, StringToLPCWSTR(DokanService::file_path_), DokanService::file_path_.size());

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
		if (LPCWSTRToString(FileName) != DokanService::file_path_)
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
		if (LPCWSTRToString(FileName) != DokanService::file_path_)
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