#pragma once

#include <dokan/dokan.h>
#include <dokan/fileinfo.h>
#include <string>
#include <WinBase.h>
#include <stdlib.h>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "stego-disk_export.h"
#include "utils/non_copyable.h"
#include "utils/stego_types.h"

namespace stego_disk
{
	/**
	 * Forward declarations
	 */
	class StegoStorage;

	/**
	 * TODO: replace by real observing pointer in future
	 */
	template<class Type>
	using observer_ptr = Type *;

	/**
	 * Helper functions
	 */
	static std::wstring StringToWString(const std::string &str);
	static FILETIME GetCurrentFileTime();
	static std::string LPCWSTRToString(LPCWSTR str);

	/**
	 * Dokan operations
	 */
	static NTSTATUS DOKAN_CALLBACK SFSCreateFile(LPCWSTR FileName, PDOKAN_IO_SECURITY_CONTEXT SecurityContext, ACCESS_MASK DesiredAccess, ULONG FileAttributes, ULONG ShareAccess, ULONG CreateDisposition, ULONG CreateOptions, PDOKAN_FILE_INFO DokanFileInfo);
	static NTSTATUS DOKAN_CALLBACK SFSFindFiles(LPCWSTR FileName, PFillFindData FillFindData, PDOKAN_FILE_INFO DokanFileInfo);
	static NTSTATUS DOKAN_CALLBACK SFSMounted(PDOKAN_FILE_INFO DokanFileInfo);
	static NTSTATUS DOKAN_CALLBACK SFSUnmounted(PDOKAN_FILE_INFO DokanFileInfo);
	static NTSTATUS DOKAN_CALLBACK SFSGetVolumeInformation(LPWSTR VolumeNameBuffer, DWORD VolumeNameSize, LPDWORD VolumeSerialNumber, LPDWORD MaximumComponentLength, LPDWORD FileSystemFlags, LPWSTR FileSystemNameBuffer, DWORD FileSystemNameSize, PDOKAN_FILE_INFO DokanFileInfo);
	static NTSTATUS DOKAN_CALLBACK SFSReadFile(LPCWSTR FileName, LPVOID Buffer, DWORD BufferLength, LPDWORD ReadLength, LONGLONG Offset, PDOKAN_FILE_INFO DokanFileInfo);
	static NTSTATUS DOKAN_CALLBACK SFSWriteFile(LPCWSTR FileName, LPCVOID Buffer, DWORD NumberOfBytesToWrite, LPDWORD NumberOfBytesWritten, LONGLONG Offset, PDOKAN_FILE_INFO DokanFileInfo);
	static NTSTATUS DOKAN_CALLBACK SFSGetFileInformation(LPCWSTR FileName, LPBY_HANDLE_FILE_INFORMATION HandleFileInformation, PDOKAN_FILE_INFO DokanFileInfo);

	class STEGO_DISK_EXPORT DokanService : public NonCopyable {
	public:
		DokanService();

		static void Init(observer_ptr<StegoStorage> stego_storage, const std::string &mount_point);
		static void Mount();
		static void Unmount();
		static void IsMounted();

		static observer_ptr<StegoStorage> stego_storage_;
		static uint64 capacity_;
		static std::wstring file_path_;
		static std::wstring mount_point_;
		static PDOKAN_OPERATIONS operations_;
		static PDOKAN_OPTIONS options_;
		static std::mutex dokan_mutex_;
		static std::mutex mount_mutex_;
		static std::condition_variable mount_ready_;
	};
}
