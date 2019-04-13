/**
* @file encoder_factory.h
* @author Martin Kosdy
* @author Matus Kysel
* @date 2016
* @brief Encoder factory
*
*/

#pragma once

#include <memory>
#include <vector>
#include <string>

#include "stego-disk_export.h"
#include "utils/non_copyable.h"

namespace stego_disk {

	/**
	 * Forward declarations
	 */
	class Encoder;

class STEGO_DISK_EXPORT EncoderFactory final : public NonCopyable
{
private: 
  EncoderFactory();

public:

  enum class EncoderType {
    LSB,
    HAMMING
  };

  // Get vector of all encoders (each encoder with all possible settings)
  static std::vector<std::shared_ptr<Encoder>> GetAllEncoders();
  // Get vector of all encoders (each encoder once with default settings)
  static std::vector<std::shared_ptr<Encoder>> GetEncoders();
  // Get names of all encoders supported by this library
  static std::vector<std::string> GetEncoderNames();
  // Get instance of encoder based on the his type
  static std::shared_ptr<Encoder> GetEncoder(const EncoderType encoder);
  // set encoder param by name
  static void SetEncoderArg(std::shared_ptr<Encoder> encoder,
                                  std::string_view param, std::string_view val);
  // Get instance of the default encoder
  static std::shared_ptr<Encoder> GetDefaultEncoder();

  static EncoderType GetDefaultEncoderType();

  static EncoderType GetEncoderType(std::string_view encoder);

  static const std::string GetEncoderName(const EncoderType encoder);

private:
  static const EncoderType kDefaultEncoder = EncoderType::HAMMING;
};

} // stego_disk
