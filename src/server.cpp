#include<string>
#include "async_msg/protocol.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
//#include"protocol.cpp"
int run_server()
{
    int server_fd = socket(AF_INET,SOCK_STREAM, 0);

    if(server_fd == -1)
    {
        std::cerr << "socket() failed: "<< std::strerror(errno)<<'\n';
        return 1;
    }

    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(8080);

    if(bind(
        server_fd,
        reinterpret_cast<sockaddr*>(&server_address),
        sizeof(server_address))==-1)
    {
        std::cerr<<"bind() failed: "<<std::strerror(errno)<<'\n';
        close(server_fd);
        return 1;
    }
    if(listen(server_fd,10)== -1)
    {
        std::cerr<<"listen() failed: "<<std::strerror(errno)<<'\n';
        close(server_fd);
        return 1;
    }
    std::cout<< "Server listening on port 8080...\n";

    while(true)
    {
    int client_fd = accept(server_fd, nullptr, nullptr);

    if(client_fd == -1)
    {
        std::cerr<<"accept() failed: "<<std::strerror(errno)<<'\n';
        continue;
        //close(server_fd);
        //return 1;
    }
    std::cout<<"Client connected!\n";
   
    constexpr std::uint32_t MAX_MESSAGE_SIZE = 4096;

    while(true)
    {
        std::uint32_t encoded_length = 0;
   
async_msg::RecvResult header_result = 
async_msg::recv_all(
     client_fd,
            &encoded_length,
            async_msg::HEADER_SIZE
);

if(header_result == async_msg::RecvResult::Disconnected)
{
    std::cout<<"Client disconnected\n";
    break;
}

if(header_result==async_msg::RecvResult::Error)
{
    std::cerr<<"Failed to receive message header\n";
    break;

}
        std::uint32_t message_length= async_msg::decode_length(encoded_length);

        if(message_length==0)
        {
            std::cerr<<"Invalid message: empty payload\n";
            break;
        }
        if(message_length>MAX_MESSAGE_SIZE)
        {
            std::cerr<<"Message too large: "<<message_length<<" bytes\n";
            break;
        }
        std::string message(message_length,'\0');


    async_msg::RecvResult payload_result = async_msg::recv_all(
        client_fd,
            message.data(),
            message_length
    );

    if(payload_result==async_msg::RecvResult::Disconnected)
    {
        std::cout<<"Client disconnected during message\n";
break;
    }

    if(payload_result==async_msg::RecvResult::Error)
    {
        std::cout<<"Failed to receive message payload\n";
break;
    }
    std::cout<<"Received:"<<message<<'\n';
}   
close(client_fd);
}
close(server_fd);
return 0;
}

