/**
* @file fitness.h
* @author Matus Kysel
* @date 2016
* @brief Fitness functions interface
*
*/

#pragma once

#include <string>
#include <memory>
#include <stdexcept>

#include "utils/stego_types.h"

namespace stego_disk {

	/**
	 * Forward declarations
	 */
	class CarrierFile;
	class MemoryBuffer;

class Fitness {

public:
  explicit Fitness(std::shared_ptr<CarrierFile> file) { file_ = file; }

  Fitness(const Fitness&) = delete;
  Fitness& operator=(const Fitness&) = delete;

  virtual ~Fitness() {}

  virtual uint64 SelectBytes(const MemoryBuffer &in, MemoryBuffer *out) = 0;

  virtual void InsertBytes(const MemoryBuffer &in, MemoryBuffer *out) const = 0;

protected:
	std::shared_ptr<CarrierFile> file_{ nullptr };

};

} // stego_disk
