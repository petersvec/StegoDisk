/**
* @file carrier_files_manager.h
* @author Martin Kosdy
* @author Matus Kysel
* @date 2016
* @brief Class for managing carrier files
*
*/

#ifndef STEGODISK_FILEMANAGEMENT_CARRIERFILESMANAGER_H_
#define STEGODISK_FILEMANAGEMENT_CARRIERFILESMANAGER_H_

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "api_mask.h"
#include "hash/hash.h"
#include "keys/key.h"
#include "utils/thread_pool.h"

namespace stego_disk {

class Encoder;
class VirtualStorage;
class CarrierFile;

class CarrierFilesManager {

public:
  CarrierFilesManager();
  ~CarrierFilesManager();
  void LoadDirectory(const std::string &directory, const std::string &filter = "");
  void SaveAllFiles();

  uint64 GetCapacity() const;
  uint64 GetRawCapacity() const;
  std::string GetPath() const;
  uint64 GetCapacityUsingEncoder(std::shared_ptr<Encoder> encoder) const;

  void ApplyEncoder();
  void SetEncoder(std::shared_ptr<Encoder> encoder);
  void UnSetEncoder();
  void SetEncoderArg(const std::string &param, const std::string &val);

  void SetPassword(const std::string &password);

  bool LoadVirtualStorage(std::shared_ptr<VirtualStorage> storage);
  void SaveVirtualStorage();

private:
  void Init();

  void AddFileAtPath(std::string &path);

  void GenerateMasterKey();
  void DeriveSubkeys();

  std::string base_path_;

  std::vector<std::shared_ptr<CarrierFile>> carrier_files_;
  uint64 capacity_{ 0 };
  uint64 files_in_directory_{ 0 };

  Hash password_hash_;
  Key master_key_;

  std::shared_ptr<VirtualStorage> virtual_storage_{ nullptr };
  std::shared_ptr<Encoder> encoder_{ nullptr };
  std::unique_ptr<ThreadPool> thread_pool_{ nullptr };
  bool is_active_encoder_{ false };
};

} // stego_disk

#endif // STEGODISK_FILEMANAGEMENT_CARRIERFILESMANAGER_H_
