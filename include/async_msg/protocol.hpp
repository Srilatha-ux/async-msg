#pragma once

#include<cstdint>

namespace async_msg
{
    constexpr std::uint32_t HEADER_SIZE = 4;
    std::uint32_t encode_length(std::uint32_t length);
    std::uint32_t decode_length(std::uint32_t encoded_length);
} //namespace async_msg