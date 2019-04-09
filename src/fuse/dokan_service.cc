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
		operations_->GetFileInformation = stego_disk::SFSGetFileInformation;
	}

	void DokanService::Mount()
	{
		LOG_INFO("Mounting Dokan");

		std::thread th([]() {
			DokanMain(DokanService::options_, DokanService::operations_);
		});
		
		th.detach();
	}

	void DokanService::Unmount()
	{
		LOG_INFO("Unmounting Dokan");
		DokanUnmount(DokanService::mount_point_[0]);
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
		LOG_DEBUG("SFSCreateFile");

		if (wcscmp(FileName, std::wstring(1, '\\').c_str()) == 0)
		{
			DokanFileInfo->IsDirectory = TRUE;
			return STATUS_SUCCESS;
		}

		if (wcscmp(FileName, DokanService::file_path_.c_str()) == 0)
		{
			DokanFileInfo->IsDirectory = FALSE;
			return STATUS_SUCCESS;
		}

		return STATUS_NO_SUCH_FILE;
	}

	NTSTATUS DOKAN_CALLBACK SFSFindFiles(LPCWSTR FileName, PFillFindData FillFindData, PDOKAN_FILE_INFO DokanFileInfo)
	{
		LOG_DEBUG("SFSFindFiles");

		WIN32_FIND_DATAW file_info;
		auto file_time = GetCurrentFileTime();

		file_info.nFileSizeLow = DokanService::capacity_;
		file_info.nFileSizeHigh = 0;
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
		LOG_DEBUG("SFSMounted");
		return STATUS_SUCCESS;
	}

	NTSTATUS DOKAN_CALLBACK SFSUnmounted(PDOKAN_FILE_INFO DokanFileInfo)
	{
		LOG_DEBUG("SFSUnmounted");
		return STATUS_SUCCESS;
	}

	NTSTATUS DOKAN_CALLBACK SFSGetVolumeInformation(LPWSTR VolumeNameBuffer, DWORD VolumeNameSize, LPDWORD VolumeSerialNumber, LPDWORD MaximumComponentLength, LPDWORD FileSystemFlags, LPWSTR FileSystemNameBuffer, DWORD FileSystemNameSize, PDOKAN_FILE_INFO DokanFileInfo)
	{
		LOG_DEBUG("SFSGetVolumeInformation");

		//wcscpy_s(VolumeNameBuffer, VolumeNameSize, L"StegoDisk");

		return STATUS_SUCCESS;
	}

	NTSTATUS DOKAN_CALLBACK SFSReadFile(LPCWSTR FileName, LPVOID Buffer, DWORD BufferLength, LPDWORD ReadLength, LONGLONG Offset, PDOKAN_FILE_INFO DokanFileInfo)
	{
		LOG_DEBUG("SFSReadFile");

		if (wcscmp(FileName, DokanService::file_path_.c_str()) != 0)
		{
			return STATUS_NO_SUCH_FILE;
		}

		uint64 offset64 = Offset;
		uint64 size64 = BufferLength;

		if (offset64 >= DokanService::capacity_)
		{
			LOG_ERROR("Trying to read past the end of storage!");
			LOG_DEBUG("File: " + std::string(DokanService::file_path_.begin(), DokanService::file_path_.end()) +
					  " offset: " + std::to_string(offset64) + 
					  " size: " + std::to_string(size64));
			return STATUS_END_OF_FILE;
		}

		if (offset64 + size64 > DokanService::capacity_)
		{
			LOG_INFO("Trim the read to file size!");
			size64 = DokanService::capacity_ - offset64;
			LOG_DEBUG("File: " + std::string(DokanService::file_path_.begin(), DokanService::file_path_.end()) +
					  " capacity: " + std::to_string(DokanService::capacity_) +
					  " offset: " + std::to_string(offset64) +
					  " size: " + std::to_string(size64));
		}

		// try catch?
		DokanService::stego_storage_->Read(Buffer, offset64, size64);

		return STATUS_SUCCESS;
	}

	NTSTATUS DOKAN_CALLBACK SFSWriteFile(LPCWSTR FileName, LPCVOID Buffer, DWORD NumberOfBytesToWrite, LPDWORD NumberOfBytesWritten, LONGLONG Offset, PDOKAN_FILE_INFO DokanFileInfo)
	{
		LOG_DEBUG("SFSWriteFile");

		if (wcscmp(FileName, DokanService::file_path_.c_str()) != 0)
		{
			return STATUS_NO_SUCH_FILE;
		}

		uint64 offset64 = Offset;
		uint64 size64 = NumberOfBytesToWrite;

		if (offset64 >= DokanService::capacity_)
		{
			LOG_ERROR("Trying to write past the end of storage!");
			LOG_DEBUG("File: " + std::string(DokanService::file_path_.begin(), DokanService::file_path_.end()) +
				" offset: " + std::to_string(offset64) +
				" size: " + std::to_string(size64));
			return STATUS_END_OF_FILE;
		}

		if (offset64 + size64 > DokanService::capacity_)
		{
			LOG_INFO("Trim the write to file size!");
			size64 = DokanService::capacity_ - offset64;
			LOG_DEBUG("File: " + std::string(DokanService::file_path_.begin(), DokanService::file_path_.end()) +
				" capacity: " + std::to_string(DokanService::capacity_) +
				" offset: " + std::to_string(offset64) +
				" size: " + std::to_string(size64));
		}

		// try catch?
		DokanService::stego_storage_->Write(Buffer, offset64, size64);

		return STATUS_SUCCESS;
	}

	NTSTATUS DOKAN_CALLBACK SFSGetFileInformation(LPCWSTR FileName, LPBY_HANDLE_FILE_INFORMATION HandleFileInformation, PDOKAN_FILE_INFO DokanFileInfo)
	{
		LOG_DEBUG("SFSGetFileInformation");

		auto file_time = GetCurrentFileTime();

		if (wcscmp(FileName, std::wstring(1, '\\').c_str()) == 0)
		{
			HandleFileInformation->dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
			HandleFileInformation->ftCreationTime = file_time;
			HandleFileInformation->ftLastAccessTime = file_time;
			HandleFileInformation->ftLastWriteTime = file_time;
			return STATUS_SUCCESS;
		}

		if (wcscmp(FileName, DokanService::file_path_.c_str()) == 0)
		{
			HandleFileInformation->dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
			HandleFileInformation->ftCreationTime = file_time;
			HandleFileInformation->ftLastAccessTime = file_time;
			HandleFileInformation->ftLastWriteTime = file_time;
			HandleFileInformation->nFileSizeLow = DokanService::capacity_;
			HandleFileInformation->nFileSizeHigh = 0;
			return STATUS_SUCCESS;
		}

		return STATUS_NO_SUCH_FILE;
	}
}