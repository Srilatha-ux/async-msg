#include<cassert>
#include<cstdint>
#include<iostream>
#include "async_msg/protocol.hpp"

int main()
{
    std::uint32_t original_length = 1234;
    std::uint32_t encoded = async_msg::encode_length(original_length);
    std::uint32_t decoded = async_msg::decode_length(encoded);
    assert(decoded == original_length);
    std::cout<<"Protocol test passed \n";
}