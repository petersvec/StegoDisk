/**
* @file stego_storage.h
* @author Martin Kosdy
* @author Matus Kysel
* @date 2016
* @brief File containing StegoStorage interface.
*
*/

#pragma once

#include <memory>
#include <string>
#include <stdexcept>

#include "encoders/encoder_factory.h"
#include "stego-disk_export.h"
#include "permutations/permutation_factory.h"
#include "utils/json.h"
#include "utils/non_copyable.h"

namespace stego_disk {
	/**
	 * Forward declarations
	 */
	class CarrierFilesManager;
	class VirtualStorage;

class STEGO_DISK_EXPORT StegoStorage : public NonCopyable {

public:
  StegoStorage();
  ~StegoStorage();

  void Open(std::string_view storage_base_path, std::string_view password, std::string_view filter = "");
  void Load();
  void Save();

  void Read(void* destination, const std::size_t offset,
            const std::size_t length) const;
  void Write(const void* source, const std::size_t offset,
             const std::size_t length) const;

  void Configure(std::string_view config_path) const;
  void Configure() const;
  void Configure(const EncoderFactory::EncoderType encoder,
                 const PermutationFactory::PermutationType global_perm,
                 const PermutationFactory::PermutationType local_perm) const;

  std::size_t GetSize() const;

  void ChangeEncoder(std::string &config) const;

private:

	std::unique_ptr<CarrierFilesManager> carrier_files_manager_{ nullptr };
	std::shared_ptr<VirtualStorage> virtual_storage_{ nullptr };
	bool opened_{ false };
};

} // stego_disk
