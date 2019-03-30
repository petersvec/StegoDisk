/**
* @file keccak_hash_impl.h
* @author Martin Kosdy
* @author Matus Kysel
* @date 2016
* @brief Implementation of hash function Keccak
*
*/

#pragma once

#include <memory>

#include "hash_impl.h"
#include "utils/stego_types.h"

namespace stego_disk {
	/**
	 * Forward declarations
	 */
	class MemoryBuffer;

class KeccakHashImpl : public HashImpl {

public:
    explicit KeccakHashImpl(std::size_t state_size = 32);
    virtual ~KeccakHashImpl();

	virtual void Process(MemoryBuffer& state, const uint8* data, std::size_t length) override;
};

} // stego_disk

