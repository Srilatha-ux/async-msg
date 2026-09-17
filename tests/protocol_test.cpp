#include "async_msg/protocol.hpp"
#include<cassert>
#include<cstdint>
#include<cstring>
#include<iostream>
#include<sys/socket.h>
#include<unistd.h>

int main()
{
    std::uint32_t original_length = 1234;
    std::uint32_t encoded = async_msg::encode_length(original_length);
    std::uint32_t decoded = async_msg::decode_length(encoded);
    assert(decoded == original_length);

    int sockets[2];

    int result = socketpair(AF_UNIX,SOCK_STREAM,0,sockets);

    assert(result==0);

    const char* message = "hello async-msg";

    bool sent = async_msg::send_all(sockets[0],message,std::strlen(message));

    assert(sent);

    char buffer[64]{};

    //bool received = async_msg::recv_all(sockets[1],buffer,std::strlen(message)
    async_msg::RecvResult received = async_msg::recv_all(sockets[1],buffer,std::strlen(message));
    assert(received == async_msg::RecvResult::Success);

    assert(std::strcmp(buffer,message)==0);
    close(sockets[0]);
    close(sockets[1]);
    std::cout<<"Protocol test passed \n";

    return 0;
}