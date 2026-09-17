#include "async_msg/protocol.hpp"
#include<arpa/inet.h>
#include<cerrno>
#include<sys/socket.h>

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
    
    bool send_all(int socket_fd, const void* data, std::size_t size)
    {
        const auto* buffer = 
          static_cast<const char*>(data);
        
        std::size_t total_sent = 0;

        while(total_sent < size)
        {
            ssize_t bytes_sent = send(socket_fd, buffer+total_sent, size-total_sent,0);

            if(bytes_sent == -1)
            {
                if(errno==EINTR)
                {
                    continue;
                }
                return false;
            }

        if(bytes_sent==0)
        {
            return false;
        }
        total_sent+=static_cast<std::size_t>(bytes_sent);
    }
    return true;
    }

    RecvResult recv_all(int socket_fd, void* data, std::size_t size)
    {

        auto* buffer = static_cast<char*>(data);
        std::size_t total_received = 0;

        while(total_received < size)
        {
            ssize_t bytes_received = recv(socket_fd,buffer+total_received,size-total_received,0);

            if(bytes_received == -1)
            {
                if(errno == EINTR)
                {
                    continue;
                }
                return RecvResult::Error;
            }

            if(bytes_received == 0)
            {
                return RecvResult::Disconnected;
            }

            total_received+=static_cast<std::size_t>(bytes_received);
        }
        return RecvResult::Success;
    }

    bool send_frame(int socket_fd, const std::string& message)
    {
        std::uint32_t message_length = static_cast<std::uint32_t>(message.size());

        std::uint32_t encoded_length = encode_length(message_length);

        bool header_sent = send_all(socket_fd,&encoded_length, HEADER_SIZE);

        if(!header_sent)
        {
            return false;
        }

        return send_all(socket_fd, message.data(),message.size());
    }

    RecvResult recv_frame(int socket_fd,std::string& message)
    {
        constexpr std::uint32_t MAX_MESSAGE_SIZE = 4096;

        std::uint32_t encoded_length = 0;

        RecvResult header_result = recv_all(socket_fd, &encoded_length,HEADER_SIZE);

        if(header_result != RecvResult::Success)
        {
            return header_result;
        }

        std::uint32_t message_length = decode_length(encoded_length);

        if(message_length == 0)
        {
            return RecvResult::Error;
        }

        if(message_length > MAX_MESSAGE_SIZE)
        {
            return RecvResult::Error;
        }

        message.resize(message_length);

        RecvResult payload_result = recv_all(socket_fd,message.data(),message_length);

        if(payload_result != RecvResult::Success)
        {
            message.clear();
            return payload_result;
        }
        return RecvResult::Success;
    }
} //namespace async_msg