/**
* @file encoder.h
* @author Martin Kosdy
* @author Matus Kysel
* @date 2016
* @brief Interface class for encoders
*
*/

#pragma once

#include <memory>
#include <string>
#include <stdexcept>

#include "utils/stego_types.h"
#include "utils/non_copyable.h"

namespace stego_disk {

class Encoder : public NonCopyable {

public:
  Encoder();
  virtual ~Encoder();
  virtual int Embed(uint8* codeword, const uint8* data) = 0;
  virtual int Extract(const uint8* codeword, uint8* data) = 0;

  virtual uint32 GetDataBlockSize();
  virtual uint32 GetCodewordBlockSize();
  virtual void SetArgByName(std::string_view arg, std::string_view val) = 0;

  virtual const std::string GetNameInstance() const = 0;
  virtual std::shared_ptr<Encoder> GetNewInstance() = 0;

protected:
	uint32 codeword_block_size_{ 0 };
	uint32 data_block_size_{ 0 };
};

} // stego_disk
