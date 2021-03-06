/**
* @file key.h
* @author Martin Kosdy
* @author Matus Kysel
* @date 2016
* @brief Key class
*
*/

#pragma once

#include <string>
#include <memory>

namespace stego_disk {
	/**
	 * Forward declarations
	 */
	class MemoryBuffer;

class Key {

public:
  Key();
  explicit Key(const MemoryBuffer& data_buffer);
  ~Key();

  std::size_t GetSize();

  const MemoryBuffer& GetData()
  {
	  return *data_;
  }

  static Key FromString(std::string_view input);

  Key& operator^=(const Key& other);
  Key operator^(const Key& other);

private:
	std::unique_ptr<MemoryBuffer> data_;
};
} // stego_disk

