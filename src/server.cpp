#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

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
    while(true)
    {
    char buffer[1024]{};

    ssize_t bytes_received = recv(
        client_fd,
        buffer,
        sizeof(buffer)-1,
        0
    );

    if(bytes_received == -1)
    {
        std::cerr << "recv() failed: "<< std::strerror(errno)<<'\n';
        //close(client_fd);
        //close(server_fd);
       // return 1;
     //  continue;
     break;
    }

    if(bytes_received == 0)
    {
        std::cout<<"Client disconnected\n";
        break;
    }
    std::cout<<"Received: "<< buffer << '\n';
    //close(client_fd);
    }
    close(client_fd);
}
close(server_fd);
return 0;
}

