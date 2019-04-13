/**
* @file key.cc
* @author Martin Kosdy
* @author Matus Kysel
* @date 2016
* @brief Key class implementation
*
*/

#include "keys/key.h"
#include "api_mask.h"
#include "hash/hash.h"
#include "utils/memory_buffer.h"

namespace stego_disk {

Key::Key() : data_(nullptr) {}

Key::Key(const MemoryBuffer& data_buffer) : data_(std::make_unique<MemoryBuffer>(data_buffer)) {}

Key::~Key() {}

Key& Key::operator^=(const Key& other) {
    (*data_) ^= (*other.data_);
    return *this;
}

Key Key::operator^(const Key& other) {
    return Key(*data_ ^ *other.data_);
}

std::size_t Key::GetSize() {
    return data_->GetSize();
}

stego_disk::Key Key::FromString(std::string_view input) {
    return Key(Hash(input).GetState());
}

} // stego_disk

