#include"async_msg/server.hpp"
#include "async_msg/protocol.hpp"

#include<cerrno>
#include<cstring>
#include<iostream>

#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<unistd.h>

namespace async_msg
{
    server::server(int port):port_(port)
    {
    }
int server::port()const
{
return port_;
}
int server::run()
{
   int server_fd = socket(AF_INET, SOCK_STREAM,0);

   if(server_fd == -1)
   {
    std::cerr<<"socket() failed: "<<std::strerror(errno)<<'\n';
    return 1;
   }

   sockaddr_in server_address{};

   server_address.sin_family = AF_INET;
   server_address.sin_addr.s_addr = INADDR_ANY;
   server_address.sin_port = htons(port_);

   if(bind(server_fd, reinterpret_cast<sockaddr*>(&server_address),sizeof(server_address))==-1)
   {
    std::cerr<<"bind() failed: "<<std::strerror(errno)<<'\n';
    close(server_fd);
    return 1;
   }

   if(listen(server_fd,10)==-1)
   {
    std::cerr<<"listen() failed: "<<std::strerror(errno)<<'\n';
    close(server_fd);
    return 1;
   }
   std::cout<<"Server listening on port "<<port_<<"..\n";
   while(true){
   
   int client_fd = accept(server_fd, nullptr,nullptr);

   if(client_fd == -1)
   {
    std::cerr<<"accept() failed: "<<std::strerror(errno)<<'\n';
    continue;
   }
   std::cout<<"Client connected:\n";
   
    while(true)
    {
        std::string message;

   async_msg::RecvResult result = async_msg::recv_frame(client_fd, message);
   
   if(result == async_msg::RecvResult::Success)
   {
    std::cout<<"Received: "<<message<<'\n';
   }

   else if(result == async_msg::RecvResult::Disconnected)
   {
    std::cout<<"Client disconnected..\n";
    break;   
   }
   else{
    std::cerr<<"Failed to receive message..\n";
    break;
   }
}
   close(client_fd);
   }
    return 0;
}
}//namespace async_msgs
