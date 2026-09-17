#include "async_msg/protocol.hpp"

#include<arpa/inet.h>
#include<cassert>
#include<cstring>
#include<iostream>
#include<string>
#include<sys/socket.h>
#include<unistd.h>

int main()
{
    int socket_fd = socket(AF_INET,SOCK_STREAM,0);

    assert(socket_fd!=-1);

    sockaddr_in server_address{};

    server_address.sin_family=AF_INET;
    server_address.sin_port= htons(8080);

    int result = inet_pton(
        AF_INET,
        "127.0.0.1",
        &server_address.sin_addr
    );

    assert(result==1);
    result=connect(socket_fd, reinterpret_cast<sockaddr*>(&server_address),sizeof(server_address));

    assert(result==0);

    //std::string message = "hello async-msg";
std::string messages[] = {"hello async-msg",
"Second Message",
"hello hey"};

for(const std::string& message:messages)
{
    std::uint32_t encoded_length = async_msg::encode_length(static_cast<std::uint32_t>(message.size()));

    bool header_sent = async_msg::send_all(socket_fd,&encoded_length,async_msg::HEADER_SIZE);

    assert(header_sent);

    bool payload_sent = async_msg::send_all(socket_fd,message.data(),message.size());

    assert(payload_sent);

    std::cout<<"Framed message sent\n";
}
    close(socket_fd);

    return 0;
}