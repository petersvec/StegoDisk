/**
* @file hash_impl.h
* @author Martin Kosdy
* @author Matus Kysel
* @date 2016
* @brief Interface for hash implementations
*
*/

#pragma once

#include <memory>
#include <stdexcept>

#include "utils/stego_types.h"

namespace stego_disk
{
	/**
	 * Forward declarations
	 */
	class MemoryBuffer;

class HashImpl {

public:
  HashImpl() : state_size_(0) {}
  virtual ~HashImpl() {}

  virtual void Process(MemoryBuffer& state,
                       const uint8* data, std::size_t length) = 0;
  virtual void Append(MemoryBuffer& state,
                      const uint8* data, std::size_t length);

  std::size_t GetStateSize() const { return state_size_; }

protected:
  size_t state_size_;

};

} // stego_disk