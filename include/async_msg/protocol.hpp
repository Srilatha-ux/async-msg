#pragma once

#include<cstddef>
#include<cstdint>

namespace async_msg
{
    constexpr std::uint32_t HEADER_SIZE = 4;
    std::uint32_t encode_length(std::uint32_t length);
    std::uint32_t decode_length(std::uint32_t encoded_length);

    bool send_all(int socket_fd, const void* data, std::size_t size);
    bool recv_all(int socket_fd, void* data, std::size_t size);
} //namespace async_msg