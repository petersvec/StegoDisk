/**
* @file carrier_files_manager.h
* @author Martin Kosdy
* @author Matus Kysel
* @date 2016
* @brief Class for managing carrier files
*
*/

#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <unordered_set>

#include "utils/stego_types.h"

namespace stego_disk {

	/**
	 * Forward declarations
	 */
	class Encoder;
	class VirtualStorage;
	class CarrierFile;
	class Key;
	class Hash;
	class ThreadPool;

static std::unordered_set<std::string> SupportedFormats {
	"jpg", "bmp", "png", "mkv", "mp4"
};

class CarrierFilesManager {

public:
  CarrierFilesManager();
  ~CarrierFilesManager();

  void LoadDirectory(std::string_view directory, std::string_view filter = "");
  void SaveAllFiles();

  uint64 GetCapacity() const;
  uint64 GetRawCapacity() const;
  std::string GetPath() const;
  uint64 GetCapacityUsingEncoder(std::shared_ptr<Encoder> encoder) const;

  void ApplyEncoder();
  void SetEncoder(std::shared_ptr<Encoder> encoder);
  void UnSetEncoder();
  void SetEncoderArg(std::string_view param, std::string_view val);

  void SetPassword(std::string_view password);

  bool LoadVirtualStorage(std::shared_ptr<VirtualStorage> storage);
  void SaveVirtualStorage();

  std::string CreateFilterFromConfig() const;

private:
  void GenerateMasterKey();
  void DeriveSubkeys();

  std::string base_path_;

  std::vector<std::shared_ptr<CarrierFile>> carrier_files_;
  uint64 capacity_{ 0 };
  uint64 files_in_directory_{ 0 };

  std::unique_ptr<Key> master_key_;
  std::shared_ptr<VirtualStorage> virtual_storage_{ nullptr };
  std::shared_ptr<Encoder> encoder_{ nullptr };
  std::unique_ptr<ThreadPool> thread_pool_;
  std::unique_ptr<Hash> password_hash_;
  bool is_active_encoder_{ false };
};

} // stego_disk
