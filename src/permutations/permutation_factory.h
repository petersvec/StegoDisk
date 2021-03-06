/**
* @file permutation_factory.h
* @author Martin Kosdy
* @author Matus Kysel
* @date 2016
* @brief Permutation factory
*
*/

#pragma once

#include <memory>
#include <vector>

#include "stego-disk_export.h"
#include "utils/non_copyable.h"

namespace stego_disk {
	/**
	 * Forward declarations
	 */
	class Permutation;

class STEGO_DISK_EXPORT PermutationFactory final : public NonCopyable {
private:
  PermutationFactory();
public:

  enum class PermutationType {
    IDENTITY,
    AFFINE,
    AFFINE64,
    FEISTEL_NUM,
    FEISTEL_MIX
  };

  // get vector of all permutations (each permutation once)
  static std::vector<std::shared_ptr<Permutation>> GetPermutations();
  // get instance of permutation based on the code name
  static std::shared_ptr<Permutation> GetPermutation(
      std::string_view permutation_name);
  // get instance of permutation based on type
  static std::shared_ptr<Permutation> GetPermutation(
      const PermutationType permutation);
  // get instance of the default permutation
  static std::shared_ptr<Permutation> GetDefaultPermutation();

  static PermutationType GetDefaultPermutationType();

  static PermutationType GetPermutationType(std::string_view permutation);

  static const std::string GetPermutationName( const PermutationType permutation);

private:
  static const PermutationType kDefaultPermutation = PermutationType::FEISTEL_MIX;
};

} // stego_disk
