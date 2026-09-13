#include "async_msg/protocol.hpp"
#include<arpa/inet.h>

namespace async_msg
{
    std::uint32_t encode_length(std::uint32_t length)
    {
        return htonl(length);
    }

    std::uint32_t decode_length(std::uint32_t encoded_length)
    {
        return ntohl(encoded_length);
    }
} //namespace async_msg