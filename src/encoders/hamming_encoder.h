/**
* @file hamming_encoder.h
* @author Martin Kosdy
* @author Matus Kysel
* @date 2016
* @brief Hamming encoder class
*
*/

#pragma once

#include <string>

#include "encoder.h"

constexpr auto EncoderHammingCodeNameDef = "Hamming";

namespace stego_disk {

class HammingEncoder : public Encoder {

public:
  HammingEncoder();
  explicit HammingEncoder(uint32 parity_bits);

  virtual int Embed(uint8 *codeword, const uint8 *data) override;
  virtual int Extract(const uint8 *codeword, uint8 *data) override;

  virtual void SetArgByName(std::string_view arg, std::string_view val) override;

  static const std::string GetName();
  virtual const std::string GetNameInstance() const override;
  static std::shared_ptr<Encoder> GetNew();
  virtual std::shared_ptr<Encoder> GetNewInstance() override;

  static int GetParityBitsMin();
  static int GetParityBitsMax();

private:
  void Init(uint32 parity_bits);
  uint64 ComputeH(const uint8 *buffer);
  void SwapBitInCodeword(uint8 *buffer, int bit_in);
  void WriteBitsToBuffer(uint64 value, int offset, uint8 *buffer);
  uint64 ReadBitsFromBuffer(const uint8 *buffer, int offset, int length);

  uint32 parity_bits_{ 0 };
  uint32 total_bits_{ 0 };

  uint32 codewords_in_block_{ 0 };
  uint32 codeword_buffer_size_{ 0 };

  static const uint32 kEncoderHammingParityBitsMin = 3;
  static const uint32 kEncoderHammingParityBitsMax = 8;
  static const std::string kEncoderHammingCodeName;
  // default block size
  static const uint32 kEncoderHammingDefaultParityBits = 5;
};

} // stego_disk
