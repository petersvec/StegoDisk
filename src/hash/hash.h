/**
* @file hash.h
* @author Martin Kosdy
* @author Matus Kysel
* @date 2016
* @brief Interface for hash functions
*
*/

#pragma once

#include <memory>
#include <string>

#include "utils/stego_types.h"

namespace stego_disk {
	/**
	 * Forward declarations
	 */
	class HashImpl;
	class MemoryBuffer;

class Hash final {
public:
  Hash();
  explicit Hash(std::string_view data);
  Hash(const uint8* data, std::size_t length);
  ~Hash();

  void Process(std::string_view data);
  void Process(const MemoryBuffer& data);
  void Process(const uint8* data, std::size_t length);

  void Append(std::string_view data);
  void Append(const MemoryBuffer& data);
  void Append(const uint8* data, std::size_t length);
  const MemoryBuffer& GetState() const;
  std::size_t GetStateSize() const;

private:
  void Init();
  std::unique_ptr<MemoryBuffer> state_{ nullptr };

  //TODO: implementation could be stored as shared ptr in each hash instance
  //  std::shared_ptr<HashImpl> hashImpl;
private:
  static std::unique_ptr<HashImpl> default_hash_impl_;

};

} // stego_disk

